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
            CLIENT,
            SERVER,
            UNKNOWN
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
            std::cout << "Connection: connectToClient() uid = " << uid << "\n";

            if (Owner::SERVER == m_owner)
            {
                if (m_socket.is_open())
                {
                    m_id = uid;

                    readHeader();
                }
                else
                {
                    std::cout << "connectToClient: socket not opened!\n";
                }
            }
        }

        void connectToServer(const boost::asio::ip::tcp::resolver::results_type &endpoints)
        {
            std::cout << "Connection: connectToServer():\n";
            if (Owner::CLIENT == m_owner)
            {
                boost::asio::async_connect(m_socket, endpoints,
                                           [this](boost::system::error_code errCode, boost::asio::ip::tcp::endpoint endpoint)
                                           {
                                               if (!errCode)
                                               {
                                                   readHeader();
                                               }
                                           });
            }
        }

        void disconnect()
        {
            if (isConnected())
                boost::asio::post(m_asioContext, [this]()
                                  { m_socket.close(); });
        }

        bool isConnected() const
        {
            return m_socket.is_open();
        }

        uint32_t getId() const
        {
            return m_id;
        }

        void send(const Message<T> &msg)
        {
            std::cout << "Connection: send() " << toString(msg.m_header.id) << " size: " << msg.m_header.size << "\n";

            boost::asio::post(m_asioContext,
                              [this, msg]()
                              {
                                  bool isWritingMessageNow = m_messagesOut.size();
                                  m_messagesOut.push_back(msg);

                                  if (!isWritingMessageNow)
                                      writeHeader();
                              });
        }

    private:
        void readHeader()
        {
            std::cout << "Connection: readHeader()\n";

            boost::asio::async_read(m_socket, boost::asio::buffer(&m_tmpMsgIn.m_header, sizeof(Header<T>)),
                                    [this](std::error_code errCode, std::size_t length)
                                    {
                                        if (!errCode)
                                        {
                                            std::cout << "[" << m_id << "] readHeader() "
                                                      << "size: " << m_tmpMsgIn.m_header.size << ", type: " << toString(m_tmpMsgIn.m_header.id) << "\n";
                                            if (m_tmpMsgIn.m_header.size)
                                            {
                                                std::cout << "We have a body to read! heder assured size: " << m_tmpMsgIn.m_header.size << "\n";

                                                m_tmpMsgIn.m_body.resize(m_tmpMsgIn.m_header.size);
                                                readBody();
                                            }
                                            else
                                            {
                                                std::cout << "bodyless msg " << toString(m_tmpMsgIn.m_header.id);
                                                addToIncomingMessageQueue();
                                            }
                                        }
                                        else
                                        {
                                            std::cout << "[" << m_id << "] Read Header Fail! errCode: " << errCode << " Closing socket...\n";
                                            m_socket.close();
                                        }
                                    });
        }

        void readBody()
        {
            std::cout << "Connection: readBody()\n";

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
            std::cout << "Connection: addToIncomingMessageQueue()\n";

            if (Connection::Owner::SERVER == m_owner)
            {
                std::cout << "appending msg to msgIn SERVER\n";

                m_messagesIn.push_back({this->shared_from_this(), m_tmpMsgIn});

                std::cout << "now " << m_messagesIn.size() << " msgs in queue\n";
                // std::cout << "msgIn:" << m_tmpMsgIn << "\n";
            }
            else
            {
                std::cout << "appending msg to msgIn CLIENT\n";
                m_messagesIn.push_back({nullptr, m_tmpMsgIn});
                std::cout << "now " << m_messagesIn.size() << "msgs in queue\n";
            }

            readHeader(); // continue reading
        }

        void writeHeader()
        {
            std::cout << "Connection: writeHeader() \n";
            boost::asio::async_write(m_socket, boost::asio::buffer(&m_messagesOut.front().m_header, sizeof(m_messagesOut.front().m_header)),
                                     [this](std::error_code errCode, std::size_t length)
                                     {
                                         if (!errCode)
                                         {
                                             if (m_messagesOut.front().m_body.size())
                                             {
                                                 std::cout << "m_messagesOut.m_body.size(): " << m_messagesOut.front().m_body.size() << "\n";
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
            std::cout << "Connection: writeBody()\n";

            boost::asio::async_write(m_socket, boost::asio::buffer(m_messagesOut.front().m_body.data(), sizeof(m_messagesOut.front().m_body)),
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
                                             std::cout << "[" << m_id << "] Write Body Fail! errCode: " << errCode << "\n";
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
