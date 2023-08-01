#pragma once

#include <string>

enum class CustomMessageTypes : uint32_t
{
    SERVER_ACCEPT,
    SERVER_DENY,
    SERVER_PING,
    MESSAGE_ALL,
    SERVER_MESSAGE
};

inline std::string toString(CustomMessageTypes headerType)
{
    switch (headerType)
    {
        case CustomMessageTypes::SERVER_ACCEPT:
            return "SERVER_ACCEPT";
        case CustomMessageTypes::SERVER_DENY:
            return "SERVER_DENY";
        case CustomMessageTypes::SERVER_PING:
            return "SERVER_PING";
        case CustomMessageTypes::MESSAGE_ALL:
            return "MESSAGE_ALL";
        case CustomMessageTypes::SERVER_MESSAGE:
        return "SERVER_MESSAGE";
    }

    return "unknown message type";
}