#pragma once

#include <boost/asio/ts/internet.hpp>

class server {
public:
    server(boost::asio::io_context &io_context, short port)
        : acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
          socket_(io_context) {
        do_accept();
    }

private:
    void do_accept();

    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::ip::tcp::socket socket_;
};