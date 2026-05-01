#pragma once

#include "Card.h"

#include <deque>
#include <iosfwd>
#include <memory>
#include <utility>

class Deck {
    std::deque<std::unique_ptr<Card>> cards_;

public:
    Deck() = default;
    Deck(Deck&&) = default;
    Deck& operator=(Deck&&) = default;

    Deck(const Deck&) = delete;
    Deck& operator=(const Deck&) = delete;

    size_t size() const;
    bool empty() const;

    auto begin() { return cards_.begin(); }
    auto end() { return cards_.end(); }
    auto begin() const { return cards_.cbegin(); }
    auto end() const { return cards_.cend(); }

    std::unique_ptr<Card> draw();
    void addToBottom(std::unique_ptr<Card> card);
    std::pair<Deck, Deck> split();

    friend std::ostream& operator<<(std::ostream& os, const Deck& deck);
};
