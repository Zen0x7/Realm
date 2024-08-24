#include <state.h>

#include <session.h>
#include <foreach.h>

void state::insert(session *session) {
    std::lock_guard lock(mutex_);
    sessions_.insert({session->get_id(), session});
}

void state::remove(const session *session) {
    std::lock_guard lock(mutex_);
    sessions_.erase(session->get_id());
}

void state::broadcast(const std::string &data) {
    message to_broadcast = message::from_string(data);

    std::vector<std::weak_ptr<session> > sessions; {
        std::lock_guard lock(mutex_);
        sessions.reserve(sessions_.size());
        foreach (const auto p, sessions_)
            sessions.emplace_back(p.second->weak_from_this());
    }

    foreach (auto const &reference, sessions)
        if (const auto session = reference.lock())
            session->write(to_broadcast);
}
