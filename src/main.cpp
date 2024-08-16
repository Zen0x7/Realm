#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <thread>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include <boost/asio/signal_set.hpp>

using boost::asio::ip::tcp;

class session : public std::enable_shared_from_this<session> {
public:
    session(tcp::socket socket) : socket_(std::move(socket)) { }

    void start() {
        do_read();
    }

private:
    void do_read() {
        auto self(shared_from_this());
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
                                [this, self](boost::system::error_code ec, std::size_t length) {
                                    if (!ec) {
                                        do_write(length);
                                    }
                                });
    }

    void do_write(std::size_t length) {
        auto self(shared_from_this());
        async_write(socket_, boost::asio::buffer(data_, length),
                    [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                        if (!ec) {
                            do_read();
                        }
                    });
    }

    tcp::socket socket_;

    enum { max_length = 1024 };

    char data_[max_length];
};

class server {
public:
    server(boost::asio::io_context &io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
          socket_(io_context) {
        do_accept();
    }

private:
    void do_accept() {
        acceptor_.async_accept(socket_,
                               [this](boost::system::error_code ec) {
                                   if (!ec) {
                                       std::make_shared<session>(std::move(socket_))->start();
                                   }

                                   do_accept();
                               });
    }

    tcp::acceptor acceptor_;
    tcp::socket socket_;
};

int main(int argc, char *argv[]) {
    boost::asio::io_context io_context_;
    server server_(io_context_, 8000);
    auto threads_number_ = std::thread::hardware_concurrency();

    std::vector<std::thread> threads_;
    threads_.reserve(threads_number_ - 1);
    for (auto i = threads_number_ - 1; i > 0; --i)
        threads_.emplace_back([&io_context_] { io_context_.run(); });

    boost::asio::signal_set _signals(io_context_, SIGINT, SIGTERM);
    _signals.async_wait(
        [&](boost::system::error_code const&, int)
        {
            io_context_.stop();
        });

    io_context_.run();

    return EXIT_SUCCESS;
}
