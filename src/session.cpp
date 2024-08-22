#include <session.h>

#include <boost/uuid/random_generator.hpp>
#include <iostream>

void session::do_read_header() {
    auto self(shared_from_this());
    async_read(socket_, boost::asio::buffer(message_.data(), message::header_length),
               [this, self](const boost::system::error_code &error_code, std::size_t /*length*/) {
                   if (!error_code && message_.decode_header()) {
                       do_read_body();
                   }
               });
}

void session::do_read_body() {
    auto self(shared_from_this());
    async_read(socket_, boost::asio::buffer(message_.body(), message_.body_length()),
               [this, self](const boost::system::error_code &error_code, std::size_t /*length*/) {
                   if (!error_code) {
                       std::cout << "Message received: " << message_.body() << std::endl;
                       do_read_header();
                   }
               });
}

void session::do_write() {
    auto self(shared_from_this());
    async_write(socket_, boost::asio::buffer(queue_.front().data(), queue_.front().length()),
                [this](const boost::system::error_code &error_code, std::size_t /*length*/) {
                    if (!error_code) {
                        queue_.pop_front();
                        if (!queue_.empty()) {
                            do_write();
                        }
                    } else {
                        socket_.close();
                    }
                });
}

session::session(boost::asio::ip::tcp::socket socket, std::shared_ptr<state> const &state) : state_(state),
    id_(boost::uuids::random_generator()()),
    socket_(std::move(socket)) {
}

session::~session() {
    state_->remove(this);
}

void session::start() {
    state_->insert(this);
    do_read_header();

    const std::string accepted_message = "EHLO";
    message welcome_message;
    welcome_message.body_length(accepted_message.size());
    std::memcpy(welcome_message.body(), accepted_message.data(), welcome_message.body_length());
    welcome_message.encode_header();

    this->write(welcome_message);
}

boost::uuids::uuid session::get_id() const {
    return id_;
}

void session::write(const message &message) {
    post(socket_.get_executor(), [this, message] {
        const bool write_in_progress = !queue_.empty();
        queue_.push_back(message);
        if (!write_in_progress) {
            do_write();
        }
    });
}
