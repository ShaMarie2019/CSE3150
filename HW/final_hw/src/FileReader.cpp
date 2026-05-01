#include "FileReader.h"

#include "FaceCard.h"
#include "JokerCard.h"
#include "PlayingCard.h"

#include <fstream>
#include <memory>
#include <stdexcept>

Deck FileReader::readDeckFromCSV(const std::string& path) {
    Deck deck;
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open input deck: " + path);
    }

    try {
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) {
                continue;
            }

            size_t commaPos = line.find(',');
            if (commaPos == std::string::npos) {
                throw std::runtime_error("Malformed CSV input");
            }

            std::string suit = line.substr(0, commaPos);
            std::string value = line.substr(commaPos + 1);

            if (suit.empty() || value.empty()) {
                throw std::runtime_error("Malformed CSV input");
            }

            if (suit == "Joker") {
                deck.addToBottom(std::make_unique<JokerCard>(value));
            } else {
                int rank = std::stoi(value);
                if (rank < 1 || rank > 13) {
                    throw std::runtime_error("Malformed CSV input");
                }

                if (rank >= 11) {
                    deck.addToBottom(std::make_unique<FaceCard>(suit, rank));
                } else {
                    deck.addToBottom(std::make_unique<PlayingCard>(suit, rank));
                }
            }
        }
    } catch (...) {
        throw std::runtime_error("Malformed CSV input");
    }

    if (deck.empty()) {
        throw std::runtime_error("Empty or invalid CSV deck");
    }

    return deck;
}
