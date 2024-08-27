#pragma once

#include <message_queue.h>

#include <boost/asio.hpp>

class client : public std::enable_shared_from_this<client> {
public:
    explicit client(boost::asio::io_context &io_context);

    void do_resolve();

    void run();

private:
    boost::asio::ip::tcp::resolver resolver_;
    boost::asio::ip::tcp::socket socket_;
    char buffer_[1024] = {};
    messages_queue queue_;
    message message_;

    void on_resolve(const boost::system::error_code &error_code,
                    const boost::asio::ip::tcp::resolver::results_type &endpoints);

    void on_connect(const boost::system::error_code &error_code);

    void do_read_header();

    void do_read_body();
};
