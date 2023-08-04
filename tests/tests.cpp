#include <gtest/gtest.h>

#include <owned_message.h>
#include "../custom_message_types.h"

TEST(basic, test1)
{
    // tcp_communication::OwnedMessage<CustomMessageTypes> msg;

    tcp_communication::Header<CustomMessageTypes> header;

    header.id = CustomMessageTypes::SERVER_PING;
    // msg.m_header = header;

    
}