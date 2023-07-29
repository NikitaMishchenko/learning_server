#pragma once

#include <memory>
#include <utility>

#include <boost/asio.hpp>

#include "owned_message.h"
#include "tcp_message.h"
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

                    readHeader();
                }
            } 
        }

        void connectToServer(const boost::asio::ip::tcp::resolver::results_type& endpoints)
        {
            if (m_owner == Owner::CLIENET)
            {
                boost::asio::async_connect(m_socket, endpoints,
                                           [this](boost::system::error_code errCode, boost::asio::ip::tcp::endpoint endpoint)
                                           {
                                                if(!errCode)
                                                {
                                                    readHeader();
                                                }
                                           });

            }
        }

        void disconnect()
        {
            if (isConnected())
                boost::asio::post(m_asioContext, [this]() {m_socket.close();});
        }

        bool isConnected() const
        {
            return m_socket.is_open();
        }

        uint32_t getId() { return m_id; }

        void send(const Message<T>& msg)
        {
            boost::asio::post(m_asioContext, 
                [this, msg]()
                {
                    bool isWritingMessageNow = m_messagesOut.size();
                    m_messagesOut.push_back(msg);
                    
                    if (isWritingMessageNow)
                        writeHeader();
                });
        }

private:
    void readHeader()
    {
        boost::asio::async_read(m_socket, boost::asio::buffer(&m_tmpMsgIn.m_header, sizeof(Header<T>)),
            [this](std::error_code errCode, std::size_t length)
            {
                if (!errCode)
                {
                    if (m_tmpMsgIn.m_header.size)
                    {
                        m_tmpMsgIn.m_body.resize(m_tmpMsgIn.m_header.size);
                        readBody();
                    }
                    else
                    {
                        addToIncomingMessageQueue();
                    }
                }
                else
                {
                    std::cout << "[" << m_id << "] Read Header Fail!\n";
                    m_socket.close();
                }

            });
    }

    void readBody()
    {
        boost::asio::async_read(m_socket, boost::asio::buffer(m_tmpMsgIn.m_body.data(), m_tmpMsgIn.m_body.size()),
            [this](std::error_code errCode, std::size_t length)
            {
                if (!errCode)
                {
                    addToIncomingMessageQueue();
                }
                else
                {
                    std::cout << "[" << m_id << "] Read Body Fail!\n";
                    m_socket.close();
                }

            });
    }


    void addToIncomingMessageQueue()
    {
        if (Connection::Owner::SERVER == m_owner)
        {
            m_messagesIn.push_back({this->shared_from_this(), m_tmpMsgIn});
        }
        else
        {
            m_messagesIn.push_back({nullptr, m_tmpMsgIn});
        }

        readHeader(); // continue rading
    }

    void writeHeader()
    {
        boost::asio::async_write(m_socket, boost::asio::buffer(&m_messagesOut.front().m_header, sizeof(m_messagesOut.front().m_header)),
            [this](std::error_code errCode, std::size_t length)
            {
                if (!errCode)
                {
                    if (m_messagesOut.front().size())
                    {
                        writeBody();
                    }
                    else
                    {
                        m_messagesOut.pop_front();

                        if (!m_messagesOut.empty())
                        {
                            writeHeader();
                        }   
                    }
                }
                else
                {
                    std::cout << "[" << m_id << "] Write Header Fail!\n";
                    m_socket.close();
                }

            });
    }

    void writeBody()
    {
        boost::asio::async_write(m_socket, boost::asio::buffer(&m_messagesOut.front().body.data, sizeof(m_messagesOut.front().body)),
            [this](std::error_code errCode, std::size_t length)
            {
                if (!errCode)
                {
                    m_messagesOut.pop_front();

                    if (!m_messagesOut.empty())
                    {
                        writeHeader();
                    }   
                }
                else
                {
                    std::cout << "[" << m_id << "] Write Body Fail!\n";
                    m_socket.close();
                }

            });
    }


protected:
        boost::asio::ip::tcp::socket m_socket;

        boost::asio::io_context &m_asioContext;

        ThreadSafeQueue<Message<T>> m_messagesOut;
        ThreadSafeQueue<OwnedMessage<T>> &m_messagesIn; // provided from outside

        Message<T> m_tmpMsgIn;

        Owner m_owner = Owner::SERVER;
        uint32_t m_id = 0;
    };

} // tcp_communication
