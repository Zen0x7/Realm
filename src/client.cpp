#include <client.h>

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
        do_read();
    }
}

void client::on_read(const boost::system::error_code &error_code, size_t bytes_transferred) {
    std::cout << buffer_ << std::endl;
    do_read();
}

void client::do_read() {
    socket_.async_receive(boost::asio::buffer(buffer_), std::bind(&client::on_read, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}
