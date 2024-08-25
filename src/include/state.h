#pragma once

#include <boost/container_hash/hash.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>

#include <memory>
#include <unordered_map>

class worker;

class state : public std::enable_shared_from_this<state> {
    boost::uuids::uuid id_;
    std::unordered_map<boost::uuids::uuid, worker *, boost::hash<boost::uuids::uuid> > workers_;
    std::mutex mutex_;

public:
    state();
    boost::uuids::uuid get_id() const;
    void insert(worker *session);
    void remove(const worker *session);
    void broadcast(const std::string &data);
};
