#include <iostream>

#include <string>
#include <thread>
#include <chrono>

#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include <boost/asio/error.hpp>

std::string httpGetIndexRequest()
{
    return "GET /index.html HTTP/1.1\r\n"
           "Host: example.com\r\n"
           "Connection: close\r\n\r\n";
}

std::vector<uint8_t> vBuffer(1024);

void grabSomeData(boost::asio::ip::tcp::socket& socket)
{
    std::cout << "grabSomeData entry\n";

    socket.async_read_some(boost::asio::buffer(vBuffer.data(), vBuffer.size()),
        [&](boost::system::error_code errCode, std::size_t length)
        {
            if (!errCode)
            {
                std::cout << "\n\nRead" << length << " bytes\n\n";
                
                for(int i = 0; i < length; i++)
                {
                    std::cout << vBuffer[i];
                }

                grabSomeData(socket);
            }
            else
            {
                std::cout << "grabSomeData errCode: " << errCode << "\n";
            }
        }
    );    
}


int main()
{

    boost::asio::io_context context;

    // start context (context need tasks to work on)
    std::thread threadContext = std::thread([&]() { context.run(); });

    std::string address = "93.184.216.34"; // example.com
    int32_t port  = 80;
    boost::system::error_code errorCode;
     
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address(address, errorCode), port);

    boost::asio::ip::tcp::socket socket(context);

    socket.connect(endpoint, errorCode);

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

        // prepare task before request for data
        grabSomeData(socket); // prime context with work to do (it's own thread)

        std::string sRequest = httpGetIndexRequest();

        socket.write_some(boost::asio::buffer(sRequest.data(), sRequest.size()), errorCode);

        // loading data from socket

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(200ms);

        std::cout << "write_some errCode: " << errorCode.message() << "\n";

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(2000ms);

        context.stop();
        
        if(threadContext.joinable()) 
            threadContext.join();
    }
    else
    {
        std::cout << "socket is not open!\n";
    }


    return 0;
}