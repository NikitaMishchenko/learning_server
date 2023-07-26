#pragma once

#include <memory>

#include <boost/asio.hpp>

#include "owned_message.h"
#include "message.h"
#include "thread_safe_queue.h"

namespace tcp_communication
{
    template <typename T>
    class Connection : public std::enable_shared_from_this<Connection<T> >
    {
    public:
        Connection() {}
        virtual ~Connection() {}


        bool ConnectToServer(){};
        bool Disconnect(){};
        bool IsConnected(){};

    protected:
        boost::asio::ip::tcp::socket m_socket;

        boost::asio::io_context& m_asioContext;

        ThreadSafeQueue<Message<T> > m_messagesOut;
        ThreadSafeQueue<Message<T> >& m_messagesIn; // provided from outside
    };

} // tcp_communication
