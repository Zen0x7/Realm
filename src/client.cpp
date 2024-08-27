#include <client.h>

#include <protocol.h>

#include <iostream>
#include <boost/beast/core/bind_handler.hpp>

client::client(boost::asio::io_context &io_context) : resolver_(make_strand(io_context)), stream_(make_strand(io_context)) {
}

void client::do_resolve() {
    std::cout << "[INFO] Resolving ..." << std::endl;
    resolver_.async_resolve("0.0.0.0", "8000",
                                boost::beast::bind_front_handler(&client::on_resolve, shared_from_this()));
}

void client::run() {
    do_resolve();
}

void client::on_resolve(const boost::system::error_code &error_code,
                        boost::asio::ip::tcp::resolver::results_type endpoints) {
    std::cout << "[INFO] Resolved ..." << std::endl;
    if (!error_code) {
        std::cout << "[INFO] Connecting ..." << std::endl;
        stream_.async_connect(endpoints,
            boost::beast::bind_front_handler(&client::on_connect, shared_from_this()));
    } else {
        throw std::invalid_argument("State can't be resolved");
    }
}

void client::on_connect(const boost::system::error_code &error_code, boost::asio::ip::tcp::resolver::results_type::endpoint_type) {
    if (!error_code) {
        std::cout << "[INFO] Connected" << std::endl;
        do_read_header();
    }
}

void client::do_read_header() {
    stream_.async_read_some(boost::asio::buffer(message_.data(), message::header_length_),
               [this](const boost::system::error_code &error_code, std::size_t length) {
                   if (!error_code && message_.decode()) {
                       do_read_body();
                   } else {
                           stream_.close();
                   }
               });
}

void client::do_read_body() {
    stream_.async_read_some(boost::asio::buffer(message_.body(), message_.body_length() + message::attribute_checksum_length_),
               [this](const boost::system::error_code &error_code, std::size_t length) {
                   if (!error_code) {
                       const auto reply = protocol::from_worker(message_);
                       if (!reply.closes) {
                           do_read_header();
                       } else {
                           stream_.close();
                       }
                   } else {
                           stream_.close();
                   }
               });
}
