#pragma once

#include <string_view>

class protocol {
public:
    void static from_worker(const std::string_view &message);
};
