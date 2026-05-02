// Tests for the ROV policy.
//
// Receiving a ROV-invalid announcement on an ROV AS should drop it before
// it ever enters the recv queue or local RIB. Valid announcements should
// flow through exactly like in BGP.

#include <gtest/gtest.h>

#include "sim/announcement.h"
#include "sim/as.h"
#include "sim/rov.h"

using namespace sim;

TEST(ROVTest, DropsInvalidAnnouncement) {
    AS as(42, std::make_unique<ROV>());
    Announcement bad = Announcement::make_origin(7, "1.2.3.0/24", /*rov_invalid=*/true);
    bad.received_from = Relationship::CUSTOMER;
    bad.next_hop      = 7;

    EXPECT_FALSE(as.policy()->receive(bad));
    as.policy()->process_and_store();
    EXPECT_TRUE(as.policy()->local_rib().empty());
    EXPECT_TRUE(as.policy()->recv_queue().empty());
}

TEST(ROVTest, AcceptsValidAnnouncement) {
    AS as(42, std::make_unique<ROV>());
    Announcement good = Announcement::make_origin(7, "1.2.3.0/24", false);
    good.received_from = Relationship::CUSTOMER;
    good.next_hop      = 7;

    EXPECT_TRUE(as.policy()->receive(good));
    as.policy()->process_and_store();
    ASSERT_EQ(as.policy()->local_rib().size(), 1u);
    EXPECT_EQ(as.policy()->local_rib().at("1.2.3.0/24").as_path.front(), 42u);
}

TEST(ROVTest, MixedValidAndInvalid) {
    // ROV AS receives one invalid (drop) and one valid (keep) announcement.
    AS as(42, std::make_unique<ROV>());
    Announcement bad  = Announcement::make_origin(7, "1.2.3.0/24", true);
    bad.received_from = Relationship::CUSTOMER; bad.next_hop = 7;
    Announcement good = Announcement::make_origin(8, "1.2.3.0/24", false);
    good.received_from = Relationship::CUSTOMER; good.next_hop = 8;

    EXPECT_FALSE(as.policy()->receive(bad));
    EXPECT_TRUE(as.policy()->receive(good));
    as.policy()->process_and_store();

    ASSERT_EQ(as.policy()->local_rib().size(), 1u);
    const auto& a = as.policy()->local_rib().at("1.2.3.0/24");
    EXPECT_EQ(a.as_path.back(), 8u);  // came from AS 8, not the bad AS 7
    EXPECT_FALSE(a.rov_invalid);
}
