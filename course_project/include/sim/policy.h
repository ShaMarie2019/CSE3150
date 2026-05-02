#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "sim/announcement.h"
#include "sim/types.h"

namespace sim {

class AS; 

class Policy {
public:
    virtual ~Policy() = default;
    void set_as(AS* as) { as_ = as; }
    virtual bool receive(const Announcement& ann) = 0;
    virtual void process_and_store() = 0;

    const std::unordered_map<std::string, Announcement>& local_rib() const { return local_rib_; }
    std::unordered_map<std::string, Announcement>&        mutable_local_rib() { return local_rib_; }

    const std::unordered_map<std::string, std::vector<Announcement>>& recv_queue() const { return recv_queue_; }
    std::unordered_map<std::string, std::vector<Announcement>>&       mutable_recv_queue() { return recv_queue_; }

    void seed(Announcement ann) {
        std::string p = ann.prefix;
        local_rib_[std::move(p)] = std::move(ann);
    }

    virtual std::string name() const = 0;

protected:
    AS*                                                            as_ = nullptr;
    std::unordered_map<std::string, Announcement>                  local_rib_;
    std::unordered_map<std::string, std::vector<Announcement>>     recv_queue_;
};

}
