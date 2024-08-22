#include <protocol.h>
#include <iostream>

void protocol::from_worker(const std::string_view & message)  {
    std::cout << message << std::endl;
}