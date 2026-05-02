#include "sim/bgp.h"

#include <utility>

#include "sim/as.h"

namespace sim {

bool BGP::prefer(const Announcement& candidate, const Announcement& current) {
    if (static_cast<int>(candidate.received_from) !=
        static_cast<int>(current.received_from)) {
        return static_cast<int>(candidate.received_from) >
               static_cast<int>(current.received_from);
    }
    if (candidate.as_path.size() != current.as_path.size()) {
        return candidate.as_path.size() < current.as_path.size();
    }
    return candidate.next_hop < current.next_hop;
}

bool BGP::receive(const Announcement& ann) {
    recv_queue_[ann.prefix].push_back(ann);
    return true;
}

void BGP::process_and_store() {
  
    const ASN our_asn = as_ ? as_->asn() : 0;

    for (auto& kv : recv_queue_) {
        const std::string& prefix      = kv.first;
        std::vector<Announcement>& q   = kv.second;
        if (q.empty()) continue;

        Announcement* best = &q.front();
        for (size_t i = 1; i < q.size(); ++i) {
            if (prefer(q[i], *best)) best = &q[i];
        }

        Announcement to_store = *best;
        to_store.as_path.insert(to_store.as_path.begin(), our_asn);

    
        auto it = local_rib_.find(prefix);
        if (it == local_rib_.end()) {
            local_rib_.emplace(prefix, std::move(to_store));
        } else if (it->second.received_from != Relationship::ORIGIN
                   && prefer(to_store, it->second)) {
            it->second = std::move(to_store);
        }
    }

    recv_queue_.clear();
}

}  
