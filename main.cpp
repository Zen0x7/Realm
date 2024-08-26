#include <cstdlib>
#include <iostream>
#include <memory>
#include <thread>

#include <boost/asio/signal_set.hpp>
#include <boost/program_options.hpp>

#include <state.h>
#include <server.h>
#include <client.h>

#define VERSION "1.0.0"

int main(int argc, char *argv[]) {
    boost::program_options::options_description program_description("Allowed options");

    program_description.add_options()
            ("help", "Get more details about options")
            ("threads", boost::program_options::value<int>()->default_value(1), "Quantity of I/O threads assigned")
            ("server-as", boost::program_options::value<std::string>()->default_value("state"),
             "Serve this program as `state` or `worker`");

    boost::program_options::variables_map vm;
    store(parse_command_line(argc, argv, program_description), vm);
    notify(vm);

    std::cout << "[INFO] 2024 © Ian Torres — All rights reserved" << std::endl;

    if (vm.contains("help")) {
        std::cout << "[INFO] Server version "
                << VERSION
                << std::endl << std::endl;
        std::cout << program_description << std::endl;
        return EXIT_FAILURE;
    }

    boost::asio::io_context io_context_;
    const auto state_ = std::make_shared<state>();
    const auto server_as_ = vm["server-as"].as<std::string>();

    if (server_as_ == "state") {
        std::cout << "[INFO] Server running as `state`" << std::endl;
        server server_(io_context_, state_, 8000);
    } else if (server_as_ == "worker") {
        std::cout << "[INFO] Server running as `worker`" << std::endl;
        client client_(io_context_);
    } else {
        std::cerr << "[ERROR] Option `server-as` should be `state` or `worker`" << std::endl;
        return EXIT_FAILURE;
    }

    auto threads_number_ = vm["threads"].as<int>();

    if (threads_number_ < 1) {
        std::cout << "[WARN] Option `threads` should be equals or greater than `1`" << std::endl;
        threads_number_ = 1;
    }

    std::vector<std::thread> threads_;
    threads_.reserve(threads_number_ - 1);
    for (auto i = threads_number_ - 1; i > 0; --i)
        threads_.emplace_back([&io_context_] { io_context_.run(); });

    boost::asio::signal_set _signals(io_context_, SIGINT, SIGTERM);
    _signals.async_wait(
        [&](boost::system::error_code const &, int) {
            io_context_.stop();
        });

    io_context_.run();

    return EXIT_SUCCESS;
}
