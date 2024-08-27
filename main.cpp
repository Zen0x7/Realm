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
            ("serve-as", boost::program_options::value<std::string>()->default_value("both"),
             "Serve as `state`, `worker` or `both`")
            ("debug", boost::program_options::value<bool>()->default_value(false),
             "Serve with debug globally enabled");

    boost::program_options::options_description state_description("State related options");

    state_description.add_options()
    ("state_host", boost::program_options::value<std::string>()->default_value("0.0.0.0"),
     "State host")
    ("state_port", boost::program_options::value<unsigned short>()->default_value(8000),
     "State port")
    ("state_debug", boost::program_options::value<bool>()->default_value(false),"State with debug enabled")
    ("state_threads", boost::program_options::value<int>()->default_value(1),"State threads assigned");

    boost::program_options::options_description worker_description("Worker related options");

    worker_description.add_options()
    ("worker_debug", boost::program_options::value<bool>()->default_value(false),"Worker with debug enabled")
    ("worker_threads", boost::program_options::value<int>()->default_value(1),"Worker threads assigned");

    boost::program_options::options_description commandline_description;

    commandline_description.add(program_description).add(state_description).add(worker_description);

    boost::program_options::variables_map vm;
    store(parse_command_line(argc, argv, commandline_description), vm);
    notify(vm);

    std::cout << "[INFO] 2024 © Ian Torres — All rights reserved" << std::endl;
    std::cout << "[INFO] Realm version "
        << VERSION
        << std::endl;

    if (vm.contains("help")) {
        std::cout << commandline_description << std::endl;
        return EXIT_FAILURE;
    }

    boost::asio::io_context state_io_context_;
    boost::asio::io_context worker_io_context_;
    const auto state_ = std::make_shared<state>();
    const auto serve_as_ = vm["serve-as"].as<std::string>();

    if (serve_as_ == "state") {
        std::cout << "[INFO] Server running as `state`" << std::endl;
        std::make_shared<server>(state_io_context_, state_, 8000)->run();
    } else if (serve_as_ == "worker") {
        std::cout << "[INFO] Server running as `worker`" << std::endl;
        std::make_shared<client>(worker_io_context_)->run();
    } else if (serve_as_ == "both") {
        std::cout << "[INFO] Server running as `state` and `worker`" << std::endl;
        std::make_shared<server>(state_io_context_, state_, 8000)->run();
        std::make_shared<client>(worker_io_context_)->run();
    } else {
        std::cerr << "[ERROR] Option `server-as` should be `state`, `worker` or `both`" << std::endl;
        return EXIT_FAILURE;
    }

    auto state_threads_number_ = vm["state_threads"].as<int>();
    auto worker_threads_number_ = vm["worker_threads"].as<int>();

    if ((serve_as_ == "state" || serve_as_ == "both") && state_threads_number_ < 1) {
        std::cout << "[WARN] Option `state_threads` should be equals or greater than `1`" << std::endl;
        state_threads_number_ = 1;
    }

    if ((serve_as_ == "worker" || serve_as_ == "both") && worker_threads_number_ < 1) {
        std::cout << "[WARN] Option `worker_threads` should be equals or greater than `1`" << std::endl;
        worker_threads_number_ = 1;
    }

    if (serve_as_ == "state" || serve_as_ == "both") {
        std::vector<std::thread> state_threads_;
        state_threads_.reserve(state_threads_number_ - 1);
        for (auto i = state_threads_number_ - 1; i > 0; --i)
            state_threads_.emplace_back([&state_io_context_] { state_io_context_.run(); });

        boost::asio::signal_set state_signals_(state_io_context_, SIGINT, SIGTERM);
        state_signals_.async_wait(
            [&](boost::system::error_code const &, int) {
                state_io_context_.stop();
            });
    }

    if (serve_as_ == "worker" || serve_as_ == "both") {
        std::vector<std::thread> worker_threads_;
        worker_threads_.reserve(worker_threads_number_ - 1);
        for (auto i = worker_threads_number_ - 1; i > 0; --i)
            worker_threads_.emplace_back([&worker_io_context_] { worker_io_context_.run(); });

        boost::asio::signal_set state_signals_(worker_io_context_, SIGINT, SIGTERM);
        state_signals_.async_wait(
            [&](boost::system::error_code const &, int) {
                worker_io_context_.stop();
            });
    }

    std::thread state_thread_([&state_io_context_] () {
        state_io_context_.run();
    });

    std::thread worker_thread_([&worker_io_context_] () {
        worker_io_context_.run();
    });

    if (serve_as_ == "state" || serve_as_ == "both") {
        state_thread_.detach();
    }

    if (serve_as_ == "worker" || serve_as_ == "both") {
        worker_thread_.detach();
    }

    std::thread heartbeat([] () {
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(5000));
            std::cout << "Running ..." << std::endl;
        }
    });

    heartbeat.join();

    return EXIT_SUCCESS;
}
