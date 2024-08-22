#pragma once

#include <boost/asio/ts/internet.hpp>

#include <state.h>

class server {
public:
    server(boost::asio::io_context &io_context, std::shared_ptr<state> const &state, short port);

private:
    void do_accept();

    std::shared_ptr<state> state_;
    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::ip::tcp::socket socket_;
};
