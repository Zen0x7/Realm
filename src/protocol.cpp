#include <protocol.h>
#include <message.h>
#include <iostream>

response protocol::from_worker(const message & message) {
    if (has_integrity(message)) {
        // Works
        std::string data = { message.body(), message.body_length() };

        std::cout << data << std::endl;
        return {
            .status_code_ = response::OK,
            .closes = false,
        };
    }

    return {
        .status_code_ = response::CRC_ERROR,
        .closes = true,
    };
}

response protocol::from_server(const message & message)  {
    if (has_integrity(message)) {
        // Works
        std::string data = { message.body(), message.body_length() };

        std::cout << data << std::endl;
        return {
            .status_code_ = response::OK,
            .closes = false,
        };
    }

    return {
        .status_code_ = response::CRC_ERROR,
        .closes = true,
    };
}

bool protocol::has_integrity(const message &message) {
    const std::string checksum = { message.data() + 36 + message.body_length(), 4};
    const std::string body = { message.data(), 36 + message.body_length() };

    boost::crc_32_type crc;
    crc.process_bytes(body.data(), body.size());

    unsigned int checksum_value;
    std::memcpy(&checksum_value, checksum.data(), sizeof(checksum_value));
    return checksum_value == crc.checksum();
}
