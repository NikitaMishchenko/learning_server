#pragma once

#include <memory>
#include <iostream>

#include "tcp_message.h"

namespace tcp_communication
{

    template<typename T>
    class Connection;

    template<typename T>
    struct OwnedMessage : public Message<T>
    {
        OwnedMessage(std::shared_ptr<Connection<T>> connection, Message<T> msg_)
            : remote(connection), msg(msg_)
        {}

        friend std::ostream& operator<< (std::ostream& os, const OwnedMessage<T>& msg)
        {
            os << msg.msg;
            return os;
        }        

        std::shared_ptr<Connection<T>> remote;
        Message<T> msg;
    };

} // tcp_communication
