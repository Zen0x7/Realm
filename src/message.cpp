#include <message.h>

#include <foreach.h>

#include <boost/crc.hpp>
#include <boost/uuid/random_generator.hpp>

#include <cstring>
#include <sstream>

message::message() : id_(boost::uuids::random_generator()()), body_length_(0) {
}

const char * message::data() const {
    return data_;
}

message message::from_string(const std::string &data) {
    message draft;
    draft.body_length(data.size());
    draft.encode(data);
    return draft;
}

std::string message::get_serialized_id() const {
    std::ostringstream stream;
    foreach (const uint8_t &item, id_) {
        stream << item;
    }
    return stream.str();
}

boost::uuids::uuid message::parse_serialized_id(const std::string &id) {
    boost::uuids::uuid output = {};
    std::memcpy(output.data, id.data(), attribute_identifier_length_);
    return output;
}

char * message::data() {
    return data_;
}

std::size_t message::length() const {
    return header_length_ + body_length_;
}

const char * message::body() const {
    return data_ + header_length_;
}

char * message::body() {
    return data_ + header_length_;
}

std::size_t message::body_length() const {
    return body_length_;
}

void message::body_length(std::size_t new_length) {
    body_length_ = new_length;
    if (body_length_ > max_body_length) body_length_ = max_body_length;
}

bool message::decode() {
    std::memcpy(&body_length_, data_ + 0, attribute_size_length_);

    char sender_id[attribute_identifier_length_] = "";
    std::memcpy(sender_id, data_ + attribute_size_length_, attribute_identifier_length_);

    char receiver_id[attribute_identifier_length_] = "";
    std::memcpy(receiver_id, data_ + (attribute_size_length_ + attribute_identifier_length_), attribute_identifier_length_);

    if (body_length_ > max_body_length) {
        body_length_ = 0;
        return false;
    }
    return true;
}

void message::encode(const std::string &data) {
    auto * body_size = (char *)(&body_length_);
    std::stringstream ss;

    ss.write(body_size, attribute_size_length_);
    ss.write(get_serialized_id().data(), attribute_identifier_length_);
    ss.write(get_serialized_id().data(), attribute_identifier_length_);
    ss.write(data.data(), data.size());

    boost::crc_32_type crc;
    crc.process_bytes(ss.str().data(), ss.str().size());
    char checksum[attribute_checksum_length_] = "";
    const unsigned long checksum_value = crc.checksum();
    std::memcpy(checksum, &checksum_value, attribute_checksum_length_);

    ss.write(checksum, attribute_checksum_length_);

    std::memcpy(data_, ss.str().data(), ss.str().size());
}

