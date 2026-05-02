// Cycle-detection tests: provider/customer cycles must be reported, peer
// cycles must NOT be reported (peers are bidirectional and that's expected).

#include <gtest/gtest.h>

#include <sstream>

#include "sim/as_graph.h"

using namespace sim;

TEST(CycleTest, AcyclicGraphPasses) {
    // 1 -> 2 -> 3 (1 is top-level provider).
    std::stringstream ss(
        "1|2|-1|bgp\n"
        "2|3|-1|bgp\n");
    AsGraph g;
    g.load_caida(ss);
    EXPECT_NO_THROW(g.check_acyclic());
}

TEST(CycleTest, ProviderCycleThrows) {
    // 1 -> 2 -> 3 -> 1.
    std::stringstream ss(
        "1|2|-1|bgp\n"
        "2|3|-1|bgp\n"
        "3|1|-1|bgp\n");
    AsGraph g;
    g.load_caida(ss);
    EXPECT_THROW(g.check_acyclic(), CycleError);
}

TEST(CycleTest, SelfLoopIsACycle) {
    std::stringstream ss("1|1|-1|bgp\n");
    AsGraph g;
    g.load_caida(ss);
    EXPECT_THROW(g.check_acyclic(), CycleError);
}

TEST(CycleTest, PeerCyclesAreFine) {
    // Three ASes all peering with each other - allowed.
    std::stringstream ss(
        "1|2|0|bgp\n"
        "2|3|0|bgp\n"
        "3|1|0|bgp\n");
    AsGraph g;
    g.load_caida(ss);
    EXPECT_NO_THROW(g.check_acyclic());
}

TEST(CycleTest, MixedPeersAndProvidersNoCycle) {
    // Diamond: 1->2, 1->3, 2->4, 3->4. No cycle even though there are two
    // paths from 1 to 4.
    std::stringstream ss(
        "1|2|-1|bgp\n"
        "1|3|-1|bgp\n"
        "2|4|-1|bgp\n"
        "3|4|-1|bgp\n"
        "2|3|0|bgp\n");
    AsGraph g;
    g.load_caida(ss);
    EXPECT_NO_THROW(g.check_acyclic());
}
