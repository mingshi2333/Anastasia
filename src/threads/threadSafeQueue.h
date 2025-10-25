#include <atomic>
#include <concepts>
#include <condition_variable>
#include <cstddef>
#include <deque>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <syncstream>
#include <thread>
#include <utility>
#include <vector>

namespace ana
{

template <typename Lock>
concept is_lockable = requires(Lock&& lock) {
    lock.lock();
    lock.unlock();
    { lock.try_lock() } -> std::convertible_to<bool>;
};

template <typename T, typename Lock = std::mutex>
    requires is_lockable<Lock>
class ThreadSafeQueue
{
public:
    using value_type = T;
    using size_type  = std::deque<T>::size_type;

    void push_back(T&& value)
    {
        std::scoped_lock lck{ mutex_ };
        data_.push_back(value);
    }

    void push_front(T&& value)
    {
        std::scoped_lock lck{ mutex_ };
        data_.push_front(value);
    }

    [[nodiscard]] bool empty()
    {
        std::scoped_lock lck{ mutex_ };
        return data_.empty();
    }

    [[nodiscard]] std::optional<T> pop_back()
    {
        std::scoped_lock lck{ mutex_ };
        if (data_.empty())
        {
            return std::nullopt;
        }
        auto back = std::move(data_.back());
        data_.pop_back();
        return back;
    }

    [[nodiscard]] std::optional<T> pop_front()
    {
        std::scoped_lock lck{ mutex_ };

        if (data_.empty())
        {
            return std::nullopt;
        }
        auto front = std::move(data_.front());
        data_.pop_front();
        return front;
    }

    [[nodiscard]] std::optional<T> steal()
    {
        std::scoped_lock lck(mutex_);
        if (data_.empty())
        {
            return std::nullopt;
        }

        auto back = std::move(data_.back());
        data_.pop_back();
        return back;
    }

    void rotate_to_front(T const& item)
    {
        std::scoped_lock lck(mutex_);
        auto iter = std::find(data_.begin(), data_.end(), item);

        if (iter != data_.end())
        {
            std::ignore = data_.erase(iter);
        }

        data_.push_front(item);
    }

    [[nodiscard]] std::optional<T> copy_front_and_rotate_to_back()
    {
        std::scoped_lock lck(mutex_);

        if (data_.empty())
        {
            return std::nullopt;
        }

        auto front = data_.front();
        data_.pop_front();

        data_.push_back(front);

        return front;
    }

private:
    mutable std::mutex mutex_{};
    std::deque<T> data_{};
};
} // namespace ana
