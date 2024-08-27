#pragma once

#include <message_queue.h>

#include <boost/asio.hpp>
#include <boost/beast/core/tcp_stream.hpp>

class client : public std::enable_shared_from_this<client> {
public:
    explicit client(boost::asio::io_context &io_context);

    void do_resolve();

    void run();

private:
    boost::asio::ip::tcp::resolver resolver_;
    boost::beast::tcp_stream stream_;
    char buffer_[1024] = {};
    messages_queue queue_;
    message message_;

    void on_resolve(const boost::system::error_code &error_code,
                    boost::asio::ip::tcp::resolver::results_type endpoints);

    void on_connect(const boost::system::error_code &error_code, boost::asio::ip::tcp::resolver::results_type::endpoint_type);

    void do_read_header();

    void do_read_body();
};
