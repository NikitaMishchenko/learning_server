#pragma once

#include <thread>
#include <memory>
#include <deque>

#include "thread_safe_queue.h"
#include "owned_message.h"
#include "connection.h"

namespace tcp_communication
{
    template <typename T>
    class IServer
    {
    public:
        IServer(uint16_t port)
            : m_asioAcceptor(m_asioContext, boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), port))
        {
        }

        virtual ~IServer()
        {
            stop();
        }

        virtual bool start()
        {
            std::cout << "[SERVER] starting at endpoint: " << m_asioAcceptor.local_endpoint() << "\n";

            try
            {
                waitForClientConnection(); // work before

                m_contextThread = std::thread([this](){
                                                        std::cout << "waiting for clients...\n";
                                                        m_asioContext.run();
                                                     });
            
            }
            catch(const std::exception& e)
            {
                std::cerr << "[SERVER] throwed an exception " <<  e.what() << '\n';
                return false;    
            }

            std::cout << "[SERER] Started\n";
            return true;
        }

        virtual void stop()
        {
            m_asioContext.stop();

            if (m_contextThread.joinable()) m_contextThread.join();

            std::cout << "[SERVER] Stopped\n";
        }

        // for ASIO context
        void waitForClientConnection()
        {
            std::cout << "waitForClientConnection entry\n";

            m_asioAcceptor.async_accept(
                [this](boost::system::error_code errCode, boost::asio::ip::tcp::socket socket)
                {
                    if (!errCode)
                    { 
                        std::cout << "[SERVER] New Connection success: " << socket.remote_endpoint() << "\n";

                        /* testing
                        std::shared_ptr<Connection<T>> = newConnection = 
                            std::make_shared<Connection<T>>(Connection<T>::owner::server, m_asioContext, std::moove(socket), m_msgsIn);
                        

                        // possible connection deny
                        if (onClientConnetc(newConnection))    
                        {
                            m_connections.push_back(std::move(newConnection));

                            m_connections.back()->ConnectToClient(m_clientIdCounter++);

                            std::cout << "[" << m_connections.back()->GetId() << "] Connection Approved\n";
                        }
                        else
                        {
                            std::cout << "[-----] Connection Denied\n";
                        }
                        */
                    }
                    else
                    {
                        std::cout << "[SERVER] New Connection failed, errCode: " << errCode.message() << "\n";
                    }

                    waitForClientConnection(); // next task
                }
            );
        }

        void messageClient(std::shared_ptr<Connection<T>> client, const Message<T> &msg)
        {
            if (client && client->isConnected())
            {
                client->send(msg);
            }
            else
            {
                onClientDisconnect(client);
                client.reset();
                m_connections.erase( 
                    std::remove(m_connections.begin(), m_connections.end(), client),  m_connections.end());
            }
        }

        void messageAllClients(const Message<T> &msg, std::shared_ptr<Connection<T>> clientToIgnore = nullptr)
        {
            bool isAnyInvalidCLientsExists = false;

            for (auto& client : m_connections)
            {
                if (client && client->isConnected())
                {
                    if (client != clientToIgnore)
                        client->send(msg);
                }
                else
                {
                    onClientDisconnect(client);
                    client.reset();
                    isAnyInvalidCLientsExists = true;
                }
            }

            if (isAnyInvalidCLientsExists) // todo check here
                m_connections.erase( 
                    std::remove(m_connections.begin(), m_connections.end(), nullptr),  m_connections.end());
        }

        // handle incomming message
        void update(size_t nMaxMessages = -1) ///< max at default
        {
            size_t nMessageCount = 0;
            while (nMessageCount < nMaxMessages && !m_msgsIn.empty())
            {
                auto msg = m_msgsIn.pop_front();

                onMessage(msg.remote, msg.msg); // pass to msg hanler

                nMessageCount++;
            }
        }

    protected:
        virtual bool onClientConnect(std::shared_ptr<Connection<T>> client)
        {
            return false;
        }

        virtual void onClientDisconnect(std::shared_ptr<Connection<T>> client)
        {
        }

        virtual bool onMessage(std::shared_ptr<Connection<T> > client, const Message<T> &msg)
        {
        }

    private:
        ThreadSafeQueue<OwnedMessage<T>> m_msgsIn;

        std::deque<std::shared_ptr<Connection<T>>> m_connections; ///< validated conncections

        boost::asio::io_context m_asioContext;
        std::thread m_contextThread;

        boost::asio::ip::tcp::acceptor m_asioAcceptor;

        uint32_t m_clientIdCounter = 10000; // unique for each client

    };

}