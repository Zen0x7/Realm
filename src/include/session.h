#pragma once

#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include <boost/uuid/random_generator.hpp>

#include <state.h>

class session : public std::enable_shared_from_this<session> {
public:
    session(boost::asio::ip::tcp::socket socket, std::shared_ptr<state> const &state) : state_(state),
        id_(boost::uuids::random_generator()()),
        socket_(std::move(socket)) {
    }

    ~session();

    void start();

    std::string get_id() const;
private:
    void do_read();

    void do_write(std::size_t length);

    std::shared_ptr<state> state_;
    boost::uuids::uuid id_;
    boost::asio::ip::tcp::socket socket_;

    enum { max_length = 1024 };

    char data_[max_length];
};
