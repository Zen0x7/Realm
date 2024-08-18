#pragma once

#include <memory>
#include <unordered_map>

#include <boost/uuid/uuid_io.hpp>

class session;

class state : public std::enable_shared_from_this<state> {
    std::unordered_map<std::string, session*> sessions_;
public:
    void insert(session * session);
    void remove(const session * session);
};