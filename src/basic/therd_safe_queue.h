#pragma once

#include <thread>
#include <deque>
#include <mutex>
#include <memory>

#include <boost/noncopyable.hpp>

template<typename T>
class ThreadSafeQueue : private boost::noncopyable
{
public:

    ThreadSafeQueue() = default;
    ThreadSafeQueue(const ThreadSafeQueue<T>&) = delete; // noncopiable
    ~ThreadSafeQueue(){clear();}

    const T& front() const
    {
        std::scoped_lock lock(m_mutex);
        return m_data.front();
    }

    const T& back() const
    {
        std::scoped_lock lock(m_mutex);
        return m_data.back();
    }

    bool empty() const
    {
        std::scoped_lock lock(m_mutex);
        return m_data.empty();
    }
    
    size_t size() const
    {
        std::scoped_lock lock(m_mutex);
        return m_data.size();
    }

    T pop_front()
    {
        std::scoped_lock lock(m_mutex);
        T retVal = std::move(m_data.front());
        m_data.pop_front();
        return m_data.front();
    }


protected:
    
    mutable std::mutex m_mutex;
    std::deque<T> m_data;
};
