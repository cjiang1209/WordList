#include <iostream>
#include <random>
#include <algorithm>

#include "WordList.h"

int main(int argc, char* argv[]) {
    if (argc != 2){
    	return 1;
    }

    MEDDLY::initialize();

//    WordList wl(argv[1], EncodingMethod::BINARY, CharSet::FULL);
//    WordList wl(argv[1], EncodingMethod::BINARY, CharSet::COMPACT);
    WordList wl(argv[1], EncodingMethod::ONE_HOT, CharSet::FULL);
//    WordList wl(argv[1], EncodingMethod::ONE_HOT, CharSet::COMPACT);
    wl.build();

    MEDDLY::cleanup();

    return 0;
}
