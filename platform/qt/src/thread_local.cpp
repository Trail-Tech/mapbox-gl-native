#include <mbgl/util/thread_local.hpp>

#include <mbgl/actor/scheduler.hpp>
#include <mbgl/renderer/backend_scope.hpp>

#include <array>
#include <cassert>

#include <QThreadStorage>

namespace mbgl {
namespace util {

template <class T>
class ThreadLocal<T>::Impl {
public:
    QThreadStorage<std::array<T*, 1>> local;
};

template <class T>
ThreadLocal<T>::ThreadLocal() : impl(std::make_unique<Impl>()) {
    set(nullptr);
}

template <class T>
ThreadLocal<T>::~ThreadLocal() {
    // ThreadLocal will not take ownership
    // of the pointer it is managing. The pointer
    // needs to be explicitly cleared before we
    // destroy this object.
    //
    // XXX jbrooks: but that isn't happening on exit somewhere, and
    // having this assert fire turns every exit into a crash. I'm
    // not too concerned about freeing resources at-exit, and it
    // doesn't seem like there's any other destruction path for this.

    //assert(!get());
}

template <class T>
T* ThreadLocal<T>::get() {
    return impl->local.localData()[0];
}

template <class T>
void ThreadLocal<T>::set(T* ptr) {
   impl->local.localData()[0] = ptr;
}

template class ThreadLocal<Scheduler>;
template class ThreadLocal<BackendScope>;
template class ThreadLocal<int>; // For unit tests

} // namespace util
} // namespace mbgl
