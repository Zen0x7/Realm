#pragma once

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>

class message {
public:
    static constexpr std::size_t attribute_size_length_ = 2;
    static constexpr std::size_t attribute_identifier_length_ = 16;
    static constexpr std::size_t attribute_checksum_length_ = 4;
    static constexpr std::size_t header_length_ = attribute_size_length_ + (attribute_identifier_length_ * 2);
    static constexpr std::size_t max_body_length = 1024 - header_length_;
    boost::uuids::uuid id_;

    message();

    [[nodiscard]] const char *data() const;
    char *data();

    static message from_string(const std::string &data);

    [[nodiscard]] std::string get_serialized_id() const;

    static boost::uuids::uuid parse_serialized_id(const std::string &id);

    [[nodiscard]] std::size_t length() const;

    [[nodiscard]] const char *body() const;

    char *body();

    [[nodiscard]] std::size_t body_length() const;

    void body_length(std::size_t new_length);

    bool decode();

    void encode(const std::string & data);

private:
    char data_[header_length_ + max_body_length] = "";
    std::size_t body_length_;
};
