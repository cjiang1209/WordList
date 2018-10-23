#ifndef ENCODING_H_
#define ENCODING_H_

#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

enum EncodingMethod {
	BINARY,
	ONE_HOT
};

enum CharSet {
	FULL,
	COMPACT
};

class Encoding
{
protected:
	int _length;
	unordered_map<char, int> _char_map;

public:
	Encoding(int length, const vector<char>& chs);

	virtual int minterm_length() const = 0;
	virtual void encode(string word, int* arr) const = 0;

	virtual string name() const = 0;
};

class BinaryEncoding : public Encoding
{
protected:
	int _bits_per_char;

public:
	BinaryEncoding(int length, const vector<char>& chs);

	virtual int minterm_length() const override;
	virtual void encode(string word, int* arr) const override;

	virtual string name() const override;
};

class OneHotEncoding : public Encoding
{
public:
	OneHotEncoding(int length, const vector<char>& chs);

	virtual int minterm_length() const override;
	virtual void encode(string word, int* arr) const override;

	virtual string name() const override;
};

#endif
