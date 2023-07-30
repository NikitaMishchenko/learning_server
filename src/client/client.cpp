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

int main()
{
    CustomClient c;
    c.connect("127.0.0.1", 8080);

    // telnet localhost 8080
    bool isFinished = false;
    
    //while (!isFinished)
    {

        // get input data
        c.pingServer();

        if (c.isConnected())
		{
			if (!c.incomingMsgs().empty())
			{

				auto msg = c.incomingMsgs().pop_front().msg;

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
                        isFinished = true;
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
		}
		else
		{
			std::cout << "Server Down\n";
			isFinished = true;
		}
    }

    return 0;
}