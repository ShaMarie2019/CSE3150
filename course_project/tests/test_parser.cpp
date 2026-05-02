// Unit tests for AsGraph::load_caida.
//
// Exercises: comments are skipped, provider/customer rows produce both
// directions of the edge, peer rows produce a symmetric edge, malformed
// lines are tolerated, and the parser handles trailing newlines.

#include <gtest/gtest.h>

#include <algorithm>
#include <sstream>

#include "sim/as_graph.h"

using namespace sim;

namespace {

// Helper: assert vector contains the value.
::testing::AssertionResult HasASN(const std::vector<ASN>& v, ASN x) {
    if (std::find(v.begin(), v.end(), x) != v.end()) {
        return ::testing::AssertionSuccess();
    }
    return ::testing::AssertionFailure() << "ASN " << x << " not found";
}

}  // namespace

TEST(ParserTest, SkipsComments) {
    std::string input =
        "# comment line one\n"
        "# source:topology|BGP|...\n"
        "1|2|-1|bgp\n";
    std::stringstream ss(input);
    AsGraph g;
    g.load_caida(ss);
    EXPECT_EQ(g.size(), 2u);
}

TEST(ParserTest, ProviderCustomerEdgeBothSides) {
    std::string input = "10|20|-1|bgp\n";
    std::stringstream ss(input);
    AsGraph g;
    g.load_caida(ss);

    ASSERT_TRUE(g.contains(10));
    ASSERT_TRUE(g.contains(20));
    EXPECT_TRUE(HasASN(g.at(10).customers(), 20));
    EXPECT_TRUE(HasASN(g.at(20).providers(), 10));
    EXPECT_TRUE(g.at(10).peers().empty());
    EXPECT_TRUE(g.at(20).peers().empty());
}

TEST(ParserTest, PeerEdgeIsSymmetric) {
    std::string input = "10|20|0|bgp\n";
    std::stringstream ss(input);
    AsGraph g;
    g.load_caida(ss);

    EXPECT_TRUE(HasASN(g.at(10).peers(), 20));
    EXPECT_TRUE(HasASN(g.at(20).peers(), 10));
    EXPECT_TRUE(g.at(10).providers().empty());
    EXPECT_TRUE(g.at(10).customers().empty());
}

TEST(ParserTest, IgnoresSourceField) {
    // Same provider/customer pair from different sources should produce
    // a single edge, not duplicates.
    std::string input =
        "5|6|-1|bgp\n"
        "5|6|-1|something_else\n";
    std::stringstream ss(input);
    AsGraph g;
    g.load_caida(ss);

    EXPECT_EQ(g.at(5).customers().size(), 1u);
    EXPECT_EQ(g.at(6).providers().size(), 1u);
}

TEST(ParserTest, MultipleEdgesPerNode) {
    std::string input =
        "1|2|-1|bgp\n"
        "1|3|-1|bgp\n"
        "1|4|0|bgp\n"
        "5|2|-1|bgp\n";
    std::stringstream ss(input);
    AsGraph g;
    g.load_caida(ss);

    EXPECT_EQ(g.at(1).customers().size(), 2u);  // 2 and 3
    EXPECT_EQ(g.at(1).peers().size(), 1u);      // 4
    EXPECT_EQ(g.at(2).providers().size(), 2u);  // 1 and 5
}

TEST(ParserTest, HandlesMalformedLinesGracefully) {
    std::string input =
        "1|2|-1|bgp\n"
        "garbage line\n"
        "3|4|-1|bgp\n"
        "\n"  // empty
        "5|6|-1|bgp\n";
    std::stringstream ss(input);
    AsGraph g;
    g.load_caida(ss);
    // 6 ASes from 3 valid lines.
    EXPECT_EQ(g.size(), 6u);
}

TEST(ParserTest, EmptyInput) {
    std::stringstream ss("");
    AsGraph g;
    g.load_caida(ss);
    EXPECT_EQ(g.size(), 0u);
}
