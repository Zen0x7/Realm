#include <session.h>

void session::do_read()  {
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
                            [this, self](boost::system::error_code ec, std::size_t length) {
                                if (!ec) {
                                    do_write(length);
                                }
                            });
}

void session::do_write(std::size_t length) {
    auto self(shared_from_this());
    async_write(socket_, boost::asio::buffer(data_, length),
                [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                    if (!ec) {
                        do_read();
                    }
                });
}

session::~session() {
    state_->remove(this);
}

void session::start() {
    state_->insert(this);
    do_read();
}

std::string session::get_id() const {
    return to_string(id_);
}
