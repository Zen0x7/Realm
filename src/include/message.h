#pragma once

#include <foreach.h>

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/crc.hpp>
#include <boost/algorithm/hex.hpp>

#include <bitset>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <iterator>
#include <iostream>

class message {
public:
    static constexpr std::size_t attribute_size_length_ = 2;
    static constexpr std::size_t header_length_ = 2 + 16 + 16;
    static constexpr std::size_t max_body_length = 1024;
    boost::uuids::uuid id_;

    message() : id_(boost::uuids::random_generator()()), body_length_(0) {
    }

    const char *data() const {
        return data_;
    }

    static message from_string(const std::string &data) {
        message draft;
        draft.body_length(data.size());
        draft.encode(data);
        return draft;
    }

    [[nodiscard]] std::string get_serialized_id() const {
        std::ostringstream stream;
        foreach (const uint8_t &item, id_) {
            stream << item;
        }
        return stream.str();
    }

    static boost::uuids::uuid parse_serialized_id(const std::string &id) {
        boost::uuids::uuid output = {};
        std::memcpy(output.data, id.data(), 16);
        return output;
    }

    char *data() {
        return data_;
    }

    std::size_t length() const {
        return header_length_ + body_length_;
    }

    const char *body() const {
        return data_ + header_length_;
    }

    char *body() {
        return data_ + header_length_;
    }

    std::size_t body_length() const {
        return body_length_;
    }

    void body_length(std::size_t new_length) {
        body_length_ = new_length;
        if (body_length_ > max_body_length)
            body_length_ = max_body_length;
    }

    bool decode() {
        std::memcpy(&body_length_, data_ + 0, 2);

        char sender_id[17] = "";
        std::memcpy(sender_id, data_ + 2, 16);

        char receiver_id[17] = "";
        std::memcpy(receiver_id, data_ + 18, 16);

        if (body_length_ > max_body_length) {
            body_length_ = 0;
            return false;
        }
        return true;
    }

    void encode(const std::string & data) {
        auto * body_size = (char *)(&body_length_);
        std::stringstream ss;

        ss.write(body_size, 2);
        ss.write(get_serialized_id().data(), 16);
        ss.write(get_serialized_id().data(), 16);
        ss.write(data.data(), data.size());

        boost::crc_32_type crc;
        crc.process_bytes(ss.str().data(), ss.str().size());
        char checksum[4] = "";
        const unsigned long checksum_value = crc.checksum();
        std::memcpy(checksum, &checksum_value, 4);

        ss.write(checksum, 4);

        std::memcpy(data_, ss.str().data(), ss.str().size());
    }

private:
    char data_[header_length_ + max_body_length] = "";
    std::size_t body_length_;
};
