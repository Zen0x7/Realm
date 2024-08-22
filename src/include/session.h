#pragma once

#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>

#include <state.h>
#include <message.h>
#include <deque>

typedef std::deque<message> messages_queue;

class session : public std::enable_shared_from_this<session> {
public:
    session(boost::asio::ip::tcp::socket socket, std::shared_ptr<state> const &state);

    ~session();

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
