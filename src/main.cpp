#include <iostream>

#include <string>
#include <thread>
#include <chrono>

#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include <boost/asio/error.hpp>

#include "basic/message.h"
#include "basic/client_interface.h"
#include "basic/server.h"

enum class CustomMessageTypes : uint32_t
{
    SERVER_ACCEPT,
    SERVER_DENY,
    SERVER_PING,
    MESSAGE_ALL,
    SERVER_MESSAGE
};

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
    }
};

int main()
{
    CustomServer server(8080);
    server.start();

    while(1)
        server.update();

    // telnet localhost 8080

    return 0;
}