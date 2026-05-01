#include "Deck.h"

#include <algorithm>
#include <iterator>
#include <ostream>

size_t Deck::size() const {
    return cards_.size();
}

bool Deck::empty() const {
    return cards_.empty();
}

std::unique_ptr<Card> Deck::draw() {
    if (cards_.empty()) {
        return nullptr;
    }

    auto card = std::move(cards_.front());
    cards_.pop_front();
    return card;
}

void Deck::addToBottom(std::unique_ptr<Card> card) {
    cards_.push_back(std::move(card));
}

std::pair<Deck, Deck> Deck::split() {
    Deck a, b;
    size_t half = cards_.size() / 2;
    std::move(std::make_move_iterator(cards_.begin()),
              std::make_move_iterator(cards_.begin() + half),
              std::back_inserter(a.cards_));
    std::move(std::make_move_iterator(cards_.begin() + half),
              std::make_move_iterator(cards_.end()),
              std::back_inserter(b.cards_));
    cards_.clear();
    return {std::move(a), std::move(b)};
}

std::ostream& operator<<(std::ostream& os, const Deck& deck) {
    bool first = true;

    for (const auto& card : deck) {
        if (!first) {
            os << ", ";
        }
        first = false;
        os << *card;
    }

    return os;
}
