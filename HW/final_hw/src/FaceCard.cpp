#include "FaceCard.h"

#include <ostream>
#include <utility>

FaceCard::FaceCard(std::string suit, int rank)
    : PlayingCard(std::move(suit), rank) {}

void FaceCard::print(std::ostream& os) const {
    std::string rankName;

    switch (rank_) {
        case 11:
            rankName = "Jack";
            break;
        case 12:
            rankName = "Queen";
            break;
        case 13:
            rankName = "King";
            break;
        default:
            rankName = rankToString(rank_);
            break;
    }

    os << rankName << " of " << suit_;
}
