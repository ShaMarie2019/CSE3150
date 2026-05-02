#pragma once

#include <istream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "sim/as.h"
#include "sim/types.h"

namespace sim {

struct CycleError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};


class AsGraph {
public:
    AsGraph()  = default;
    ~AsGraph() = default;

    // Disable copy (we own AS objects). Movable.
    AsGraph(const AsGraph&)            = delete;
    AsGraph& operator=(const AsGraph&) = delete;
    AsGraph(AsGraph&&)                 = default;
    AsGraph& operator=(AsGraph&&)      = default;

    void load_caida(std::istream& in);
    void load_caida_file(const std::string& path);
    void check_acyclic() const;
    void compute_propagation_ranks();

    AS&       at(ASN asn);
    const AS& at(ASN asn) const;
    AS*       find(ASN asn);
    const AS* find(ASN asn) const;

    bool contains(ASN asn) const { return ases_.find(asn) != ases_.end(); }
    std::size_t size() const { return ases_.size(); }

    const std::unordered_map<ASN, std::shared_ptr<AS>>& ases() const { return ases_; }
    std::unordered_map<ASN, std::shared_ptr<AS>>&       mutable_ases() { return ases_; }
    const std::vector<std::vector<ASN>>& ranks() const { return ranks_; }


    AS& add_or_get(ASN asn);

    void add_provider_customer_edge(ASN provider, ASN customer);
    void add_peer_edge(ASN a, ASN b);

private:
    std::unordered_map<ASN, std::shared_ptr<AS>> ases_;
    std::vector<std::vector<ASN>>                ranks_;
};

} 
