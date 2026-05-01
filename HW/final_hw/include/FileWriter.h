#pragma once

#include "Deck.h"

#include <fstream>
#include <string>

class FileWriter {
    std::ofstream file_;

public:
    explicit FileWriter(const std::string& path);
    void writeRound(int round, const Deck& a, const Deck& b);
};
