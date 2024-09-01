#pragma once

#include <state.h>
#include <message_queue.h>

#include <boost/asio/ts/internet.hpp>

class worker : public std::enable_shared_from_this<worker> {
public:
    worker(boost::asio::ip::tcp::socket && socket, std::shared_ptr<state> const &state);

    ~worker();

    void start();

    boost::uuids::uuid get_id() const;

    void write(const message &message);

private:
    void do_read_header();

    void on_read_header(const boost::system::error_code &error_code, std::size_t length);

    void do_read_body();

    void on_read_body(const boost::system::error_code &error_code, std::size_t length);

    void do_write();

    void on_write(const boost::system::error_code &error_code, std::size_t length);

    std::shared_ptr<state> state_;
    boost::uuids::uuid id_;
    boost::asio::ip::tcp::socket socket_;
    message message_;
    messages_queue queue_;
};
