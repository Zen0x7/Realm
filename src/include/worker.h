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

    void do_read_body();

    void do_write();

    std::shared_ptr<state> state_;
    boost::uuids::uuid id_;
    boost::asio::ip::tcp::socket socket_;
    message message_;
    messages_queue queue_;
};
