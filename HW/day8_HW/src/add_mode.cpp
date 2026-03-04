#include <iostream>
#include <string>
#include "add_mode.h"

using std::cout;
using std::endl;
using std::string;

namespace add_mode {
    int add(int a, int b) {
        return a + b;
    }

    double add(double a, double b) {
        return a + b;
    }

    string add(string a, string b) {
        return a + b;
    }
    
    void run(int argc, char* argv[]) {
        if (argc < 4) {
            cout << "Usage: ./analyzer <mode> [arguments]" << endl;
            return;
        }

        string arg1 = argv[2];
        string arg2 = argv[3];

        bool hasDec = (arg1.find('.') != string::npos || arg2.find('.') != string::npos);
        bool digit = true;

        for (char c : arg1) {
            if (!std::isdigit(c)) digit = false;
        }

        for (char c : arg2) {
            if (!std::isdigit(c)) digit = false;
        }

        if (digit && !hasDec) {
            int a = std::stoi(arg1);
            int b = std::stoi(arg2);
            std::cout << "Result: " << add(a, b) << std::endl;
        } else if (hasDec) {
            double a = std::stod(arg1);
            double b = std::stod(arg2);
            std::cout << "Result: " << std::to_string(add(a, b)) << std::endl;
        } else {
            std::cout << "Result: " << add(arg1, arg2) << std::endl;
        }
    }
}