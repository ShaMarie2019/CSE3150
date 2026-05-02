#include "sim/rov.h"

namespace sim {

bool ROV::receive(const Announcement& ann) {
    if (ann.rov_invalid) {
        return false;
    }
    return BGP::receive(ann);
}

}
