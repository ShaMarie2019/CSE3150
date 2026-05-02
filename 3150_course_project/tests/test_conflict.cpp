// BGP conflict-resolution tests. Direct unit tests of BGP::prefer through a
// tiny graph, so we don't depend on full propagation correctness.

#include <gtest/gtest.h>

#include "sim/as.h"
#include "sim/announcement.h"
#include "sim/bgp.h"

using namespace sim;

namespace {

Announcement ann(Relationship rel, std::vector<ASN> path, ASN next_hop) {
    Announcement a;
    a.prefix        = "1.0.0.0/24";
    a.received_from = rel;
    a.as_path       = std::move(path);
    a.next_hop      = next_hop;
    return a;
}

// Test fixture exposing the protected static `prefer` for test purposes.
class TestableBGP : public BGP {
public:
    using BGP::prefer;
};

}  // namespace

TEST(ConflictTest, RelationshipBeatsPath) {
    auto from_customer = ann(Relationship::CUSTOMER, {2, 3, 4, 5}, 2);
    auto from_peer     = ann(Relationship::PEER,     {2},          2);
    EXPECT_TRUE(TestableBGP::prefer(from_customer, from_peer));
    EXPECT_FALSE(TestableBGP::prefer(from_peer, from_customer));
}

TEST(ConflictTest, CustomerBeatsProvider) {
    auto cust = ann(Relationship::CUSTOMER, {2, 3}, 2);
    auto prov = ann(Relationship::PROVIDER, {2},    2);
    EXPECT_TRUE(TestableBGP::prefer(cust, prov));
}

TEST(ConflictTest, PeerBeatsProvider) {
    auto peer = ann(Relationship::PEER,     {2, 3}, 2);
    auto prov = ann(Relationship::PROVIDER, {2},    2);
    EXPECT_TRUE(TestableBGP::prefer(peer, prov));
}

TEST(ConflictTest, ShortestPathBreaksTie) {
    auto a = ann(Relationship::CUSTOMER, {2, 3, 4}, 2);
    auto b = ann(Relationship::CUSTOMER, {2, 3},    2);
    EXPECT_TRUE(TestableBGP::prefer(b, a));
    EXPECT_FALSE(TestableBGP::prefer(a, b));
}

TEST(ConflictTest, LowerNextHopWinsLastResort) {
    auto a = ann(Relationship::CUSTOMER, {7, 9}, 7);
    auto b = ann(Relationship::CUSTOMER, {3, 9}, 3);
    EXPECT_TRUE(TestableBGP::prefer(b, a));
}

TEST(ConflictTest, SameAnnDoesNotPreferItself) {
    auto a = ann(Relationship::CUSTOMER, {7, 9}, 7);
    auto b = a;
    EXPECT_FALSE(TestableBGP::prefer(a, b));
    EXPECT_FALSE(TestableBGP::prefer(b, a));
}

TEST(ConflictTest, OriginBeatsAll) {
    auto origin = ann(Relationship::ORIGIN,   {1},    1);
    auto cust   = ann(Relationship::CUSTOMER, {1, 2}, 1);
    EXPECT_TRUE(TestableBGP::prefer(origin, cust));
    EXPECT_FALSE(TestableBGP::prefer(cust, origin));
}
