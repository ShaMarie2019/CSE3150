#include <iostream>
#include <cctype>
#include "grade_utils.h"
using std::cout, std::cin, std::endl;

namespace grade_utils {
        double calculateGrade() {
		cout << "Enter student type (U for undergrad, G for grad): " << endl;
		char s;
		cin >> s;
		s = std::toupper(s);
		cout << "Enter numeric grade: " << endl;
		double grade;
		cin >> grade;
		if (0 > grade || grade  > 100) {
			cout << "Invalid grade" << endl;
			return 1;
		}

		if (s == 'U' && grade >= 60 || s == 'G' && grade >= 70) cout << "Status: Pass" << endl;
		else cout << "Status: Fail" << endl;
		return grade;
	}
}

