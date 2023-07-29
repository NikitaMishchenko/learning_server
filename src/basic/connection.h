#pragma once

#include <memory>
#include <utility>

#include <boost/asio.hpp>

#include "owned_message.h"
#include "message.h"
#include "thread_safe_queue.h"

namespace tcp_communication
{
    template <typename T>
    class Connection : public std::enable_shared_from_this<Connection<T>>
    {
    public:
        enum Owner
        {
            SERVER,
            CLIENET
        };

        Connection(Owner owner,
                   boost::asio::io_context &asioContext,
                   boost::asio::ip::tcp::socket socket,
                   ThreadSafeQueue<OwnedMessage<T>> &msgQueueIn)
            : m_asioContext(asioContext), m_socket(std::move(socket)), m_messagesIn(msgQueueIn)
        {
            m_owner = owner;
        }
        virtual ~Connection() {}

        void connectToClient(uint32_t uid = 0)
        {
            if (m_owner == Owner::SERVER)
            {
                if (m_socket.is_open())
                {
                    m_id = uid;
                }
            } 
        }

        bool connectToServer(uint32_t uid = 0)
        {
            if (m_owner == Owner::CLIENET)
            {
                if (m_socket.is_open())
                {
                    m_id = uid;
                }
            }
        }

        bool disconnect()
        {
            return false;
        }

        bool isConnected()
        {
            return false;
        }

        uint32_t getId() { return m_id; }

    protected:
        boost::asio::ip::tcp::socket m_socket;

        boost::asio::io_context &m_asioContext;

        ThreadSafeQueue<Message<T>> m_messagesOut;
        ThreadSafeQueue<OwnedMessage<T>> &m_messagesIn; // provided from outside

        Owner m_owner = Owner::SERVER;
        uint32_t m_id = 0;
    };

} // tcp_communication
