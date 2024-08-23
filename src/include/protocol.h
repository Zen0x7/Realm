#pragma once

#include <message.h>

struct response {
    enum STATUS_CODE {
        CRC_ERROR,
        DRAFT,
        OK,
    } status_code_ = DRAFT;
    bool closes = false;
};

class protocol {
public:
    response static from_worker(const message & message);
    response static from_server(const message & message);

    static bool has_integrity(const message & message);
};
