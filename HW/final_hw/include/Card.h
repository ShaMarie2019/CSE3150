#pragma once

#include <iosfwd>

class Card {
public:
    virtual int value() const = 0;
    virtual void print(std::ostream& os) const = 0;

    virtual bool operator<(const Card& other) const;
    virtual bool operator==(const Card& other) const;

    virtual ~Card() = default;

    friend std::ostream& operator<<(std::ostream& os, const Card& card);
};
