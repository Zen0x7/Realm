#include <state.h>

#include <worker.h>
#include <foreach.h>

void state::insert(worker *worker) {
    std::lock_guard lock(mutex_);
    workers_.insert({worker->get_id(), worker});
}

void state::remove(const worker *worker) {
    std::lock_guard lock(mutex_);
    workers_.erase(worker->get_id());
}

void state::broadcast(const std::string &data) {
    message to_broadcast = message::from_string(data);

    std::vector<std::weak_ptr<worker> > workers; {
        std::lock_guard lock(mutex_);
        workers.reserve(workers_.size());
        foreach (const auto &p, workers_)
            workers.emplace_back(p.second->weak_from_this());
    }

    foreach (auto const &reference, workers)
        if (const auto &worker = reference.lock())
            worker->write(to_broadcast);
}
