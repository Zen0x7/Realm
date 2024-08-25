#include <protocol.h>

#include <message.h>

#include <boost/crc.hpp>

#include <iostream>

protocol_response protocol::from_worker(const message & message) {
    if (has_integrity(message)) {
        // Works
        std::string data = { message.body(), message.body_length() };

        std::cout << data << std::endl;
        return {
            .status_code_ = protocol_response::OK,
            .closes = false,
        };
    }

    return {
        .status_code_ = protocol_response::CRC_ERROR,
        .closes = true,
    };
}

protocol_response protocol::from_server(const message & message)  {
    if (has_integrity(message)) {
        // Works
        const std::string data = { message.body(), message.body_length() };


        std::cout << data << std::endl;
        return {
            .status_code_ = protocol_response::OK,
            .closes = false,
        };
    }

    return {
        .status_code_ = protocol_response::CRC_ERROR,
        .closes = true,
    };
}

bool protocol::has_integrity(const message &message) {
    const std::string checksum = { message.data() + message::header_length_ + message.body_length(), message::attribute_checksum_length_};
    const std::string body = { message.data(), message::header_length_ + message.body_length() };

    boost::crc_32_type crc;
    crc.process_bytes(body.data(), body.size());

    unsigned int checksum_value;
    std::memcpy(&checksum_value, checksum.data(), sizeof(checksum_value));
    return checksum_value == crc.checksum();
}
