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
            return m_body.size(); // sizeof(m_header) + 
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

            const size_t initialBodySize = msg.m_body.size();
            const size_t newBodySize = initialBodySize + sizeof(DataType);

            std::cout << "msg.m_body.size() = " << initialBodySize << "\n";
            msg.m_body.resize(newBodySize);
            std::cout << "new msg.m_body.size() = " << newBodySize << "\n";

            std::memcpy(msg.m_body.data() + (newBodySize - initialBodySize), &data, (newBodySize - initialBodySize));

            msg.m_header.size = newBodySize;

            std::cout << "header stored size = " << msg.m_header.size << "\n";

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
