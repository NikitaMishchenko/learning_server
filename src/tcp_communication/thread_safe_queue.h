#pragma once

#include <thread>
#include <deque>
#include <mutex>
#include <memory>

#include <boost/noncopyable.hpp>

namespace tcp_communication
{
    template<typename T>
    class ThreadSafeQueue : private boost::noncopyable
    {
    public:

        ThreadSafeQueue() = default;
        ThreadSafeQueue(const ThreadSafeQueue<T>&) = delete; // noncopiable
        ~ThreadSafeQueue()
        {
            clear();
        }

        const T& front() const
        {
            std::scoped_lock lock(m_mutexQ);
            return m_data.front();
        }

        const T& back() const
        {
            std::scoped_lock lock(m_mutexQ);
            return m_data.back();
        }

        bool empty() const
        {
            std::scoped_lock lock(m_mutexQ);
            return m_data.empty();
        }
        
        size_t size() const
        {
            std::scoped_lock lock(m_mutexQ);
            return m_data.size();
        }

        void clear()
        {
            std::scoped_lock lock(m_mutexQ);
            m_data.clear();
        }

        T pop_front()
        {
            std::scoped_lock lock(m_mutexQ);
            T retVal = std::move(m_data.front());
            m_data.pop_front();
            return m_data.front();
        }

        void push_back(const T& input)
        {
            std::scoped_lock(m_mutexQ);
            
            m_data.emplace_back(std::move(input));
            
            std::unique_lock<std::mutex> ul(m_mutex);
            cvBlocking.notify_one();
        }

        void push_front(const T& input)
        {
            std::scoped_lock(m_mutexQ);
            
            m_data.emplace_front(std::move(input));
            
            std::unique_lock<std::mutex> ul(m_mutex);
            cvBlocking.notify_one();
        }

    protected:
        mutable std::mutex m_mutexQ;
        mutable std::mutex m_mutex;
        std::condition_variable cvBlocking;
        std::deque<T> m_data;
    };

} // namespace tcp_communication