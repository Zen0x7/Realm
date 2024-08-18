#include <server.h>
#include <session.h>

void server::do_accept() {
    acceptor_.async_accept(socket_,
                           [this](boost::system::error_code ec) {
                               if (!ec) {
                                   std::make_shared<session>(std::move(socket_), state_)->start();
                               }

                               do_accept();
                           });
}