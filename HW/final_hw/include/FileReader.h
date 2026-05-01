#pragma once

#include "Deck.h"

#include <string>

class FileReader {
public:
    static Deck readDeckFromCSV(const std::string& path);
};
