#include <iostream>
#include <string>
#include "string_utils.h"
using std::cin, std::cout, std::endl;

namespace string_utils {
	int runStringOption() {
		int str_len;
		cout << "Enter string length:";
		cin >> str_len;
		if (str_len >= 20) {
			cout << "Error:" << str_len  << endl;
			return 1;
		} 
		cin.ignore();
		std::string user_str;
		cout << "Enter string:";
		std::getline(cin, user_str);
		char arr_str[20];
		
		for (size_t i = 0; i < str_len; i++) {
			arr_str[i] = user_str[i];
		}
		arr_str[str_len] = '\0';
		cout << "C-style string: " << arr_str << endl;
		return 0;
	}
}
		



