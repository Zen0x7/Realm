#include <worker.h>

#include <protocol.h>

#include <boost/asio/ts/buffer.hpp>
#include <boost/uuid/random_generator.hpp>

void worker::do_read_header() {
    auto self(shared_from_this());
    async_read(socket_, boost::asio::buffer(message_.data(), message::header_length_),
               [this, self](const boost::system::error_code &error_code, std::size_t /*length*/) {
                   if (!error_code && message_.decode()) {
                       do_read_body();
                   } else {
                       socket_.close();
                   }
               });
}

void worker::do_read_body() {
    auto self(shared_from_this());
    async_read(socket_, boost::asio::buffer(message_.body(), message_.body_length() + message::attribute_checksum_length_),
               [this, self](const boost::system::error_code &error_code, std::size_t /*length*/) {
                   if (!error_code) {
                       const auto reply = protocol::from_server(message_);
                       if (!reply.closes) {
                           do_read_header();
                       } else {
                           socket_.close();
                       }
                   } else {
                       socket_.close();
                   }
               });
}

void worker::do_write() {
    auto self(shared_from_this());
    async_write(socket_, boost::asio::buffer(queue_.front().data(), queue_.front().length() + message::attribute_checksum_length_),
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

worker::worker(boost::asio::ip::tcp::socket socket, std::shared_ptr<state> const &state) : state_(state),
    id_(boost::uuids::random_generator()()),
    socket_(std::move(socket)) {
}

worker::~worker() {
    state_->remove(this);
}

void worker::start() {
    state_->insert(this);
    do_read_header();

    const auto accepted_message = message::from_string("EHLO", state_->get_id());

    this->write(accepted_message);
}

boost::uuids::uuid worker::get_id() const {
    return id_;
}

void worker::write(const message &message) {
    post(socket_.get_executor(), [this, message] {
        const bool write_in_progress = !queue_.empty();
        queue_.push_back(message);
        if (!write_in_progress) {
            do_write();
        }
    });
}
