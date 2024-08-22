#pragma once

#include <string_view>

class protocol {
public:
    void static from_worker(const std::string & message, std::size_t bytes);
    void static from_server(const std::string & message, std::size_t bytes);
};
