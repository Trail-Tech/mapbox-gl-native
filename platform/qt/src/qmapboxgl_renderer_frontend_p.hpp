#pragma once

#include <mbgl/renderer/renderer_backend.hpp>
#include <mbgl/renderer/renderer_frontend.hpp>

#include <QObject>

namespace mbgl {
    class Renderer;
} // namespace mbgl

class QMapboxGLRendererFrontend : public QObject, public mbgl::RendererFrontend
{
    Q_OBJECT

public:
    explicit QMapboxGLRendererFrontend(std::unique_ptr<mbgl::Renderer>, mbgl::RendererBackend&);
    ~QMapboxGLRendererFrontend() override;
    
    void reset() override;
    void setObserver(mbgl::RendererObserver&) override;

    void update(std::shared_ptr<mbgl::UpdateParameters>) override;

    mbgl::Renderer* getRenderer();

public slots:
    void render();

signals:
    void updated();
    
private:
    std::unique_ptr<mbgl::Renderer> renderer;
    mbgl::RendererBackend& backend;
    std::shared_ptr<mbgl::UpdateParameters> updateParameters;
};
