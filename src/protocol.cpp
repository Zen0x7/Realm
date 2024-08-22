#include <protocol.h>
#include <iostream>

void protocol::from_worker(const std::string & message, std::size_t bytes)  {
    std::cout.write(message.data(), bytes);
    std::cout << "\n";
}

void protocol::from_server(const std::string & message, std::size_t bytes)  {
    std::cout.write(message.data(), bytes);
    std::cout << "\n";
}