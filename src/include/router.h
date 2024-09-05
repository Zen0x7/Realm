#pragma once

#include <memory>
#include <functional>

class response : public std::enable_shared_from_this<response> {
};

class request : public std::enable_shared_from_this<request> {

};

typedef std::shared_ptr<request> request_type;
typedef std::shared_ptr<response> response_type;
typedef std::function<response_type(request_type &, response_type &)> callback_type;

class router : public std::enable_shared_from_this<router> {
public:
    router() = default;

    static std::shared_ptr<router> factory() {
        return std::make_shared<router>();
    }

    router GET(const std::string &path, callback_type callback);
    router POST(const std::string &path, callback_type callback);
    router PUT(const std::string &path, callback_type callback);
    router PATCH(const std::string &path, callback_type callback);
    router DELETE(const std::string &path, callback_type callback);
    router OPTIONS(const std::string &path, callback_type callback);
    router HEAD(const std::string &path, callback_type callback);
};
