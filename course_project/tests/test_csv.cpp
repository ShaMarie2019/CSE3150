// Tests for CSV input helpers (announcements + ROV ASNs).

#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>
#include <string>

#include "sim/csv_io.h"

using namespace sim;

namespace {

// Write a string to a temp file and return its path.
std::string write_temp(const std::string& contents, const std::string& name) {
    std::string path = "/tmp/" + name;
    std::ofstream f(path);
    f << contents;
    return path;
}

}  // namespace

TEST(CsvTest, ReadsAnnouncements) {
    std::string p = write_temp(
        "seed_asn,prefix,rov_invalid\n"
        "27,1.2.0.0/16,False\n"
        "25,1.2.0.0/16,True\n",
        "test_anns_basic.csv");

    auto anns = read_announcements_csv(p);
    ASSERT_EQ(anns.size(), 2u);

    EXPECT_EQ(anns[0].next_hop, 27u);
    EXPECT_EQ(anns[0].as_path.size(), 1u);
    EXPECT_EQ(anns[0].as_path[0], 27u);
    EXPECT_EQ(anns[0].prefix, "1.2.0.0/16");
    EXPECT_FALSE(anns[0].rov_invalid);

    EXPECT_EQ(anns[1].next_hop, 25u);
    EXPECT_TRUE(anns[1].rov_invalid);

    std::remove(p.c_str());
}

TEST(CsvTest, IgnoresBlankLines) {
    std::string p = write_temp(
        "seed_asn,prefix,rov_invalid\n"
        "\n"
        "1,10.0.0.0/24,False\n"
        "\n",
        "test_anns_blank.csv");
    auto anns = read_announcements_csv(p);
    EXPECT_EQ(anns.size(), 1u);
    std::remove(p.c_str());
}

TEST(CsvTest, ReadsRovAsns) {
    std::string p = write_temp("174\n43\n68\n27\n", "test_rov_basic.csv");
    auto s = read_rov_asns_csv(p);
    EXPECT_EQ(s.size(), 4u);
    EXPECT_TRUE(s.count(174));
    EXPECT_TRUE(s.count(43));
    EXPECT_TRUE(s.count(68));
    EXPECT_TRUE(s.count(27));
    std::remove(p.c_str());
}

TEST(CsvTest, RovAsnsToleratesBlanksAndHeader) {
    std::string p = write_temp("asn\n123\n\n456\n", "test_rov_header.csv");
    auto s = read_rov_asns_csv(p);
    EXPECT_EQ(s.size(), 2u);
    EXPECT_TRUE(s.count(123));
    EXPECT_TRUE(s.count(456));
    std::remove(p.c_str());
}

TEST(CsvTest, MissingFileThrows) {
    EXPECT_THROW(read_announcements_csv("/tmp/does_not_exist_abc123.csv"),
                 std::runtime_error);
    EXPECT_THROW(read_rov_asns_csv("/tmp/does_not_exist_abc123.csv"),
                 std::runtime_error);
}
