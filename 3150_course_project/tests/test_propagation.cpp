// End-to-end propagation tests on hand-built mini-graphs.
//
// These act as system tests for the simulator: build a tiny graph, seed an
// announcement, run propagate(), and check what's in each AS's local RIB.

#include <gtest/gtest.h>

#include <sstream>

#include "sim/as_graph.h"
#include "sim/rov.h"
#include "sim/simulator.h"

using namespace sim;

namespace {

// Build the example graph from bgpsimulator.com mentioned in the spec:
//   AS 4 has customers {3, 666} and peer 777.
//   AS 3 and AS 666 are leaves.
//   AS 777 peers with AS 4 (only).
AsGraph make_example_graph() {
    std::stringstream ss(
        "4|3|-1|bgp\n"
        "4|666|-1|bgp\n"
        "4|777|0|bgp\n");
    AsGraph g;
    g.load_caida(ss);
    g.compute_propagation_ranks();
    return g;
}

}  // namespace

TEST(PropagationTest, SeededOriginStays) {
    auto g = make_example_graph();
    Simulator sim(g);
    sim.seed_origin(666, "1.2.0.0/16", false);
    sim.propagate();

    const auto& rib666 = g.at(666).policy()->local_rib();
    ASSERT_EQ(rib666.size(), 1u);
    const auto& a = rib666.at("1.2.0.0/16");
    EXPECT_EQ(a.received_from, Relationship::ORIGIN);
    ASSERT_EQ(a.as_path.size(), 1u);
    EXPECT_EQ(a.as_path[0], 666u);
}

TEST(PropagationTest, AnnouncementGoesUpToProvider) {
    auto g = make_example_graph();
    Simulator sim(g);
    sim.seed_origin(666, "1.2.0.0/16", false);
    sim.propagate();

    // AS 4 is provider of 666; should have learned via customer 666.
    const auto& rib4 = g.at(4).policy()->local_rib();
    ASSERT_EQ(rib4.size(), 1u);
    const auto& a = rib4.at("1.2.0.0/16");
    EXPECT_EQ(a.received_from, Relationship::CUSTOMER);
    ASSERT_EQ(a.as_path.size(), 2u);
    EXPECT_EQ(a.as_path[0], 4u);
    EXPECT_EQ(a.as_path[1], 666u);
}

TEST(PropagationTest, PeerLearnsFromCustomerAnnouncement) {
    auto g = make_example_graph();
    Simulator sim(g);
    sim.seed_origin(666, "1.2.0.0/16", false);
    sim.propagate();

    // AS 777 peers with AS 4. AS 4 learned from customer 666, so it should
    // relay the announcement across to peer 777.
    const auto& rib777 = g.at(777).policy()->local_rib();
    ASSERT_EQ(rib777.size(), 1u);
    const auto& a = rib777.at("1.2.0.0/16");
    EXPECT_EQ(a.received_from, Relationship::PEER);
    ASSERT_EQ(a.as_path.size(), 3u);
    EXPECT_EQ(a.as_path[0], 777u);
    EXPECT_EQ(a.as_path[1], 4u);
    EXPECT_EQ(a.as_path[2], 666u);
}

TEST(PropagationTest, AnnouncementGoesDownToOtherCustomer) {
    auto g = make_example_graph();
    Simulator sim(g);
    sim.seed_origin(666, "1.2.0.0/16", false);
    sim.propagate();

    // AS 3 is sibling of 666 (also customer of 4). The announcement should
    // travel up to 4 then back down to 3.
    const auto& rib3 = g.at(3).policy()->local_rib();
    ASSERT_EQ(rib3.size(), 1u);
    const auto& a = rib3.at("1.2.0.0/16");
    EXPECT_EQ(a.received_from, Relationship::PROVIDER);
    ASSERT_EQ(a.as_path.size(), 3u);
    EXPECT_EQ(a.as_path[0], 3u);
    EXPECT_EQ(a.as_path[1], 4u);
    EXPECT_EQ(a.as_path[2], 666u);
}

TEST(PropagationTest, ShortestPathWinsAtSharedProvider) {
    // Provider 4 has two customers (3 and 666) both announcing the same
    // prefix. AS 4 should pick whichever has the shorter path -- they're
    // both 1-hop here, so the lower next_hop ASN wins (3).
    auto g = make_example_graph();
    Simulator sim(g);
    sim.seed_origin(3, "1.2.0.0/16", false);
    sim.seed_origin(666, "1.2.0.0/16", false);
    sim.propagate();

    const auto& rib4 = g.at(4).policy()->local_rib();
    ASSERT_EQ(rib4.size(), 1u);
    EXPECT_EQ(rib4.at("1.2.0.0/16").next_hop, 3u);
}

TEST(PropagationTest, ROVDeployerDropsHijack) {
    // 666 hijacks 1.2.0.0/16 with rov_invalid=true. AS 4 deploys ROV and
    // should NOT learn the hijack, so AS 4's RIB should be empty (no other
    // legitimate announcement in this scenario).
    auto g = make_example_graph();
    g.at(4).set_policy(std::make_unique<ROV>());
    Simulator sim(g);
    sim.seed_origin(666, "1.2.0.0/16", true);
    sim.propagate();

    EXPECT_TRUE(g.at(4).policy()->local_rib().empty());
    EXPECT_TRUE(g.at(3).policy()->local_rib().empty());
    EXPECT_TRUE(g.at(777).policy()->local_rib().empty());
    // Origin AS still has its own announcement.
    EXPECT_EQ(g.at(666).policy()->local_rib().size(), 1u);
}

TEST(PropagationTest, FormatAsPathSingle) {
    EXPECT_EQ(format_as_path({42}), "(42,)");
}

TEST(PropagationTest, FormatAsPathMultiple) {
    EXPECT_EQ(format_as_path({1, 2, 3}), "(1, 2, 3)");
}
