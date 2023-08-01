#pragma once

#include <iostream>

#include <boost/asio.hpp>

#include <server.h>
#include "../custom_message_types.h"

class CustomServer : public tcp_communication::IServer<CustomMessageTypes>
{
public:
    CustomServer(uint16_t port) : tcp_communication::IServer<CustomMessageTypes>(port)
    {
    }

protected:
    virtual bool onClientConnect(std::shared_ptr<tcp_communication::Connection<CustomMessageTypes>> client) override
    {
        std::cout << "CustomServer onClientConnect() id: " << client->getId() << "\n";
        tcp_communication::Message<CustomMessageTypes> msg;
		
        msg.m_header.id = CustomMessageTypes::SERVER_ACCEPT;
		client->send(msg);
        
        return true;

    }

    virtual void onClientDisconnect(std::shared_ptr<tcp_communication::Connection<CustomMessageTypes>> client) override
    {
        std::cout << "Removing client [" << client->getId() << "]\n";
    }

    virtual void onMessage(std::shared_ptr<tcp_communication::Connection<CustomMessageTypes>> client,
                           const tcp_communication::Message<CustomMessageTypes> &msg) override
    {
        std::cout << "CustomServer onMessage()\n";

        switch (msg.m_header.id)
        {
        case CustomMessageTypes::SERVER_PING:
        {
            std::cout << "[" << client->getId() << "]: SERVER_PING\n";

            client->send(msg); // msg contains time

            break;
        }
        default:
            std::cout << "[" << client->getId() << "]: unhandled message!\n";
            break;
        }
    }
};
