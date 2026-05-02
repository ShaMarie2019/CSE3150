#pragma once

#include <string>
#include <vector>

#include "sim/types.h"

namespace sim {

struct Announcement {
    std::string         prefix;
    std::vector<ASN>    as_path;
    ASN                 next_hop      = 0;
    Relationship        received_from = Relationship::ORIGIN;
    bool                rov_invalid   = false;

    static Announcement make_origin(ASN seed, std::string p, bool rov_invalid_) {
        Announcement a;
        a.prefix        = std::move(p);
        a.as_path       = {seed};
        a.next_hop      = seed;
        a.received_from = Relationship::ORIGIN;
        a.rov_invalid   = rov_invalid_;
        return a;
    }
};

}
