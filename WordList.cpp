#include "WordList.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <unordered_set>
#include <climits>

WordList::WordList(string fname, EncodingMethod em, CharSet cs)
	: _fname(fname), _num_words(0), _min_length(INT_MAX), _max_length(0)
{
	ifstream infile(_fname);
	unordered_set<char> char_set;
	string line;
	while (getline(infile, line)) {
		_min_length = min(_min_length, (int) line.size());
		_max_length = max(_max_length, (int) line.size());
		_num_words++;
		for (char ch : line) {
			if (ch > 0) {
				char_set.emplace(ch);
			}
		}
	}
	infile.close();

	cout << "File: " << _fname << endl;
	cout << "#Words: " << _num_words << endl;
	cout << "MinLength: " << _min_length << endl;
	cout << "MaxLength: " << _max_length << endl;

	// Build char set
	vector<char> chs;
	if (cs == CharSet::COMPACT) {
		chs.assign(char_set.begin(), char_set.end());
		sort(chs.begin(), chs.end());
		cout << "CharSet: Compact" << endl;
	}
	else if (cs == CharSet::FULL) {
		// 0 reversed for null
		for (int i = 1; i <= SCHAR_MAX; i++) {
			chs.push_back((char) i);
		}
		cout << "CharSet: Full" << endl;
	}
	else {
		cerr << "Invalid argument." << endl;
		exit(-1);
	}

	// Build encoder
	if (em == EncodingMethod::BINARY) {
		_encoding = new BinaryEncoding(_max_length, chs);
	}
	else if (em == EncodingMethod::ONE_HOT) {
		_encoding = new OneHotEncoding(_max_length, chs);
	}
	else {
		cerr << "Invalid argument." << endl;
		exit(-1);
	}
	cout << "Encoding: " << _encoding->name() << endl;

	cout << "CharSetSize: " << chs.size() << endl;
	cout << "MintermLength: " << _encoding->minterm_length() << endl;
}

WordList::~WordList()
{
	delete _encoding;
}

void WordList::build()
{
	int** minterms = new int*[_num_words];
	int minterm_length = _encoding->minterm_length();

	vector<string> words;

	ifstream infile(_fname);
	for (int i = 0; i < _num_words; i++) {
		string line;
		getline(infile, line);
		words.push_back(line);
	}
	infile.close();

	std::sort(words.begin(), words.end());
	for (int i = 0; i < _num_words; i++) {
		minterms[i] = new int[minterm_length + 1];
		minterms[i][0] = 0;
		_encoding->encode(words[i], &minterms[i][1]);

		std::reverse(&minterms[i][1] , &minterms[i][minterm_length + 1]);
	}

//	for (int i = 0; i < _num_words; i++) {
//		for (int j = 1; j <= minterm_length; j++) {
//			cout << minterms[i][j];
//		}
//		cout << endl;
//	}

	int* bounds = new int[minterm_length];
	std::fill_n(bounds, minterm_length, 2);

	MEDDLY::domain* d = MEDDLY::createDomainBottomUp(bounds, minterm_length);

	delete[] bounds;

	MEDDLY::forest* bdd_forest = d->createForest(false, MEDDLY::forest::BOOLEAN,
			MEDDLY::forest::MULTI_TERMINAL, MEDDLY::forest::policies(false));
	MEDDLY::dd_edge bdd(bdd_forest);
	bdd_forest->createEdge(minterms, _num_words, bdd);
	cout << "BDD Nodes: " << bdd.getNodeCount() << endl;

	transform_CBDD(d, bdd);
	transform_ZDD_and_CZDD(d, bdd);
	transform_TaggedBDD(d, bdd);
	transform_ESRBDD(d, bdd);

	// Clean up
	for (int i = 0; i < _num_words; i++) {
		delete[] minterms[i];
	}
	delete[] minterms;
}

void WordList::transform_CBDD(MEDDLY::domain* d, MEDDLY::dd_edge& bdd)
{
	MEDDLY::forest* cbdd_forest = d->createForest(false, MEDDLY::forest::BOOLEAN,
				MEDDLY::forest::CBDD, MEDDLY::forest::policies(false));
	MEDDLY::dd_edge cbdd(cbdd_forest);
	MEDDLY::apply(MEDDLY::COPY, bdd, cbdd);
	cout << "CBDD Nodes: " << cbdd.getNodeCount() << endl;
}

void WordList::transform_ZDD_and_CZDD(MEDDLY::domain* d, MEDDLY::dd_edge& bdd)
{
	MEDDLY::forest::policies p(false);
	p.setZeroSuppressionReduced();
	MEDDLY::forest* zdd_forest = d->createForest(false, MEDDLY::forest::BOOLEAN,
			MEDDLY::forest::MULTI_TERMINAL, p);
	MEDDLY::dd_edge zdd(zdd_forest);
	MEDDLY::apply(MEDDLY::COPY, bdd, zdd);
	cout << "ZDD Nodes: " << zdd.getNodeCount() << endl;

	MEDDLY::forest* czdd_forest = d->createForest(false, MEDDLY::forest::BOOLEAN,
				MEDDLY::forest::CZDD, MEDDLY::forest::policies(false));
	MEDDLY::dd_edge czdd(czdd_forest);
	MEDDLY::apply(MEDDLY::COPY, zdd, czdd);
	cout << "CZDD Nodes: " << czdd.getNodeCount() << endl;
}

void WordList::transform_TaggedBDD(MEDDLY::domain* d, MEDDLY::dd_edge& bdd)
{
	MEDDLY::forest* taggedbdd_forest = d->createForest(false, MEDDLY::forest::BOOLEAN,
				MEDDLY::forest::TAGGED, MEDDLY::forest::policies(false));
	MEDDLY::dd_edge taggedbdd(taggedbdd_forest);
	MEDDLY::apply(MEDDLY::COPY, bdd, taggedbdd);
	cout << "TaggedBDD Nodes: " << taggedbdd.getNodeCount() << endl;
}

void WordList::transform_ESRBDD(MEDDLY::domain* d, MEDDLY::dd_edge& bdd)
{
	MEDDLY::forest* esrbdd_forest = d->createForest(false, MEDDLY::forest::BOOLEAN,
				MEDDLY::forest::ESR, MEDDLY::forest::policies(false));
	MEDDLY::dd_edge esrbdd(esrbdd_forest);
	MEDDLY::apply(MEDDLY::COPY, bdd, esrbdd);
	cout << "ESRBDD Nodes: " << esrbdd.getNodeCount() << endl;

	// Count the reduction rules
    long* counts = new long[5];
    MEDDLY::node_handle node = esrbdd.getNode();
    dynamic_cast<MEDDLY::expert_forest*>(esrbdd_forest)->countEdgeLabels(&node, 1, counts);
    long ev = -1;
    esrbdd.getEdgeValue(ev);
    counts[ev]++;

    std::cout << "BLANK: " << counts[0] << std::endl;
    std::cout << "FULL: " << counts[1] << std::endl;
    std::cout << "ZERO: " << counts[2] << std::endl;
    std::cout << "ONE: " << counts[3] << std::endl;
    std::cout << "FULL(FALSE): " << counts[4] << std::endl;

    delete[] counts;
}
