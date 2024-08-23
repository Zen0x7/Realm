#include <protocol.h>
#include <message.h>
#include <iostream>

void protocol::from_worker(const message & message)  {
    std::cout.write(message.body(), message.body_length());
    std::cout << "\n";
}

void protocol::from_server(const message & message)  {
    std::cout.write(message.body(), message.body_length());
    std::cout << "\n";
}