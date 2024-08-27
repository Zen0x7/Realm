#include <client.h>

#include <protocol.h>

#include <iostream>
#include <boost/beast/core/bind_handler.hpp>

client::client(boost::asio::io_context &io_context) : resolver_(make_strand(io_context)), socket_(io_context) {
}

void client::do_resolve() {
    std::string server = "0.0.0.0";
    std::cout << "[INFO] Resolving ..." << std::endl;
    resolver_.async_resolve(server, "8000",
                                boost::beast::bind_front_handler(&client::on_resolve, shared_from_this()));
}

void client::run() {
    do_resolve();
}

void client::on_resolve(const boost::system::error_code &error_code,
                        const boost::asio::ip::tcp::resolver::results_type &endpoints) {
    if (!error_code) {
        std::cout << "[INFO] Connecting ..." << std::endl;
        async_connect(socket_, endpoints,
                      std::bind(&client::on_connect, this, boost::asio::placeholders::error));
    } else {
        throw std::invalid_argument("State can't be resolved");
    }
}

void client::on_connect(const boost::system::error_code &error_code) {
    if (!error_code) {
        std::cout << "[INFO] Connected" << std::endl;
        do_read_header();
    }
}

void client::do_read_header() {
    async_read(socket_, boost::asio::buffer(message_.data(), message::header_length_),
               [this](const boost::system::error_code &error_code, std::size_t length) {
                   if (!error_code && message_.decode()) {
                       do_read_body();
                   } else {
                       socket_.close();
                   }
               });
}

void client::do_read_body() {
    async_read(socket_, boost::asio::buffer(message_.body(), message_.body_length() + message::attribute_checksum_length_),
               [this](const boost::system::error_code &error_code, std::size_t length) {
                   if (!error_code) {
                       const auto reply = protocol::from_worker(message_);
                       if (!reply.closes) {
                           do_read_header();
                       } else {
                           socket_.close();
                       }
                   } else {
                       socket_.close();
                   }
               });
}
