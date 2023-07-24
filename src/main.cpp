#include <iostream>

#include <string>
#include <boost/system/error_code.hpp>

#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include <boost/asio/error.hpp>

std::string httpGetIndex()
{
    return "GET /index.html HTTP/1.1\r\n"
           "Host: example.com\r\n"
           "Connection: close\r\n\r\n";
}

int main()
{

    boost::asio::io_context context;


    std::string address = "51.38.81.49"; // example.com
    int32_t port  = 80;
    boost::system::error_code errorCode;
     
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address(address, errorCode), port);

    boost::asio::ip::tcp::socket socket(context);

    if (!errorCode)
    {
        std::cout << "Connected to \"" << address << ":" << port << "\"!" << std::endl;
    }
    else
    {
        std::cout << "Filed to connect to address: " << errorCode.message() << std::endl;
    }


    if (socket.is_open())
    {
        std::cout << "socket is open!\n";

        std::string sRequest = httpGetIndex();

        socket.write_some(boost::asio::buffer(sRequest.data(), sRequest.size()), errorCode);

        // loading data from socket

        std::cout << "write_some errCode: " << errorCode.message() << "\n";

        size_t bytesAvalible = socket.available();
        std::cout << "Bytes avalible on socket: " << bytesAvalible << "\n";

        if (bytesAvalible)
        {
            std::vector<uint8_t> vBuffer(bytesAvalible);

            socket.read_some(boost::asio::buffer(vBuffer.data(), vBuffer.size()), errorCode);

            for (std::vector<uint8_t>::const_iterator it; it != vBuffer.end(); it++)
            {
                std::cout << *it << "\n";
            }
        }
    }
    else
    {
        std::cout << "socket is not open!\n";
    }


    return 0;
}