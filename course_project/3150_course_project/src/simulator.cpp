#include "sim/simulator.h"

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <stdexcept>
#include <vector>

#include "sim/as.h"

namespace sim {

std::string format_as_path(const std::vector<ASN>& path) {
    if (path.empty()) return "()";
    std::string out;
    out.reserve(path.size() * 8 + 4);
    out.push_back('(');
    char buf[16];
    for (std::size_t i = 0; i < path.size(); ++i) {
        int n = std::snprintf(buf, sizeof(buf), "%u", path[i]);
        out.append(buf, n);
        if (i + 1 < path.size()) out.append(", ");
    }
    if (path.size() == 1) out.push_back(',');
    out.push_back(')');
    return out;
}

void Simulator::seed(const Announcement& ann) {
    AS* as = graph_.find(ann.next_hop);
    if (!as) return;
    as->policy()->seed(ann);
}

void Simulator::seed_origin(ASN seed_asn, const std::string& prefix, bool rov_invalid) {
    AS* as = graph_.find(seed_asn);
    if (!as) return;
    as->policy()->seed(Announcement::make_origin(seed_asn, prefix, rov_invalid));
}

namespace {

enum class UpOrDown { Up, Down };

void send_one_step(AS& sender, AsGraph& g, UpOrDown dir) {
    const auto& targets = (dir == UpOrDown::Up) ? sender.providers()
                                                : sender.customers();
    if (targets.empty()) return;
    const auto& rib = sender.policy()->local_rib();
    if (rib.empty()) return;

    Relationship recv_rel = (dir == UpOrDown::Up) ? Relationship::CUSTOMER
                                                  : Relationship::PROVIDER;

    for (ASN tgt : targets) {
        AS* receiver = g.find(tgt);
        if (!receiver) continue;
        for (const auto& kv : rib) {
            const Announcement& src = kv.second;
            bool on_path = false;
            for (ASN x : src.as_path) {
                if (x == tgt) { on_path = true; break; }
            }
            if (on_path) continue;

            Announcement out = src;
            out.next_hop      = sender.asn();
            out.received_from = recv_rel;
            receiver->policy()->receive(out);
        }
    }
}

}

void Simulator::propagate_up() {
    const auto& ranks = graph_.ranks();
    for (std::size_t r = 0; r < ranks.size(); ++r) {
        for (ASN asn : ranks[r]) {
            AS& a = graph_.at(asn);
            send_one_step(a, graph_, UpOrDown::Up);
        }
        if (r + 1 < ranks.size()) {
            for (ASN asn : ranks[r + 1]) {
                graph_.at(asn).policy()->process_and_store();
            }
        }
    }
}

void Simulator::propagate_across() {
    for (const auto& kv : graph_.ases()) {
        AS& sender = *kv.second;
        const auto& peers = sender.peers();
        if (peers.empty()) continue;
        const auto& rib = sender.policy()->local_rib();
        if (rib.empty()) continue;
        for (ASN peer_asn : peers) {
            AS* receiver = graph_.find(peer_asn);
            if (!receiver) continue;
            for (const auto& pkv : rib) {
                const Announcement& src = pkv.second;
                bool on_path = false;
                for (ASN x : src.as_path) {
                    if (x == peer_asn) { on_path = true; break; }
                }
                if (on_path) continue;

                if (src.received_from != Relationship::CUSTOMER
                    && src.received_from != Relationship::ORIGIN) {
                    continue;
                }
                Announcement out = src;
                out.next_hop      = sender.asn();
                out.received_from = Relationship::PEER;
                receiver->policy()->receive(out);
            }
        }
    }
  
    for (const auto& kv : graph_.ases()) {
        kv.second->policy()->process_and_store();
    }
}

void Simulator::propagate_down() {
    const auto& ranks = graph_.ranks();
    if (ranks.empty()) return;
    for (std::size_t i = ranks.size(); i-- > 0; ) {
        for (ASN asn : ranks[i]) {
            AS& a = graph_.at(asn);
            send_one_step(a, graph_, UpOrDown::Down);
        }
        if (i > 0) {
            for (ASN asn : ranks[i - 1]) {
                graph_.at(asn).policy()->process_and_store();
            }
        }
    }
}

void Simulator::propagate() {
    if (graph_.ranks().empty()) {
        graph_.compute_propagation_ranks();
    }
    propagate_up();
    propagate_across();
    propagate_down();
}

void Simulator::dump_ribs_csv(std::ostream& out) const {
    out << "asn,prefix,as_path\n";
    std::vector<ASN> asns;
    asns.reserve(graph_.size());
    for (const auto& kv : graph_.ases()) asns.push_back(kv.first);
    std::sort(asns.begin(), asns.end());

    for (ASN asn : asns) {
        const AS& a = graph_.at(asn);
        const auto& rib = a.policy()->local_rib();

        std::vector<const std::string*> prefixes;
        prefixes.reserve(rib.size());

        for (const auto& kv : rib) prefixes.push_back(&kv.first);
        std::sort(prefixes.begin(), prefixes.end(),
                  [](const std::string* x, const std::string* y) { return *x < *y; });
       
        for (const std::string* p : prefixes) {
            const Announcement& ann = rib.at(*p);
            out << asn << ',' << *p << ",\"" << format_as_path(ann.as_path) << "\"\n";
        }
    }
}

void Simulator::dump_ribs_csv_file(const std::string& path) const {
    std::ofstream f(path);
    if (!f) throw std::runtime_error("cannot open output: " + path);
    dump_ribs_csv(f);
}

}