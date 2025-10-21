#include "eventManager.h"

namespace ana
{

void EventManager::processAll()
{
    // 先拍快照，再逐个处理，避免持全局锁执行回调
    std::vector<TypeErased*> list;
    {
        std::scoped_lock lk(mapMutex_);
        list.reserve(map_.size());
        for (auto& kv : map_)
            list.push_back(kv.second.get());
    }
    for (auto* p : list)
        p->process();
}

} // namespace ana
