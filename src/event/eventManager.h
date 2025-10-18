#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <unordered_map>
#include <vector>

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
                hs = handlers; // 拍一份快照，避免遍历期间被修改
            }
            while (!local.empty())
            {
                TEvent e = std::move(local.front());
                local.pop();
                for (auto& cb : hs)
                {
                    if (cb && cb(e))
                        break; // 返回 true 则停止传播；不需要就改为 void 并去掉判断
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
        p->data.enqueue(e); // 每类型内锁，线程安全
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

    void processAll()
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
    // 你的 HashMap/SmallVector 若为自定义容器可继续用；这里用标准容器同理
    std::unordered_map<TypeID, std::unique_ptr<TypeErased>> map_;
};