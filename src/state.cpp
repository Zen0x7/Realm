#include <state.h>

#include <session.h>

void state::insert(session * session) {
    std::lock_guard lock(mutex_);
    sessions_.insert({ session->get_id(), session });
}

void state::remove(const session * session) {
    std::lock_guard lock(mutex_);
    sessions_.erase(session->get_id());
}

void state::broadcast(const std::string & data) {
    message message;
    message.body_length(data.size());
    std::memcpy(message.body(), data.data(), message.body_length());
    message.encode_header();

    std::vector<std::weak_ptr<session>> sessions;
    {
        std::lock_guard lock(mutex_);
        sessions.reserve(sessions_.size());
        for (auto p: sessions_)
            sessions.emplace_back(p.second->weak_from_this());
    }

    for (auto const & reference : sessions)
        if (auto session = reference.lock())
            session->write(message);
}
