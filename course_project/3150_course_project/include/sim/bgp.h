#pragma once

#include "sim/policy.h"

namespace sim {
    
class BGP : public Policy {
public:
    bool receive(const Announcement& ann) override;
    void process_and_store() override;
    std::string name() const override { return "BGP"; }

protected:
    static bool prefer(const Announcement& candidate, const Announcement& current);
};

} 
