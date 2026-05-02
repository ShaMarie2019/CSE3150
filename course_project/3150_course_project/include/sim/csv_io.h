#pragma once

#include <string>
#include <unordered_set>
#include <vector>

#include "sim/announcement.h"
#include "sim/types.h"

namespace sim {

std::vector<Announcement> read_announcements_csv(const std::string& path);
std::unordered_set<ASN> read_rov_asns_csv(const std::string& path);

}
