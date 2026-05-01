#include "JokerCard.h"

#include <ostream>
#include <utility>

JokerCard::JokerCard(std::string color)
    : color_(std::move(color)) {}

int JokerCard::value() const {
    return 14;
}

void JokerCard::print(std::ostream& os) const {
    os << color_ << " Joker";
}
