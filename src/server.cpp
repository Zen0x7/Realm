#include <server.h>

#include <worker.h>

server::server(boost::asio::io_context &io_context, std::shared_ptr<state> const &state, short port)
    : state_(state),
      acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
      socket_(io_context) {
    do_accept();
}

void server::do_accept() {
    acceptor_.async_accept(socket_,
                           [this](const boost::system::error_code &error_code) {
                               if (!error_code) {
                                   std::make_shared<worker>(std::move(socket_), state_)->start();
                               }

                               do_accept();
                           });
}
