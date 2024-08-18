#include <session.h>
#include <state.h>

void state::insert(session * session) {
    sessions_.insert({ session->get_id(), session });
}

void state::remove(const session * session) {
    sessions_.erase(session->get_id());
}
