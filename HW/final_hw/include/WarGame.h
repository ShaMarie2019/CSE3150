#pragma once

#include "Deck.h"
#include "FileWriter.h"

#include <string>

class WarGame {
    Deck playerA_;
    Deck playerB_;
    int round_;
    FileWriter writer_;

    void playRound();

public:
    WarGame(Deck deck, const std::string& outputPath);

    void play();
};
