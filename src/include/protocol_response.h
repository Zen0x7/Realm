#pragma once

struct protocol_response {
    enum STATUS_CODE {
        CRC_ERROR,
        DRAFT,
        OK,
    } status_code_ = DRAFT;
    bool closes = false;
};
