#pragma once

#include <cstdint>
#include <string>

namespace sim {

using ASN = std::uint32_t;


enum class Relationship : std::uint8_t {
    PROVIDER = 1,    // Got the announcement from a provider (worst).
    PEER     = 2,    // Got it from a peer.
    CUSTOMER = 3,    // Got it from a customer (best paying customer first).
    ORIGIN   = 4,    // The AS itself originated the announcement.
};

inline const char* relationship_name(Relationship r) {
    switch (r) {
        case Relationship::PROVIDER: return "provider";
        case Relationship::PEER:     return "peer";
        case Relationship::CUSTOMER: return "customer";
        case Relationship::ORIGIN:   return "origin";
    }
    return "unknown";
}

}  
