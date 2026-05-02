#pragma once

#include "sim/bgp.h"

namespace sim {

class ROV : public BGP {
public:
    bool receive(const Announcement& ann) override;
    std::string name() const override { return "ROV"; }
};

}
