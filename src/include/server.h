#pragma once

#include <state.h>

#include <boost/asio/ts/internet.hpp>

class server : public std::enable_shared_from_this<server> {
public:
    server(boost::asio::io_context &io_context, std::shared_ptr<state> const &state, boost::asio::ip::tcp::endpoint endpoint);

    void run();
private:
    void do_accept();

    void on_accept(const boost::system::error_code &error_code, boost::asio::ip::tcp::socket socket);

    std::shared_ptr<state> state_;
    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::io_context &io_context_;
};
