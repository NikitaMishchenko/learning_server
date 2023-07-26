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

enum class CustomMessage : uint32_t
{
    TYPE_1,
    TYPE_2
};

int main()
{
    tcp_communication::Message<CustomMessage> msg;
    msg.m_header.id = CustomMessage::TYPE_1;

    int a = 1;
    bool b = true;
    float c = 3.14159f;

    struct 
    {
        float x;
        float y;
    } d[5];

    msg << a << b << c << d;
    
    a = 2;
    b = false;
    c = 4.14159f;

    msg >> d >> c >> b >> a;    

    return 0;
}