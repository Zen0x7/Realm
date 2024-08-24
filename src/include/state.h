#pragma once

#include <boost/container_hash/hash.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <memory>
#include <unordered_map>

class worker;

class state : public std::enable_shared_from_this<state> {
    std::unordered_map<boost::uuids::uuid, worker *, boost::hash<boost::uuids::uuid> > workers_;
    std::mutex mutex_;

public:
    void insert(worker *session);

    void remove(const worker *session);

    void broadcast(const std::string &data);
};
