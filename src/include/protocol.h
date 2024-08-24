#pragma once

#include <message.h>
#include <protocol_response.h>

class protocol {
public:
    protocol_response static from_worker(const message & message);
    protocol_response static from_server(const message & message);

    static bool has_integrity(const message & message);
};
