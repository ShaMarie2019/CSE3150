/* Define a namespace named do_while_count_utils and a function named runDoWhileCount().
	The function must:
	1. Declare an integer variable.
	2. Use a do-while loop to print:
		Enter a number between 1 and 5:
	3. Continue looping until the value is between 1 and 5 inclusive.
	4. Declare a fixed array with values 1, 2, 3, 4, 5.
	5. Use a range-based for loop to print:
		Value: <number>
	6. Stop printing once the printed value equals the user input.

*/

#include <iostream>
#include "do_while_count_utils.h"
using std::cout, std::cin, std::endl;

namespace do_while_count_utils {
        void runDoWhileCount() {
		int num;
		do {
			cout << "Enter a number between 1 and 5: ";
			cin >> num;	
		} while (num < 1 || num > 5 );

		int set_arr[5] = {1,2,3,4,5};
		for (int i = 1; i <= num; i++) {
			cout << "Value: " << i << endl;
		}
	}
}

