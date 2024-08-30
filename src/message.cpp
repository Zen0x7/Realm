#include <message.h>

#include <foreach.h>

#include <boost/crc.hpp>
#include <boost/uuid/random_generator.hpp>

#include <cstring>
#include <sstream>

message::message(const boost::uuids::uuid &sender_id) : id_(boost::uuids::random_generator()()), sender_id_(sender_id),
                                                        body_length_(0) {
}

message::message(): id_(boost::uuids::random_generator()()), sender_id_(boost::uuids::random_generator()()),
                    body_length_(0) {
}

const char *message::data() const {
    return data_;
}

message message::from_string(const std::string &data, const boost::uuids::uuid &sender_id) {
    message draft(sender_id);
    draft.body_length(data.size());
    draft.encode(data);
    return draft;
}

std::string message::serialize(const boost::uuids::uuid &id) {
    std::ostringstream stream;
    foreach(const uint8_t &item, id) {
        stream << item;
    }
    return stream.str();
}

boost::uuids::uuid message::parse_serialized_id(const std::string &id) {
    boost::uuids::uuid output = {};
    std::memcpy(output.data, id.data(), attribute_identifier_length_);
    return output;
}

char *message::data() {
    return data_;
}

std::string message::get_sender() {
    return {data_ + attribute_size_length_, attribute_identifier_length_};
}

std::string message::get_identifier() {
    return {data_ + attribute_size_length_ + attribute_identifier_length_, attribute_identifier_length_};
}

std::size_t message::length() const {
    return header_length_ + body_length_;
}

const char *message::body() const {
    return data_ + header_length_;
}

char *message::body() {
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
    std::memcpy(sender_id_.data, data_ + attribute_size_length_, attribute_identifier_length_);
    std::memcpy(id_.data, data_ + (attribute_size_length_ + attribute_identifier_length_),
                attribute_identifier_length_);

    if (body_length_ > max_body_length) {
        body_length_ = 0;
        return false;
    }
    return true;
}

void message::encode(const std::string &data) {
    auto *body_size = (char *) &body_length_;
    std::stringstream stream;
    // Add size
    stream.write(body_size, attribute_size_length_);

    // Adds identifiers
    foreach(const uint8_t &byte, sender_id_) { stream << byte; }
    foreach(const uint8_t &byte, id_) { stream << byte; }

    // Add data
    stream << data;

    // Adds a CRC
    boost::crc_32_type crc;
    crc.process_bytes(stream.str().data(), stream.str().size());
    char checksum[attribute_checksum_length_] = "";
    const unsigned long checksum_value = crc.checksum();
    std::memcpy(checksum, &checksum_value, attribute_checksum_length_);
    stream << checksum << std::endl;

    // Merge and push
    std::memcpy(data_, stream.str().data(), stream.str().size());
}
