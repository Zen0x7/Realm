#pragma once

#include <message.h>

class protocol {
public:
    void static from_worker(const message & message);
    void static from_server(const message & message);
};
