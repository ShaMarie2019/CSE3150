#include <iostream>
using std::cout, std::cin, std::endl;
#include "integer_utils.h"

namespace integer_utils {
	void runIntegerOption() {
		int x, y;
		cout << "Enter first integer: ";
		cin >> x;
		cout << "Enter second integer: ";
		cin >> y;
		if (y == 0) {
			cout << "Error: division by zero" << endl;
			return;
		} else {
			int result = x / y;
			cout << "Result: " << result << endl;
			cout << "After post-increment: " << x++ << endl;
			cout << "After pre-increment: " << ++x << endl; 
		}
	}
	
}
