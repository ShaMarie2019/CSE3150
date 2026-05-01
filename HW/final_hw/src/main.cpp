#include "FileReader.h"
#include "WarGame.h"

#include <exception>
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: ./war_game <input_csv> <output_csv>\n";
        return 1;
    }

    try {
        Deck deck = FileReader::readDeckFromCSV(argv[1]);
        WarGame game(std::move(deck), argv[2]);
        game.play();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
