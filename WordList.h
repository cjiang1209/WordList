#ifndef WORDLIST_H_
#define WORDLIST_H_

#include <string>
#include <meddly.h>
#include <meddly_expert.h>

#include "Encoding.h"

using namespace std;

class WordList {
private:
	string _fname;
	int _num_words;
	int _min_length;
	int _max_length;

	Encoding* _encoding;

public:
	WordList(string fname, EncodingMethod em = EncodingMethod::BINARY, CharSet cs = CharSet::FULL);
	~WordList();

	void build();
	void transform_CBDD(MEDDLY::domain* d, MEDDLY::dd_edge& bdd);
	void transform_ZDD_and_CZDD(MEDDLY::domain* d, MEDDLY::dd_edge& bdd);
	void transform_TaggedBDD(MEDDLY::domain* d, MEDDLY::dd_edge& bdd);
	void transform_ESRBDD(MEDDLY::domain* d, MEDDLY::dd_edge& bdd);
};

#endif
