#include "sim/as_graph.h"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "sim/bgp.h"

namespace sim {

namespace {

inline bool parse_uint(const char* s, const char* end, ASN& out) {
    if (s == end) return false;
    ASN v = 0;
    for (const char* p = s; p != end; ++p) {
        if (*p < '0' || *p > '9') return false;
        v = v * 10 + static_cast<ASN>(*p - '0');
    }
    out = v;
    return true;
}

inline bool parse_int(const char* s, const char* end, int& out) {
    if (s == end) return false;
    bool neg = false;
    if (*s == '-') { neg = true; ++s; if (s == end) return false; }
    int v = 0;
    for (const char* p = s; p != end; ++p) {
        if (*p < '0' || *p > '9') return false;
        v = v * 10 + (*p - '0');
    }
    out = neg ? -v : v;
    return true;
}

}  

AS& AsGraph::add_or_get(ASN asn) {
    auto it = ases_.find(asn);
    if (it != ases_.end()) return *it->second;
    auto as = std::make_shared<AS>(asn, std::make_unique<BGP>());
    auto& ref = *as;
    ases_.emplace(asn, std::move(as));
    return ref;
}

void AsGraph::add_provider_customer_edge(ASN provider, ASN customer) {
    AS& p = add_or_get(provider);
    AS& c = add_or_get(customer);
    auto& cust = p.mutable_customers();
    if (std::find(cust.begin(), cust.end(), customer) == cust.end())
        cust.push_back(customer);
    auto& prov = c.mutable_providers();
    if (std::find(prov.begin(), prov.end(), provider) == prov.end())
        prov.push_back(provider);
}

void AsGraph::add_peer_edge(ASN a, ASN b) {
    AS& A = add_or_get(a);
    AS& B = add_or_get(b);
    auto& pa = A.mutable_peers();
    if (std::find(pa.begin(), pa.end(), b) == pa.end()) pa.push_back(b);
    auto& pb = B.mutable_peers();
    if (std::find(pb.begin(), pb.end(), a) == pb.end()) pb.push_back(a);
}

void AsGraph::load_caida(std::istream& in) {
    
    std::string buf((std::istreambuf_iterator<char>(in)),
                    std::istreambuf_iterator<char>());
    const char* p   = buf.data();
    const char* end = p + buf.size();

    
    if (!buf.empty()) {
        ases_.reserve(buf.size() / 250 + 64);
    }

    while (p < end) {
        const char* line_end = static_cast<const char*>(std::memchr(p, '\n', end - p));
        if (!line_end) line_end = end;

        if (p == line_end || *p == '#') {
            p = (line_end < end) ? line_end + 1 : end;
            continue;
        }

        const char* fields[4] = {p, nullptr, nullptr, nullptr};
        int field_count = 1;
        for (const char* q = p; q < line_end && field_count < 4; ++q) {
            if (*q == '|') {
                fields[field_count++] = q + 1;
            }
        }

        const char* end0 = (field_count > 1) ? fields[1] - 1 : line_end;
        const char* end1 = (field_count > 2) ? fields[2] - 1 : line_end;
        const char* end2 = (field_count > 3) ? fields[3] - 1 : line_end;

        if (field_count >= 3) {
            ASN a = 0, b = 0;
            int rel = 0;
            if (parse_uint(fields[0], end0, a)
                && parse_uint(fields[1], end1, b)
                && parse_int(fields[2], end2, rel)) {
                if (rel == -1) {
                    add_provider_customer_edge(a, b);
                } else if (rel == 0) {
                    add_peer_edge(a, b);
                }
       
            }
        }

        p = (line_end < end) ? line_end + 1 : end;
    }
}

void AsGraph::load_caida_file(const std::string& path) {
    std::ifstream f(path);
    if (!f) throw std::runtime_error("cannot open CAIDA file: " + path);
    load_caida(f);
}

AS& AsGraph::at(ASN asn) {
    auto it = ases_.find(asn);
    if (it == ases_.end()) throw std::out_of_range("AS not in graph");
    return *it->second;
}

const AS& AsGraph::at(ASN asn) const {
    auto it = ases_.find(asn);
    if (it == ases_.end()) throw std::out_of_range("AS not in graph");
    return *it->second;
}

AS* AsGraph::find(ASN asn) {
    auto it = ases_.find(asn);
    return it == ases_.end() ? nullptr : it->second.get();
}

const AS* AsGraph::find(ASN asn) const {
    auto it = ases_.find(asn);
    return it == ases_.end() ? nullptr : it->second.get();
}

void AsGraph::check_acyclic() const {
 
    enum Color : std::uint8_t { WHITE = 0, GREY = 1, BLACK = 2 };
    std::unordered_map<ASN, Color> color;
    color.reserve(ases_.size() * 2);

    struct Frame { ASN asn; std::size_t idx; };
    std::vector<Frame> stack;
    stack.reserve(64);

    for (const auto& kv : ases_) {
        ASN root = kv.first;
        if (color[root] != WHITE) continue;

        stack.push_back({root, 0});
        color[root] = GREY;

        while (!stack.empty()) {
            Frame& top = stack.back();
            const AS& a = *ases_.at(top.asn);
            const auto& children = a.customers();
            if (top.idx < children.size()) {
                ASN child = children[top.idx++];
                Color& cc = color[child];
                if (cc == GREY) {
                    std::ostringstream oss;
                    oss << "provider/customer cycle detected: AS " << top.asn
                        << " -> AS " << child
                        << " closes a cycle (provider edges form a loop)";
                    throw CycleError(oss.str());
                }
                if (cc == WHITE) {
                    cc = GREY;
                    stack.push_back({child, 0});
                }
            } else {
                color[top.asn] = BLACK;
                stack.pop_back();
            }
        }
    }
}

void AsGraph::compute_propagation_ranks() {
    ranks_.clear();


    std::unordered_map<ASN, std::size_t> remaining;
    remaining.reserve(ases_.size() * 2);

    std::queue<ASN> ready;
    for (const auto& kv : ases_) {
        std::size_t n = kv.second->customers().size();
        remaining[kv.first] = n;
        if (n == 0) {
            kv.second->set_propagation_rank(0);
            ready.push(kv.first);
        }
    }

    int max_rank = 0;
    while (!ready.empty()) {
        ASN asn = ready.front(); ready.pop();
        AS& me = *ases_.at(asn);
        int my_rank = me.propagation_rank();
        max_rank = std::max(max_rank, my_rank);
        for (ASN provider : me.providers()) {
            AS& p = *ases_.at(provider);
            int candidate = my_rank + 1;
            if (candidate > p.propagation_rank()) {
                p.set_propagation_rank(candidate);
            }
            auto& rem = remaining[provider];
            if (rem > 0) --rem;
            if (rem == 0) ready.push(provider);
        }
    }

    ranks_.assign(static_cast<std::size_t>(max_rank) + 1, {});
    for (const auto& kv : ases_) {
        int r = kv.second->propagation_rank();
        if (r >= 0) {
            ranks_[static_cast<std::size_t>(r)].push_back(kv.first);
        }
    }
}

} 
