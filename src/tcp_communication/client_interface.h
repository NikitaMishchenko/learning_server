#pragma once

#include <thread>
#include <memory>
#include <string>

#include <boost/asio.hpp>

#include "thread_safe_queue.h"
#include "owned_message.h"
#include "connection.h"

namespace tcp_communication
{
    template <typename T>
    class IClient
    {
    public:
        IClient() : m_socket(m_context) {}
        virtual ~IClient() { disconnect(); }

        virtual bool connect(const std::string &host, const uint16_t port)
        {
            try
            {
                boost::asio::ip::tcp::resolver resolver(m_context);
                boost::asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));

                m_connectionPtr = std::make_unique<Connection<T>>(Connection<T>::Owner::CLIENT, 
                                                                  m_context,
                                                                  boost::asio::ip::tcp::socket(m_context),
                                                                  m_messagesIn);
                
                m_connectionPtr->connectToServer(endpoints);

                m_threadContext = std::thread([this]()
                                            { m_context.run(); });
            }
            catch (const std::exception &e)
            {
                std::cerr << "Client connect exception occurred: " << e.what() << '\n';
                return false;
            }

            return true;
        }

        virtual void disconnect()
        {
            if (isConnected())
                disconnect();

            m_context.stop();

            if (m_threadContext.joinable())
                m_threadContext.join();

            m_connectionPtr.release();
        }

        virtual bool isConnected()
        {
            return m_connectionPtr && m_connectionPtr->isConnected();
        }

        virtual ThreadSafeQueue<OwnedMessage<T>> &incomingMsgs()
        {
            return m_messagesIn;
        }

        void send(const Message<T> msg)
        {
            if (isConnected())
            {
                std::cout << "client: sending msg, headerId: " << int(msg.m_header.id) << "\n";
                m_connectionPtr->send(msg);
            }
        }

        ThreadSafeQueue<OwnedMessage<T>>& getMsgsIn()
        {
            return m_messagesIn;
        }

    protected:
        boost::asio::io_context m_context;

        std::thread m_threadContext;

        boost::asio::ip::tcp::socket m_socket;

        std::unique_ptr<Connection<T>> m_connectionPtr;

    private:
        ThreadSafeQueue<OwnedMessage<T>> m_messagesIn;
    };

} // tcp_communication
