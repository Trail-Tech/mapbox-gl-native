#include "http_file_source.hpp"
#include "http_request.hpp"

#include <mbgl/util/logging.hpp>

#include <QByteArray>
#include <QDir>
#include <QNetworkProxyFactory>
#include <QNetworkReply>
#include <QSslConfiguration>

// Needs to be on the global namespace
// for linking purposes.
void initResources() {
    Q_INIT_RESOURCE(qmapbox);
}

namespace mbgl {

HTTPFileSource::Impl::Impl() : m_manager(new QNetworkAccessManager(this))
{
    initResources();

    QNetworkProxyFactory::setUseSystemConfiguration(true);

#if QT_VERSION >= 0x050000
    m_ssl.setProtocol(QSsl::SecureProtocols);
#else
    // Qt 4 defines SecureProtocols as TLS1 or SSL3, but we don't want SSL3.
    m_ssl.setProtocol(QSsl::TlsV1);
#endif

    m_ssl.setCaCertificates(QSslCertificate::fromPath(":ca-bundle.crt"));
    if (m_ssl.caCertificates().isEmpty()) {
        mbgl::Log::Warning(mbgl::Event::HttpRequest, "Could not load list of certificate authorities");
    }
}

void HTTPFileSource::Impl::request(HTTPRequest* req)
{
    QUrl url = req->requestUrl();

    QPair<QNetworkReply*, QVector<HTTPRequest*>>& data = m_pending[url];
    QVector<HTTPRequest*>& requestsVector = data.second;
    requestsVector.append(req);

    if (requestsVector.size() > 1) {
        return;
    }

    QNetworkRequest networkRequest = req->networkRequest();
    networkRequest.setSslConfiguration(m_ssl);

    data.first = m_manager->get(networkRequest);
    connect(data.first, SIGNAL(finished()), this, SLOT(onReplyFinished()));
    connect(data.first, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onReplyFinished()));
}

void HTTPFileSource::Impl::cancel(HTTPRequest* req)
{
    QUrl url = req->requestUrl();

    auto it = m_pending.find(url);
    if (it == m_pending.end()) {
        return;
    }

    QPair<QNetworkReply*, QVector<HTTPRequest*>>& data = it.value();
    QNetworkReply* reply = data.first;
    QVector<HTTPRequest*>& requestsVector = data.second;

    for (int i = 0; i < requestsVector.size(); ++i) {
        if (req == requestsVector.at(i)) {
            requestsVector.remove(i);
            break;
        }
    }

    if (requestsVector.empty()) {
        m_pending.erase(it);
#if QT_VERSION >= 0x050000
        reply->abort();
#else
        // XXX: We should be aborting the reply here
        // but a bug on Qt4 causes the connection of
        // other ongoing requests to drop if we call
        // abort() too often (and we do).
        Q_UNUSED(reply);
#endif
    }
}

void HTTPFileSource::Impl::onReplyFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());
    const QUrl& url = reply->url();

    auto it = m_pending.find(url);
    if (it == m_pending.end()) {
        reply->deleteLater();
        return;
    }

    QVector<HTTPRequest*>& requestsVector = it.value().second;
    for (auto req : requestsVector) {
        req->handleNetworkReply(reply);
    }

    m_pending.erase(it);
    reply->deleteLater();
}

HTTPFileSource::HTTPFileSource()
    : impl(std::make_unique<Impl>()) {
}

HTTPFileSource::~HTTPFileSource() = default;

std::unique_ptr<AsyncRequest> HTTPFileSource::request(const Resource& resource, Callback callback)
{
    return std::make_unique<HTTPRequest>(impl.get(), resource, callback);
}

uint32_t HTTPFileSource::maximumConcurrentRequests() {
#if QT_VERSION >= 0x050000
    return 20;
#else
    return 10;
#endif
}

} // mbgl
