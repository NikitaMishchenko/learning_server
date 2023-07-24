#include <iostream>

#include <string>

#include <boost/asio.hpp>

int main()
{
    boost::asio::io_service io_service;
    // socket creation
    boost::asio::ip::tcp::socket socket(io_service);

    return 0;
}