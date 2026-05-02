#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>

#include "sim/as_graph.h"
#include "sim/bgp.h"
#include "sim/csv_io.h"
#include "sim/rov.h"
#include "sim/simulator.h"

namespace {

void print_usage(const char* prog) {
    std::fprintf(stderr,
        "usage: %s --caida <relations.txt> --anns <anns.csv> "
        "--rov <rov_asns.csv> --output <ribs.csv>\n", prog);
}

struct Args {
    std::string caida;
    std::string anns;
    std::string rov;
    std::string output;
};

bool parse_args(int argc, char** argv, Args& a) {
    for (int i = 1; i < argc; ++i) {
        std::string flag = argv[i];
        if (i + 1 >= argc) return false;
        std::string val = argv[++i];
        if      (flag == "--caida")  a.caida  = val;
        else if (flag == "--anns")   a.anns   = val;
        else if (flag == "--rov")    a.rov    = val;
        else if (flag == "--output") a.output = val;
        else return false;
    }
    return !a.caida.empty() && !a.anns.empty() && !a.output.empty();
}

}  

int main(int argc, char** argv) {
    Args args;
    if (!parse_args(argc, argv, args)) {
        print_usage(argv[0]);
        return 1;
    }

    sim::AsGraph graph;
    try {
        graph.load_caida_file(args.caida);
    } catch (const std::exception& e) {
        std::fprintf(stderr, "error loading CAIDA: %s\n", e.what());
        return 1;
    }

    try {
        graph.check_acyclic();
    } catch (const sim::CycleError& e) {
        std::fprintf(stderr, "error: %s\n", e.what());
        return 2;  
    }

    graph.compute_propagation_ranks();

    if (!args.rov.empty()) {
        try {
            auto rov_asns = sim::read_rov_asns_csv(args.rov);
            for (sim::ASN asn : rov_asns) {
                if (sim::AS* as = graph.find(asn)) {
                    as->set_policy(std::make_unique<sim::ROV>());
                }
            }
        } catch (const std::exception& e) {
            std::fprintf(stderr, "error loading ROV ASNs: %s\n", e.what());
            return 1;
        }
    }

    sim::Simulator simulator(graph);
    try {
        auto anns = sim::read_announcements_csv(args.anns);
        for (const auto& a : anns) simulator.seed(a);
    } catch (const std::exception& e) {
        std::fprintf(stderr, "error loading announcements: %s\n", e.what());
        return 1;
    }

    simulator.propagate();

    try {
        simulator.dump_ribs_csv_file(args.output);
    } catch (const std::exception& e) {
        std::fprintf(stderr, "error writing output: %s\n", e.what());
        return 1;
    }

    return 0;
}
