#pragma once

#include <iostream>

#include <boost/asio.hpp>

#include <client_interface.h>

#include "../custom_message_types.h"


class CustomServer : public tcp_communication::IServer<CustomMessageTypes>
{
public:
    CustomServer(uint16_t port) :  tcp_communication::IServer<CustomMessageTypes>(port)
    {}

protected:
    virtual bool onClientConnect(std::shared_ptr< tcp_communication::Connection<CustomMessageTypes>> client) override
    {
        return true;
    }

    virtual void onClientDisconnect(std::shared_ptr<tcp_communication::Connection<CustomMessageTypes>> client) override
    {
    }

    virtual bool onMessage(std::shared_ptr<tcp_communication::Connection<CustomMessageTypes> > client,
                           const tcp_communication::Message<CustomMessageTypes> &msg) override
    {
        return true;
    }
};
