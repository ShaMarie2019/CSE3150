// Propagation-rank assignment tests.
//
// Per Section 3.3: rank 0 = ASes with no customers; rank N+1 = providers of
// rank-N ASes. Ranks must be assigned bottom-up so the simulator can iterate
// in the correct order.

#include <gtest/gtest.h>

#include <sstream>

#include "sim/as_graph.h"

using namespace sim;

TEST(RankTest, SingleNode) {
    AsGraph g;
    g.add_or_get(42);
    g.compute_propagation_ranks();
    EXPECT_EQ(g.at(42).propagation_rank(), 0);
}

TEST(RankTest, LinearChain) {
    // 1 -> 2 -> 3. 3 has no customers (rank 0). 2 has rank 1. 1 has rank 2.
    std::stringstream ss(
        "1|2|-1|bgp\n"
        "2|3|-1|bgp\n");
    AsGraph g;
    g.load_caida(ss);
    g.compute_propagation_ranks();

    EXPECT_EQ(g.at(3).propagation_rank(), 0);
    EXPECT_EQ(g.at(2).propagation_rank(), 1);
    EXPECT_EQ(g.at(1).propagation_rank(), 2);
}

TEST(RankTest, DiamondHighestPathWins) {
    // Two paths from 1 down to 4 of differing length: longer one decides
    // the rank because we want max(customer_rank)+1.
    //   1 -> 2 -> 4
    //   1 -> 3 -> 5 -> 4
    std::stringstream ss(
        "1|2|-1|bgp\n"
        "2|4|-1|bgp\n"
        "1|3|-1|bgp\n"
        "3|5|-1|bgp\n"
        "5|4|-1|bgp\n");
    AsGraph g;
    g.load_caida(ss);
    g.compute_propagation_ranks();

    EXPECT_EQ(g.at(4).propagation_rank(), 0);
    EXPECT_EQ(g.at(5).propagation_rank(), 1);
    EXPECT_EQ(g.at(2).propagation_rank(), 1);
    EXPECT_EQ(g.at(3).propagation_rank(), 2);
    EXPECT_EQ(g.at(1).propagation_rank(), 3);
}

TEST(RankTest, RanksVectorPopulated) {
    std::stringstream ss(
        "1|2|-1|bgp\n"
        "2|3|-1|bgp\n");
    AsGraph g;
    g.load_caida(ss);
    g.compute_propagation_ranks();

    const auto& ranks = g.ranks();
    ASSERT_EQ(ranks.size(), 3u);  // ranks 0, 1, 2
    EXPECT_EQ(ranks[0].size(), 1u);
    EXPECT_EQ(ranks[0][0], 3u);
    EXPECT_EQ(ranks[1][0], 2u);
    EXPECT_EQ(ranks[2][0], 1u);
}

TEST(RankTest, PeersDontAffectRanks) {
    // Peer edges shouldn't promote anyone.
    std::stringstream ss(
        "1|2|-1|bgp\n"
        "1|3|0|bgp\n"
        "2|3|0|bgp\n");
    AsGraph g;
    g.load_caida(ss);
    g.compute_propagation_ranks();

    EXPECT_EQ(g.at(2).propagation_rank(), 0);  // no customers
    EXPECT_EQ(g.at(3).propagation_rank(), 0);  // no customers
    EXPECT_EQ(g.at(1).propagation_rank(), 1);
}
