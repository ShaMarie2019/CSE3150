#pragma once

#include <memory>
#include <vector>

#include "sim/policy.h"
#include "sim/types.h"

namespace sim {

class AS {
public:
    explicit AS(ASN asn, std::unique_ptr<Policy> policy)
        : asn_(asn), policy_(std::move(policy)) {
        if (policy_) policy_->set_as(this);
    }

    ASN asn() const { return asn_; }

    const std::vector<ASN>& providers() const { return providers_; }
    const std::vector<ASN>& customers() const { return customers_; }
    const std::vector<ASN>& peers()     const { return peers_; }

    std::vector<ASN>& mutable_providers() { return providers_; }
    std::vector<ASN>& mutable_customers() { return customers_; }
    std::vector<ASN>& mutable_peers()     { return peers_; }

    int  propagation_rank() const { return propagation_rank_; }
    void set_propagation_rank(int r) { propagation_rank_ = r; }

    Policy*       policy()       { return policy_.get(); }
    const Policy* policy() const { return policy_.get(); }

    void set_policy(std::unique_ptr<Policy> p) {
        policy_ = std::move(p);
        if (policy_) policy_->set_as(this);
    }

private:
    ASN                       asn_;
    std::vector<ASN>          providers_;
    std::vector<ASN>          customers_;
    std::vector<ASN>          peers_;
    int                       propagation_rank_ = -1;
    std::unique_ptr<Policy>   policy_;
};

} 
