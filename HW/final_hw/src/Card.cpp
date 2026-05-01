#include "Card.h"

#include <ostream>

bool Card::operator<(const Card& other) const {
    return value() < other.value();
}

bool Card::operator==(const Card& other) const {
    return value() == other.value();
}

std::ostream& operator<<(std::ostream& os, const Card& card) {
    card.print(os);
    return os;
}
