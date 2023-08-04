#pragma once

#include <iostream>
#include <chrono>

#include <boost/asio.hpp>

#include <client_interface.h>

#include "../custom_message_types.h"


class CustomClient : public tcp_communication::IClient<CustomMessageTypes>
{ 
public:    
    virtual bool onClientConnect(std::shared_ptr<tcp_communication::Connection<CustomMessageTypes>> client)
    {
        tcp_communication::Message<CustomMessageTypes> msg;
		msg.m_header.id = CustomMessageTypes::SERVER_ACCEPT;
		client->send(msg);

        return true;
    }

    void pingServer()
    {
        std::cout << "CustomClient pingServer()\n";

        tcp_communication::Message<CustomMessageTypes> msg;
        msg.m_header.id = CustomMessageTypes::SERVER_PING;

        std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();

        std::cout << "sizeOf body = " << sizeof(timeNow) << "\n";

        msg << timeNow;

        std::cout << "msg Header assures size: " << msg.m_header.size << "\n";

        this->send(msg);

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1000ms);
    }

private:
};
