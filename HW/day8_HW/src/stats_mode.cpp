#include <iostream>
#include <string>
#include <set>
#include <cctype>
#include <algorithm>
#include "stats_mode.h"

using std::cout;
using std::endl;
using std::string;

namespace stats_mode {

void run(int argc, char* argv[]) {

    if (argc < 3) {
        cout << "Usage: ./analyzer <mode> [arguments]" << endl;
        return;
    }

    string str = argv[2];

    cout << "Length: " << str.size() << endl;

    std::set<char> unique;
    for (char c : str)
        unique.insert(c);

    cout << "Unique: " << unique.size() << endl;

    bool alnum = true;
    for (size_t i = 0; i < str.size(); i++)
        if (!std::isalnum(str[i]))
            alnum = false;

    cout << "Alphanumeric: " << (alnum ? "Yes" : "No") << endl;

    string reversed = str;
    std::reverse(reversed.begin(), reversed.end());
    cout << "Reversed: " << reversed << endl;

    string noSpaces;
    for (char c : str)
        if (!std::isspace(c))
            noSpaces += c;

    cout << "Without spaces: " << noSpaces << endl;

    string filtered;
    for (char c : str)
        if (std::isalnum(c))
            filtered += std::tolower(c);

    string revFiltered = filtered;
    std::reverse(revFiltered.begin(), revFiltered.end());

    cout << "Palindrome: " << (filtered == revFiltered ? "Yes" : "No") << endl;
    }
}