#include "fraction.h"
#include <iostream>
#include <utility>

int main() {
    try {
        int n1, d1, n2, d2, n3, d3;
        std::cin >> n1 >> d1 >> n2 >> d2 >> n3 >> d3;

        if (std::cin.fail()) {
            throw std::runtime_error("Invalid input");
        }

        // Creating fraction with default constructor
        Fraction f1;

        // Creating fraction with custom constructor
        Fraction f2(n1, d1);

        // Creating fraction with copy constructor
        Fraction f3(f2);

        // Demonstrating move constructor
        Fraction temp1(n2, d2);
        Fraction f4(std::move(temp1));

        // Demonstrating assignment operator with lvalue
        Fraction f5;
        f5 = f2;

        // Demonstrating assignment operator with rvalue
        Fraction f6;
        Fraction temp2(n3, d3);
        f6 = std::move(temp2);

        // Demonstrating addition operator
        Fraction sum = f2 + f3;

        // Demonstrating multiplication operator
        Fraction product = f2 * f4;

        // Demonstrating << operator
        std::cout << "f2 = " << f2 << std::endl;
        std::cout << "f3 = " << f3 << std::endl;
        std::cout << "sum = " << sum << std::endl;
        std::cout << "product = " << product << std::endl;

        // Demonstrating convertToDecimal from abstract class
        Decimal* d = &f2;
        std::cout << "f2 as decimal: " << d->convertToDecimal() << std::endl;

        // Destructors will be called when program ends
    } catch (const std::runtime_error& e) {
        std::cout << "Caught a runtime error!" << std::endl;
    }

    return 0;
}
