#pragma once

#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>

#include <state.h>

class session : public std::enable_shared_from_this<session> {
public:
    session(boost::asio::ip::tcp::socket socket, std::shared_ptr<state> const &state);
    ~session();
    void start();
    std::string get_id() const;
    void send(std::shared_ptr<std::string const> const &data);
    void on_send(std::shared_ptr<std::string const> const &data);
    void on_write(const boost::system::error_code &error_code);
private:
    void do_read();
    void do_write(std::size_t length);
    std::shared_ptr<state> state_;
    boost::uuids::uuid id_;
    boost::asio::ip::tcp::socket socket_;
    std::vector<std::shared_ptr<std::string const> > queue_;
    enum { max_length = 1024 };
    char data_[max_length];
};
