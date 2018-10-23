/*
 * Encoding.cpp
 *
 *  Created on: Oct 22, 2018
 *      Author: cjiang
 */

#include <cmath>
#include <cassert>
#include <iostream>

#include "Encoding.h"

Encoding::Encoding(int length, const vector<char>& chs)
	: _length(length)
{
	// 0 reversed for null
	_char_map.emplace(char(0), 0);
	int i = 1;
	for (char ch : chs) {
		if (_char_map.find(ch) == _char_map.end()) {
			_char_map.emplace(ch, i);
			i++;
		}
	}

	cout << "CharMapSize: " << _char_map.size() << endl;
}

BinaryEncoding::BinaryEncoding(int length, const vector<char>& chs)
	: Encoding(length, chs)
{
	_bits_per_char = (int) ceil(log2(_char_map.size()));
}

int BinaryEncoding::minterm_length() const
{
	return _bits_per_char * _length;
}

void BinaryEncoding::encode(string word, int* arr) const
{
	int i = 0;
	for (char ch : word) {
		const auto search = _char_map.find(ch);
		assert(search != _char_map.end());

		int value = search->second;
		for (int j = 0; j < _bits_per_char; j++) {
			arr[i] = (value & 1);
			value >>= 1;
			i++;
		}
	}
	int length = minterm_length();
	for (; i < length; i++) {
		arr[i] = 0;
	}
}

string BinaryEncoding::name() const
{
	return "Binary";
}

OneHotEncoding::OneHotEncoding(int length, const vector<char>& chs)
	: Encoding(length, chs)
{

}

int OneHotEncoding::minterm_length() const
{
	return _length * _char_map.size();
}

void OneHotEncoding::encode(string word, int* arr) const
{
	for (char ch : word) {
		std::fill_n(arr, _char_map.size(), 0);

		const auto search = _char_map.find(ch);
		assert(search != _char_map.end());
		int value = search->second;
		arr[value] = 1;
		arr += _char_map.size();
	}

	const auto null_search = _char_map.find(0);
	assert(null_search != _char_map.end());
	for (int i = word.size(); i < _length; i++) {
		std::fill_n(arr, _char_map.size(), 0);
		arr[null_search->second] = 1;
		arr += _char_map.size();
	}
}

string OneHotEncoding::name() const
{
	return "One Hot";
}
