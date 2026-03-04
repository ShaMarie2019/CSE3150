#include <iostream>
#include <string>
#include <cctype>
#include "text_mode.h"

using std::cout;
using std::endl;
using std::string;

namespace text_mode {
	void run(int argc, char* argv[]) {
		if (argc < 3) {
			cout << "Usage: ./analyzer <mode> [arguments]" << endl;
			return;
		}
		
		string var;
		var = argv[2];
		
		cout << "Original: " << var << endl;
    	cout << "Length: " << var.size() << endl;
				
		int letter = 0, digits = 0, spaces = 0, punc = 0;
		for (char c : var) {
			if (std::isalpha(c)) letter++;
			if (std::isdigit(c)) digits++;
			if (std::isspace(c)) spaces++;
			if (std::ispunct(c)) punc++;
		}

		cout << "Letters: " << letter << endl;
		cout << "Digits: " << digits << endl;
		cout << "Spaces: " << spaces << endl;
		cout << "Punctuation: " << punc << endl;

		string upper = var;

		for (size_t i =0; i < upper.size(); i++) {
			upper[i] = std::toupper(upper[i]);
		}
		
		cout << "Uppercase: " << upper << endl;

		for (size_t i =0; i < var.size(); i++) {
			var[i] = std::tolower(var[i]);
		}
		
		cout << "Lowercase: " << var << endl;

		if (var.find("test") != string::npos) {
			cout << "Contains test: Yes" << endl;
    	} else {
        	cout << "Contains test: No" << endl;
		}

		if (var.find(' ') != string::npos) {
			size_t pos = var.find(' ');
			string first_word = var.substr(0, pos);
			cout << "First word: " << first_word << endl;
    	}
	}
}
