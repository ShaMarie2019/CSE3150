#include "PlayingCard.h"

#include <ostream>
#include <utility>

PlayingCard::PlayingCard(std::string suit, int rank)
    : suit_(std::move(suit)), rank_(rank) {}

std::string PlayingCard::rankToString(int rank) const {
    switch (rank) {
        case 1:
            return "A";
        case 11:
            return "J";
        case 12:
            return "Q";
        case 13:
            return "K";
        default:
            return std::to_string(rank);
    }
}

int PlayingCard::value() const {
    return rank_;
}

void PlayingCard::print(std::ostream& os) const {
    os << rankToString(rank_) << " of " << suit_;
}
