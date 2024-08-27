#include <server.h>

#include <worker.h>
#include <iostream>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>

server::server(boost::asio::io_context &io_context, std::shared_ptr<state> const &state, short port)
    : state_(state),
      acceptor_(make_strand(io_context)),
      socket_(io_context),
      io_context_(io_context) {
    const auto endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port);
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(boost::asio::socket_base::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen(boost::asio::socket_base::max_listen_connections);
}

void server::run() {
    do_accept();
}

void server::do_accept() {
    acceptor_.async_accept(make_strand(io_context_), boost::beast::bind_front_handler(&server::on_accept, shared_from_this()));
}

void server::on_accept(const boost::system::error_code &error_code, boost::asio::ip::tcp::socket socket) {
    if (!error_code) {
        std::make_shared<worker>(std::move(socket), state_)->start();
    }

    do_accept();
}
