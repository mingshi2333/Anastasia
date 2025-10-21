#include "common/hash.h"
#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <vector>

namespace ana

{
class EventManager
{

    template <typename TEvent>
    struct EventData
    {
        std::mutex mtx;
        std::queue<TEvent> events;
        std::vector<std::function<bool(const TEvent&)>> handlers;

        void enqueue(const TEvent& e)
        {
            std::scoped_lock lk(mtx);
            events.push(e);
        }

        // 快照处理：不持锁执行用户回调
        void processSnapshot()
        {
            std::queue<TEvent> local;
            std::vector<std::function<bool(const TEvent&)>> hs;
            {
                std::scoped_lock lk(mtx);
                std::swap(local, events);
                hs = handlers;
            }
            while (!local.empty())
            {
                TEvent e = std::move(local.front());
                local.pop();
                for (auto& cb : hs)
                {
                    if (cb && cb(e))
                        break;
                }
            }
        }
    };

    struct TypeErased
    {
        virtual ~TypeErased()  = default;
        virtual void process() = 0;
    };

    template <typename TEvent>
    struct TypedEventData : TypeErased
    {
        EventData<TEvent> data;

        void process() override
        {
            data.processSnapshot();
        }
    };

public:
    EventManager() = default;

    template <typename TEvent>
    void pushEvent(const TEvent& e)
    {
        auto* p = getOrCreate<TEvent>();
        p->data.enqueue(e);
    }

    template <typename TEvent>
    void registerEvent(std::function<bool(const TEvent&)>&& func)
    {
        auto* p = getOrCreate<TEvent>();
        {
            std::scoped_lock lk(p->data.mtx);
            p->data.handlers.push_back(std::move(func));
        }
    }

    void processAll();

private:
    using TypeID = size_t;

    template <typename T>
    static TypeID getTypeID()
    {
        static TypeID id = nextTypeID();
        return id;
    }

    static TypeID nextTypeID()
    {
        static std::atomic<TypeID> counter{ 0 };
        return counter.fetch_add(1, std::memory_order_relaxed);
    }

    template <typename TEvent>
    TypedEventData<TEvent>* getOrCreate()
    {
        const auto id = getTypeID<TEvent>();
        std::scoped_lock lk(mapMutex_);
        auto it = map_.find(id);
        if (it == map_.end())
        {
            auto up   = std::make_unique<TypedEventData<TEvent>>();
            auto* ptr = up.get();
            map_.emplace(id, std::move(up));
            return ptr;
        }
        return static_cast<TypedEventData<TEvent>*>(it->second.get());
    }

private:
    std::mutex mapMutex_; // 仅保护 map_ 的增删查
    HashMap<TypeID, std::unique_ptr<TypeErased>> map_;
};

} // namespace ana
