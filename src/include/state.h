#pragma once

#include <memory>
#include <unordered_map>
#include <boost/container_hash/hash.hpp>

#include <boost/uuid/uuid_io.hpp>

class session;

class state : public std::enable_shared_from_this<state> {
    std::unordered_map<boost::uuids::uuid, session*, boost::hash<boost::uuids::uuid>> sessions_;
    std::mutex mutex_;
public:
    void insert(session * session);
    void remove(const session * session);
    void broadcast(const std::string & data);
};