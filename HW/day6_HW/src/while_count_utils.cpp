#include <iostream>
#include "while_count_utils.h"
using std::cout, std::cin, std::endl;

namespace while_count_utils {
	void runWhileCount() {
		int num;
		do {
			cout << "Enter a number to count to:" << endl;
			cin >> num;

			if (num > 10){
				cout << "I'm programmed to only count up to 10!" << endl;
			}
		} while (num > 10);
		int i = 1;
		while (i <= num) {
			if (i == 5){ 
				i++;
				continue;
			}
			cout << i << endl;
                        i++;
		}
	}
}	 
