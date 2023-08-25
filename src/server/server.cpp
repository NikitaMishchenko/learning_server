#include <iostream>

#include <string>
#include <thread>
#include <chrono>

#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include <boost/asio/error.hpp>

#include <tcp_message.h>

#include "client_interface.h"
#include "server.h"


#include "custom_server.h"

int main()
{
    CustomServer server(8080);
    server.start();

    try
    {
        while(1)
            server.update();
    }
    catch(...)
    {
        std::cerr << "excetion thrown in main SERVER\n";
    }
    // telnet localhost 8080

    return 0;
}