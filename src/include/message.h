#pragma once

#include <bitset>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/serialization/binary_object.hpp>
#include <sstream>

#include <iostream>

struct message_identifier {
    char bytes[16];
};

class message {
public:
    static constexpr std::size_t attribute_size_length_ = 4;
    static constexpr std::size_t header_length_ = 4 + 16 + 16;
    static constexpr std::size_t max_body_length = 512;
    boost::uuids::uuid id_;

    message() : id_(boost::uuids::random_generator()()), body_length_(0) {
    }

    const char *data() const {
        return data_;
    }

    static message from_string(const std::string &data) {
        message msg;
        msg.body_length(data.size());
        msg.encode(data);
        return msg;
    }

    [[nodiscard]] std::string get_serialized_id() const {
        std::ostringstream convert;
        for (const unsigned char a : id_.data) {
            convert << a;
        }
        return convert.str();
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
        return data_ + 36;
    }

    char *body() {
        return data_ + 36;
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
        char size[5] = "";
        std::memcpy(size, data_ + 0, 4);
        body_length_ = std::atoi(size);

        char sender_id[17] = "";
        std::memcpy(sender_id, data_ + 4, 16);

        char receiver_id[17] = "";
        std::memcpy(receiver_id, data_ + 20, 16);

        if (body_length_ > max_body_length) {
            body_length_ = 0;
            return false;
        }
        return true;
    }

    void encode(const std::string & data) {
        char header[attribute_size_length_] = "";
        std::sprintf(header, "%4d", static_cast<int>(body_length_));
        std::stringstream ss;
        ss.write(header, 4);
        ss.write(get_serialized_id().data(), 16);
        ss.write(get_serialized_id().data(), 16);
        ss.write(data.data(), data.size());

        std::memcpy(data_, ss.str().data(), ss.str().size());
    }

private:
    char data_[header_length_ + max_body_length] = "";
    std::size_t body_length_;
};
