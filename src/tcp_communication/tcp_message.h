#pragma once

#include <vector>
#include <algorithm>
#include <iostream>

#include <boost/asio.hpp>

namespace tcp_communication
{
    template<typename T>
    struct Header
    {
        T id;
        uint32_t size = 0; ///< total size of corresponding message (header + body)
    };

    template<typename T>
    struct Message
    {
        size_t size() const
        {
            return sizeof(m_header) + m_body.size();
        }

        void reserveBody(size_t i)
        {
            m_body.reserve(i);
        }

        friend std::ostream& operator << (std::ostream& os, const Message<T>& msg)
        {
            os << "ID:" << int(msg.m_header.id) << " Size:" << msg.m_header.size;
            return os;
        }
        
        template<typename DataType>
        friend Message<T>& operator << (Message<T>& msg, const DataType& data)
        {
            static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to ");

            size_t msgSize = msg.m_body.size();

            msg.m_body.resize(msg.m_body.size() + sizeof(DataType));

            std::memcpy(msg.m_body.data() + msgSize, &data, sizeof(DataType));

            msg.m_header.size = msg.size();

            return msg;
        }

        template<typename DataType>
        friend Message<T>& operator >> (Message<T>& msg, DataType& data)
        {
            static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to ");

            size_t newSize = msg.m_body.size() - sizeof(DataType);
            
            std::memcpy(&data, msg.m_body.data() + newSize, sizeof(DataType));

            msg.m_body.resize(newSize);
            
            msg.m_header.size = msg.size();

            return msg;
        }

        std::string toString()
        {
            std::stringstream ss;

            return ss.str();
        }

        Header<T> m_header;
        std::vector<char> m_body;
    };

} // tcp_communication
