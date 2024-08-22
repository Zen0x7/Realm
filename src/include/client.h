#pragma once

#include <deque>
#include <boost/asio/ts/internet.hpp>
#include <boost/asio.hpp>
#include <message.h>
#include <session.h>

typedef std::deque<message> message_queue;

class client {
public:
    explicit client(boost::asio::io_context & io_context);
    void do_resolve();
private:
    boost::asio::ip::tcp::resolver resolver_;
    boost::asio::ip::tcp::socket socket_;
    char buffer_[1024] = {};
    messages_queue queue_;
    message message_;
    void on_resolve(const boost::system::error_code & error_code, const boost::asio::ip::tcp::resolver::results_type & endpoints);
    void on_connect(const boost::system::error_code & error_code);
    void on_read(const boost::system::error_code & error_code, size_t bytes_transferred);
    void do_read_header();
    void do_read_body();
};
