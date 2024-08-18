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

void state::broadcast(std::string data) {
    auto const ss = std::make_shared<std::string const>(std::move(data));

    std::vector<std::weak_ptr<session>> v;
    {
        std::lock_guard lock(mutex_);
        v.reserve(sessions_.size());
        for (auto p: sessions_)
            v.emplace_back(p.second->weak_from_this());
    }

    for (auto const & wp : v)
        if (auto sp = wp.lock())
            sp->send(ss);
}
