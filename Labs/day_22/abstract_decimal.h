#ifndef ABSTRACT_DECIMAL_H
#define ABSTRACT_DECIMAL_H

class Decimal {
public:
    virtual double convertToDecimal() const = 0;
    virtual ~Decimal() = default;
};

#endif