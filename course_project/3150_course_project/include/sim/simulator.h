#pragma once

#include <ostream>
#include <string>
#include <vector>

#include "sim/announcement.h"
#include "sim/as_graph.h"
#include "sim/types.h"

namespace sim {

class Simulator {
public:
    explicit Simulator(AsGraph& graph) : graph_(graph) {}

    void seed(const Announcement& ann);
    void seed_origin(ASN seed_asn, const std::string& prefix, bool rov_invalid);
    void propagate();
    void dump_ribs_csv(std::ostream& out) const;
    void dump_ribs_csv_file(const std::string& path) const;

private:
    AsGraph& graph_;

    void propagate_up();
    void propagate_across();
    void propagate_down();
};


std::string format_as_path(const std::vector<ASN>& path);

} 
