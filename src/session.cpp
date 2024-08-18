#include <session.h>

#include <boost/asio/placeholders.hpp>
#include <boost/uuid/random_generator.hpp>

void session::on_send(std::shared_ptr<std::string const> const &data) {
    queue_.push_back(data);

    if (queue_.size() > 1) return;

    socket_.async_send(
        boost::asio::buffer(*queue_.front()),
        std::bind(&session::on_write, this, boost::asio::placeholders::error));
}

void session::on_write(const boost::system::error_code &error_code) {
    if (error_code) {
        return;
    }

    queue_.erase(queue_.begin());

    if (!queue_.empty()) {
        socket_.async_send(
            boost::asio::buffer(*queue_.front()),
            std::bind(&session::on_write, this, boost::asio::placeholders::error));
    }
}

void session::do_read() {
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
                            [this, self](const boost::system::error_code &error_code, std::size_t length) {
                                if (!error_code) {
                                    do_write(length);
                                }
                            });
}

void session::do_write(std::size_t length) {
    auto self(shared_from_this());
    async_write(socket_, boost::asio::buffer(data_, length),
                [this, self](const boost::system::error_code &error_code, std::size_t /*length*/) {
                    if (!error_code) {
                        do_read();
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
    do_read();
    const auto accepted_message = std::make_shared<const std::string>("EHLO");
    this->send(accepted_message);
}

std::string session::get_id() const {
    return to_string(id_);
}

void session::send(std::shared_ptr<std::string const> const &data) {
    post(socket_.get_executor(), std::bind(&session::on_send, this, data));
}
