#include <cstdlib>
#include <iostream>
#include <memory>
#include <thread>

#include <boost/asio/signal_set.hpp>

#include <state.h>
#include <server.h>

int main(int argc, char *argv[]) {
    boost::asio::io_context io_context_;
    const auto state_ = std::make_shared<state>();
    server server_(io_context_, state_, 8000);
    const auto threads_number_ = std::thread::hardware_concurrency();

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
