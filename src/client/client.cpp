#include <iostream>

#include <string>
#include <thread>
#include <chrono>

#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include <boost/asio/error.hpp>

#include "tcp_message.h"
#include "client_interface.h"

#include "custom_client.h"

bool doWork(CustomClient &client, char key)
{
    bool workIsFinished = false;

    // get input data

    if ('1' == key)
    {
        client.pingServer();
        key = '0';
    }

    if (client.isConnected())
    {
        if (!client.incomingMsgs().empty())
        {
            std::cout << "incoming message: " 
                      << "header id: " <<  int(client.incomingMsgs().back().m_header.id)
                      << "header size: " << client.incomingMsgs().back().m_header.size
                      << "body: ";
            for (auto& k : client.incomingMsgs().back().m_body) 
                std::cout << k;
            std::cout << "\n";

            auto msg = client.incomingMsgs().pop_front().msg;

            switch (msg.m_header.id)
            {
            case CustomMessageTypes::SERVER_ACCEPT:
            {
                // Server has responded to a ping request
                std::cout << "Server Accepted Connection\n";
            }
            break;

            case CustomMessageTypes::SERVER_PING:
            {
                // Server has responded to a ping request
                std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
                std::chrono::system_clock::time_point timeThen;
                msg >> timeThen;
                std::cout << "Ping: " << std::chrono::duration<double>(timeNow - timeThen).count() << "\n";
                workIsFinished = true;
            }
            break;

            case CustomMessageTypes::SERVER_MESSAGE:
            {
                // Server has responded to a ping request
                uint32_t clientID;
                msg >> clientID;
                std::cout << "Hello from [" << clientID << "]\n";
            }
            break;
            }
        }
        else
        {
            std::cout << "no incoming messages\n";
        }
    }
    else
    {
        std::cout << "Server Down\n";
        workIsFinished = true;
    }

    return workIsFinished;
}

int main()
{
    boost::asio::io_service ioservice;
    boost::asio::posix::stream_descriptor stream(ioservice, STDIN_FILENO);

    char buffButton[1] = {};
    char key;

    std::function<void(boost::system::error_code, size_t)> readHandler; // todo make all handlers this way

    readHandler = [&](boost::system::error_code errCode, size_t len)
    {
        if (errCode)
        {
            std::cerr << "readHanfler failed, errCode: " << errCode.message() << "\n";
        }
        else
        {
            if (1 == len)
            {
                std::cout << "input key is: " << buffButton[0] << "\n";
                key = buffButton[0];
            }
            boost::asio::async_read(stream, boost::asio::buffer(buffButton), readHandler);
        }
    };

    boost::asio::async_read(stream, boost::asio::buffer(buffButton), readHandler);

    // ioservice.run();
    std::thread buttonsThread([&ioservice]()
                            {
                                //ioservice.run();
                            });

    CustomClient c;
    c.connect("127.0.0.1", 8080);

    // telnet localhost 8080
    bool isFinished = false;

    key = '1';

    while (!isFinished)
    {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1000ms);
        isFinished = doWork(c, key);        
    }

    if (buttonsThread.joinable())
        buttonsThread.join();

    return 0;
}