#include <iostream>
#include <string>
#include "text_mode.h"
#include "add_mode.h"
#include "stats_mode.h"

using std::cout;
using std::endl;
using std::string;


int main(int argc, char* argv[]) {
	if (argc < 2) {
		cout << "Usage: ./analyzer <mode> [arguments]" << endl;
		return 1;
	}

	string r = argv[1];
	int r_id = 0;

	if (r == "text") r_id = 1;
	if (r == "add") r_id = 2;
	if (r == "stats") r_id = 3;

	switch(r_id) {
		case 1: // text
			text_mode::run(argc, argv);
			break;	
		case 2: // add 
			add_mode::run(argc, argv);
			break;
		case 3: // stats
			stats_mode::run(argc, argv);
			break;
		default:
			cout << "Invalid mode" << endl;
			return 1;	
	}
	return 0;
}


