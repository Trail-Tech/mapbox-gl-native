#include <mbgl/util/thread_local.hpp>

#include <array>
#include <cassert>

#include <QThreadStorage>

namespace mbgl {
namespace util {
namespace impl {

// QThreadStorage tries to be smart and take ownership of the data, which we don't want. So we're
// wrapping it in another type which doesn't own the pointer it contains.
using StorageType = QThreadStorage<std::array<void*, 1>>;

ThreadLocalBase::ThreadLocalBase() {
    static_assert(sizeof(storage) >= sizeof(StorageType), "storage is too small");
    static_assert(alignof(decltype(storage)) % alignof(StorageType) == 0, "storage is incorrectly aligned");
    new (&reinterpret_cast<StorageType&>(storage)) QThreadStorage<void*>();
}

ThreadLocalBase::~ThreadLocalBase() {
    // ThreadLocal will not take ownership of the pointer it is managing. The pointer
    // needs to be explicitly cleared before we destroy this object.
    //
    // XXX jbrooks: but that isn't happening on exit somewhere, and
    // having this assert fire turns every exit into a crash. I'm
    // not too concerned about freeing resources at-exit, and it
    // doesn't seem like there's any other destruction path for this.
    //assert(!get());
    reinterpret_cast<StorageType&>(storage).~QThreadStorage();
}

void* ThreadLocalBase::get() {
    return reinterpret_cast<StorageType&>(storage).localData()[0];
}

void ThreadLocalBase::set(void* ptr) {
    reinterpret_cast<StorageType&>(storage).setLocalData({{ ptr }});
}

} // namespace impl
} // namespace util
} // namespace mbgl
