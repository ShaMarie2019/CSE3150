#include "fraction.h"

void Fraction::check_null() const {
    if (!numer_ || !denom_) {
        throw std::runtime_error("can't dereference null pointers");
    }
}

Fraction::Fraction() {
    std::cout << "I am in the default constructor" << std::endl;
    numer_ = new int(0);
    denom_ = new int(1);
}

Fraction::Fraction(int n, int d) {
    std::cout << "I am in the custom constructor" << std::endl;

    if (d == 0) {
        throw std::runtime_error("Can't divide by zero!");
    }

    numer_ = new int(n);
    denom_ = new int(d);
}

Fraction::Fraction(const Fraction& other) {
    std::cout << "I am in the copy constructor" << std::endl;

    other.check_null();

    numer_ = new int(*other.numer_);
    denom_ = new int(*other.denom_);
}

Fraction::Fraction(Fraction&& other) noexcept {
    std::cout << "I am in the move constructor" << std::endl;

    numer_ = other.numer_;
    denom_ = other.denom_;

    other.numer_ = nullptr;
    other.denom_ = nullptr;
}

Fraction& Fraction::operator=(const Fraction& other) {
    std::cout << "I am in the assignment operator (lvalue)" << std::endl;

    if (this == &other) return *this;

    other.check_null();

    delete numer_;
    delete denom_;

    numer_ = new int(*other.numer_);
    denom_ = new int(*other.denom_);

    return *this;
}

Fraction& Fraction::operator=(Fraction&& other) noexcept {
    std::cout << "I am in the assignment operator (rvalue)" << std::endl;

    if (this == &other) return *this;

    delete numer_;
    delete denom_;

    numer_ = other.numer_;
    denom_ = other.denom_;

    other.numer_ = nullptr;
    other.denom_ = nullptr;

    return *this;
}

Fraction::~Fraction() {
    std::cout << "I am in the destructor" << std::endl;

    delete numer_;
    delete denom_;
}

double Fraction::convertToDecimal() const {
    std::cout << "I am in the convertToDecimal" << std::endl;

    check_null();

    return static_cast<double>(*numer_) / *denom_;
}

Fraction operator+(const Fraction& lhs, const Fraction& rhs) {
    std::cout << "I am in the plus operator" << std::endl;

    lhs.check_null();
    rhs.check_null();

    int n = (*lhs.numer_) * (*rhs.denom_) + (*rhs.numer_) * (*lhs.denom_);
    int d = (*lhs.denom_) * (*rhs.denom_);

    return Fraction(n, d);
}

Fraction operator*(const Fraction& lhs, const Fraction& rhs) {
    std::cout << "I am in the multiplication operator" << std::endl;

    lhs.check_null();
    rhs.check_null();

    int n = (*lhs.numer_) * (*rhs.numer_);
    int d = (*lhs.denom_) * (*rhs.denom_);

    return Fraction(n, d);
}

std::ostream& operator<<(std::ostream& os, const Fraction& f) {
    std::cout << "I am in the << operator" << std::endl;

    f.check_null();

    os << *f.numer_ << "/" << *f.denom_;
    return os;
}