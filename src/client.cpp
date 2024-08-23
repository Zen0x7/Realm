#include <client.h>
#include <protocol.h>

#include <iostream>

client::client(boost::asio::io_context &io_context) : resolver_(io_context), socket_(io_context) {
    do_resolve();
}

void client::do_resolve() {
    std::string server = "localhost";
    resolver_.async_resolve(server, "8000",
                            std::bind(&client::on_resolve, this,
                                      boost::asio::placeholders::error,
                                      boost::asio::placeholders::results));
}

void client::on_resolve(const boost::system::error_code &error_code,
                        const boost::asio::ip::tcp::resolver::results_type &endpoints) {
    if (!error_code) {
        async_connect(socket_, endpoints,
                      std::bind(&client::on_connect, this, boost::asio::placeholders::error));
    }
}

void client::on_connect(const boost::system::error_code &error_code) {
    if (!error_code) {
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
    async_read(socket_, boost::asio::buffer(message_.body(), message_.body_length()),
               [this](const boost::system::error_code &error_code, std::size_t length) {
                   if (!error_code) {
                       protocol::from_worker(message_.body(), message_.body_length());
                       do_read_header();
                   } else {
                       socket_.close();
                   }
               });
}
