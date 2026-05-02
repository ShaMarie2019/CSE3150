#include "sim/csv_io.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace sim {

namespace {

inline std::string trim(std::string s) {
    auto not_space = [](unsigned char c) { return !std::isspace(c); };
    auto a = std::find_if(s.begin(), s.end(), not_space);
    auto b = std::find_if(s.rbegin(), s.rend(), not_space).base();
    if (b <= a) return {};
    return std::string(a, b);
}

inline bool parse_bool(const std::string& s) {
    if (s == "True" || s == "true" || s == "TRUE" || s == "1") return true;
    return false;
}

}  

std::vector<Announcement> read_announcements_csv(const std::string& path) {
    std::ifstream f(path);
    if (!f) throw std::runtime_error("cannot open announcements CSV: " + path);

    std::vector<Announcement> out;
    std::string line;
    bool seen_header = false;
    while (std::getline(f, line)) {
        if (line.empty()) continue;
        if (!seen_header) {
            seen_header = true;
            if (line.find("seed_asn") != std::string::npos) continue;
        }

        
        std::string asn_s, prefix, flag;
        std::stringstream ss(line);
        if (!std::getline(ss, asn_s, ',')) continue;
        if (!std::getline(ss, prefix, ',')) continue;
        if (!std::getline(ss, flag, ',')) flag.clear();

        asn_s  = trim(asn_s);
        prefix = trim(prefix);
        flag   = trim(flag);
        if (asn_s.empty() || prefix.empty()) continue;

        try {
            ASN seed = static_cast<ASN>(std::stoul(asn_s));
            out.push_back(Announcement::make_origin(seed, prefix, parse_bool(flag)));
        } catch (...) {
        }
    }
    return out;
}

std::unordered_set<ASN> read_rov_asns_csv(const std::string& path) {
    std::ifstream f(path);
    if (!f) throw std::runtime_error("cannot open ROV ASNs CSV: " + path);

    std::unordered_set<ASN> out;
    std::string line;
    while (std::getline(f, line)) {
        std::string s = trim(line);
        if (s.empty()) continue;
        if (!std::isdigit(static_cast<unsigned char>(s[0]))) continue;
        try {
            out.insert(static_cast<ASN>(std::stoul(s)));
        } catch (...) {
        }
    }
    return out;
}

}
