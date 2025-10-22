#include "eventManager.h"

namespace ana
{

void EventManager::processAll()
{
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
