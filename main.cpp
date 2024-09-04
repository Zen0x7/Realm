#include <cstdlib>
#include <iostream>
#include <memory>
#include <thread>
#include <constants.h>

#include <boost/asio/signal_set.hpp>
#include <boost/program_options.hpp>

#include <state.h>
#include <server.h>
#include <client.h>
#include <iostream>

int main(int argc, char *argv[]) {
    std::ios::sync_with_stdio(true);
    boost::program_options::options_description program_description("Allowed options");

    program_description.add_options()
            ("help", "Get more details about options")
            ("serve-as", boost::program_options::value<std::string>()->default_value("both"),
             "Serve as `state`, `worker` or `both`")
            ("debug", boost::program_options::value<bool>()->default_value(false),
             "Serve with debug globally enabled")
            ("threads", boost::program_options::value<int>()->default_value(1), "Threads assigned");

    boost::program_options::options_description state_description("State related options");

    state_description.add_options()
            ("state_host", boost::program_options::value<std::string>()->default_value("0.0.0.0"),
             "State host")
            ("state_port", boost::program_options::value<unsigned short>()->default_value(8000),
             "State port")
            ("state_debug", boost::program_options::value<bool>()->default_value(false), "State with debug enabled");

    boost::program_options::options_description worker_description("Worker related options");

    worker_description.add_options()
            ("worker_debug", boost::program_options::value<bool>()->default_value(false), "Worker with debug enabled");

    boost::program_options::options_description commandline_description;

    commandline_description.add(program_description).add(state_description).add(worker_description);

    boost::program_options::variables_map vm;
    store(parse_command_line(argc, argv, commandline_description), vm);
    notify(vm);

    std::cout << INFO << "2024 © Ian Torres — All rights reserved" << std::endl;
    std::cout << INFO << "Realm version "
            << VERSION
            << std::endl;

    if (vm.contains("help")) {
        std::cout << commandline_description << std::endl;
        return EXIT_FAILURE;
    }

    auto threads_number_ = vm["threads"].as<int>();

    if (threads_number_ < 1) {
        std::cout << WARN << "Option `threads` should be equals or greater than `1`" << std::endl;
        threads_number_ = 1;
    }

    boost::asio::io_context io_context_ { threads_number_ };
    const auto state_ = std::make_shared<state>();
    const auto serve_as_ = vm["serve-as"].as<std::string>();

    if (serve_as_ == "state") {
        std::cout << INFO << "Server running as `state`" << std::endl;
        auto const address_ = boost::asio::ip::make_address("0.0.0.0");
        std::make_shared<server>(io_context_, state_, boost::asio::ip::tcp::endpoint{address_, 8000})->run();
    } else if (serve_as_ == "worker") {
        std::cout << INFO << "Server running as `worker`" << std::endl;
        std::make_shared<client>(io_context_)->run();
    } else if (serve_as_ == "both") {
        std::cout << INFO << "Server running as `state` and `worker`" << std::endl;
        auto const address_ = boost::asio::ip::make_address("0.0.0.0");
        std::make_shared<server>(io_context_, state_, boost::asio::ip::tcp::endpoint{address_, 8000})->run();
        std::make_shared<client>(io_context_)->run();
    } else {
        std::cerr << ERROR << "Option `server-as` should be `state`, `worker` or `both`" << std::endl;
        return EXIT_FAILURE;
    }

    std::vector<std::thread> threads_;
    threads_.reserve(threads_number_ - 1);
    for (auto i = threads_number_ - 1; i > 0; --i)
        threads_.emplace_back([&io_context_] { io_context_.run(); });

    boost::asio::signal_set signals_(io_context_, SIGINT, SIGTERM);
    signals_.async_wait(
        [&](boost::system::error_code const &, int) {
            io_context_.stop();
        });

    io_context_.run();

    for (auto &thread: threads_)
        thread.join();

    return EXIT_SUCCESS;
}
