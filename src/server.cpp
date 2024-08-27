#include <server.h>

#include <constants.h>
#include <worker.h>
#include <iostream>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/stacktrace.hpp>

server::server(boost::asio::io_context &io_context, std::shared_ptr<state> const &state, boost::asio::ip::tcp::endpoint endpoint)
    : state_(state),
      acceptor_(make_strand(io_context)),
      io_context_(io_context) {
    try {
        boost::system::error_code error_code;
        acceptor_.open(endpoint.protocol(), error_code);
        if (error_code) throw std::runtime_error("Can't open the acceptor");
        std::cout << INFO << "Acceptor has been openned ... " << std::endl;
        acceptor_.set_option(boost::asio::socket_base::reuse_address(true), error_code);
        if (error_code) throw std::runtime_error("Can't reuse_address(true) option");
        std::cout << INFO << "Acceptor has been assigned with reuse address option ... " << std::endl;
        acceptor_.bind(endpoint, error_code);
        if (error_code) throw std::runtime_error("Can't bind the acceptor");
        std::cout << INFO << "Acceptor has been binded ... " << std::endl;
        acceptor_.listen(boost::asio::socket_base::max_listen_connections, error_code);
        if (error_code) throw std::runtime_error("Can't set max connections");
        std::cout << INFO << "Acceptor has been limited in connections ... " << std::endl;
    } catch (std::runtime_error & runtime_error) {
        std::cout << boost::stacktrace::stacktrace();
        std::cerr << ERROR << runtime_error.what() << std::endl;
        std::abort();
    }
}

void server::run() {
    dispatch(acceptor_.get_executor(), boost::beast::bind_front_handler(&server::do_accept, shared_from_this()));
}

void server::do_accept() {
    std::cout << INFO << "Acceptor is waiting for a new connection ... " << std::endl;
    acceptor_.async_accept(make_strand(io_context_), boost::beast::bind_front_handler(&server::on_accept, shared_from_this()));
}

void server::on_accept(const boost::system::error_code &error_code, boost::asio::ip::tcp::socket socket) {
    if (error_code) {
        std::cout << WARN << "Connection error: " << error_code.what() << std::endl;
    } else {
        std::cout << INFO << "Connection received ... " << std::endl;
        std::make_shared<worker>(std::move(socket), state_)->start();
    }

    do_accept();
}
