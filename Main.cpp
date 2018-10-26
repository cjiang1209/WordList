#include <iostream>
#include <random>
#include <algorithm>

#include "WordList.h"

int main(int argc, char* argv[]) {
    if (argc != 4){
    	return 1;
    }

    MEDDLY::initialize();

    WordList* wl;
    if (strcmp(argv[2], "binary") == 0) {
    	if (strcmp(argv[3], "full") == 0) {
    		wl = new WordList(argv[1], EncodingMethod::BINARY, CharSet::FULL);
    	}
    	else if (strcmp(argv[3], "compact") == 0) {
    		wl = new WordList(argv[1], EncodingMethod::BINARY, CharSet::COMPACT);
    	}
    	else {
    		return 1;
    	}
    }
    else if (strcmp(argv[2], "one-hot") == 0) {
		if (strcmp(argv[3], "full") == 0) {
			wl = new WordList(argv[1], EncodingMethod::ONE_HOT, CharSet::FULL);
		}
		else if (strcmp(argv[3], "compact") == 0) {
			wl = new WordList(argv[1], EncodingMethod::ONE_HOT, CharSet::COMPACT);
		}
		else {
			return 1;
		}
    }
    else {
    	return 1;
    }

    wl->build();

    delete wl;
    MEDDLY::cleanup();

    return 0;
}
