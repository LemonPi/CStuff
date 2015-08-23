#pragma once
#include <iostream>
#include <climits>
#include <bitset>
#include <unordered_map>
#include <unordered_set>
#include <cassert>
#include <vector>
#include <iterator>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include "common.h"

enum class TTT : int {
	EMPTY = 0,
	X = 1, // 01
	O = 2  // 10
};
std::istream& operator>>(std::istream& is, TTT& piece) {
	char piece_char;
	is >> piece_char;
	if (piece_char == 'X' || piece_char == 'x') piece = TTT::X;
	else if (piece_char == 'O' || piece_char == 'o') piece = TTT::O;
	else piece = TTT::EMPTY;
	return is;
}
std::ostream& operator<<(std::ostream& os, const TTT& piece) {
	if (piece == TTT::X) return os << 'X';
	if (piece == TTT::O) return os << 'O';
	else return os << ' ';
}

class Tic_tac_toe {
	int board;	// 18 bits used to represent 9 squares 2 bits wide
	/*
			col 0   1   2
		      -------------
		row 0 | 1 | 2 | 3 |
		      -------------
		    1 | 4 | 5 | 6 |
		      -------------
		    2 | 7 | 8 | 9 |
		      -------------

		2D blocks mapping into 2-bit blocks in board, with block 1 being (0,0)
		| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 |
	*/
	// constant masks that lead to victory
	static constexpr int full_row = 0x15;			// 00 00 00 00 00 00 01 01 01, blocks 1 2 3
	static constexpr int full_col = 0x1041;			// 00 00 01 00 00 01 00 00 01, blocks 1 4 7
	static constexpr int top_left_diag = 0x10101;	// 01 00 00 00 01 00 00 00 01, blocks 1 5 9
	static constexpr int bot_left_diag = 0x1110;	// 00 00 01 00 01 00 01 00 00, blocks 3 5 7

	bool got_full_row(int shifted_board, size_t r) const {
		return (shifted_board & (full_row << (r*6))) == (full_row << (r*6));
	}
	bool got_full_col(int shifted_board, size_t c) const {
		return (shifted_board & (full_col << (c*2))) == (full_col << (c*2));
	}
	bool got_full_diag(int shifted_board) const {
		return ((shifted_board & top_left_diag) == top_left_diag) || ((shifted_board & bot_left_diag) == bot_left_diag);
	}

public:
	Tic_tac_toe() : board{0} {}	// all empty
	void place(TTT piece, size_t row, size_t col) {
		// how much to shift depends on every row (+6) and col (+2)
		size_t block {row*6 + col*2};
		cout << board << endl;
		cout << "placing " << piece << " in block " << block/2 + 1 << endl;
		// no checking that block is empty, can do that optionally
		board |= static_cast<int>(piece) << block;
		cout << board << endl;
	}
	TTT get_piece(size_t row, size_t col) const {
		size_t block {row*6 + col*2};
		return static_cast<TTT>((board >> block) & 3);	// 3 == 11
	}
	TTT check_win() const {
		// returns piece that won or TTT::EMPTY if no one won
		for (size_t r = 0; r < 3; ++r)
			if (got_full_row(board, r)) return TTT::X;
			else if (got_full_row(board << 1, r)) return TTT::O;
		for (size_t c = 0; c < 3; ++c)
			if (got_full_col(board, c)) return TTT::X;
			else if (got_full_col(board << 1, c)) return TTT::O;

		if (got_full_diag(board)) return TTT::X;
		if (got_full_diag(board << 1)) return TTT::O;

		return TTT::EMPTY;
	}

	void print() const {
		for (size_t r = 0; r < 3; ++r) {
			cout << "-------------\n|";
			for (size_t c = 0; c < 3; ++c) {
				cout << ' ' << get_piece(r, c) << " |";
			}
			cout << endl;
		}
		cout << "-------------\n";
	}
};

size_t factorial_trailing_zeros(size_t n) {
	// for n factorial, how many trailing 0s by counting factors that are multiples of 5
	size_t count {0};
	for (size_t i = 5; n / i > 0; i *= 5)
		count += n / i;
	return count;
}

template <typename T>
T max_nocomp(T a, T b) {
	T diff {b - a};
	// check sign bit - negative difference indicates b < a
	bool b_smaller_than_a {static_cast<bool>(diff >> (sizeof(T)*CHAR_BIT - 1))};
	// if b is bigger than a, then it would return b, otherwise subtract the negative difference
	return b - b_smaller_than_a * diff;
}


template <typename Iter>
typename std::iterator_traits<Iter>::value_type greatest_continuous_seq_sum(Iter begin, Iter end) {
	using T = typename std::iterator_traits<Iter>::value_type;
	T greatest_sum = 0, current_sum = 0;
	for (auto cur = begin; cur != end; ++cur) {
		// in between sequences
		if (current_sum == 0) {
			// start of new sequence
			if (*cur > 0) current_sum = *cur;
		}
		else {
			current_sum += *cur;
			// end of sequence, reset to 0
			if (current_sum < 0) current_sum = 0;
			else if (current_sum > greatest_sum) greatest_sum = current_sum;
		}
	}
	return greatest_sum;
}

template <typename Iter, typename T>
std::vector<std::pair<T,T>> pairs_summing_to_target(Iter copy_begin, Iter copy_end, const T& target) {
	// in the sequence [begin, end), find all pairs that sum up to target]
	std::vector<T> sorted {copy_begin, copy_end};
	std::vector<std::pair<T,T>> pairs;
	// use rdx_sort if O(n) time performance is required, otherwise O(nlgn)
	std::sort(begin(sorted), end(sorted));
	// eat at the array from both ends
	auto lower = begin(sorted);
	// 1 beyond where it's possible for the pair to add up to target; anything above is larger than target - lowest value
	auto upper = std::upper_bound(begin(sorted), end(sorted), target - *lower);

	while (lower < upper) {
		auto sum = *lower + *upper;
		if (sum == target) {
			// load all copies of upper value and lower value
			auto upval = *upper;
			auto lowval = *lower;
			do {
				pairs.emplace_back(lowval, upval);
				--upper;
			} while (lower < upper && upval == *upper);
			// already counted the original copy, so copy 1 less
			while (lower < upper && lowval == *(++lower)) {
				pairs.emplace_back(lowval, upval);
				++lower;
			}
		}
		else if (sum > target) --upper;
		else ++lower;
	}
	return pairs;
}
template <typename Iter, typename T>
std::vector<std::pair<T,T>> pairs_summing_to_target_alt(Iter begin, Iter end, const T& target) {
	std::unordered_set<T> passed_elems;
	std::vector<std::pair<T,T>> pairs;
	while (begin < end) {
		if (passed_elems.find(target - *begin) != passed_elems.end()) 
			pairs.emplace_back(*begin, target - *begin);
		passed_elems.insert(*begin);
		++begin;
	}
	return pairs;
}

template <typename Iter>
void shuffle(Iter begin, Iter end) {
	while (begin != end) {
		auto rand_remaining_item = std::rand() % std::distance(begin, end);

		std::swap(begin[0], begin[rand_remaining_item]);
		++begin;
	}
}

int twos_under_num(int n) {
	int place = 10;
	std::vector<int> twos_under_place {1};	// 1 two under 10, index will be log(place) - 1, so index 0 corresponds to 10^(0+1) = 10
	for (; place*10 <= n; place *= 10)
		// 9 instances of the previous place + leading 2s place
		twos_under_place.push_back(twos_under_place.back() * 9 + place);

	int total_twos = 0;
	for (int p = twos_under_place.size() - 1; p >= 0; --p) {
		int place {static_cast<int>(std::pow(10, p + 1))};
		total_twos += n / place * twos_under_place[p];
		n -= (n / place) * place;	// remove leading digit
	}
	// reduced n to below 10
	if (n >= 2) ++total_twos;
	return total_twos;
}
void test_tic_tac_toe() {
	cout << "Enter something to start game, specify piece (X or O) row (0-2) and col (0-2)\n";
	GETINPUT();
	Tic_tac_toe game {};
	TTT piece;
	size_t r, c;
	while (true) {
		ss >> piece >> r >> c;
		game.place(piece, r, c);
		game.print();
		if (game.check_win() != TTT::EMPTY) break;
		GETANOTHER();
	}

	cout << "the winner is " << game.check_win() << endl;
}


void test_factorial_trailing_zeroes() {
	GETINPUT();
	size_t n;
	ss >> n;
	cout << factorial_trailing_zeros(n) << endl;
}

void test_max_nocomp() {
	GETINPUT();
	int a, b;
	ss >> a >> b;
	cout << max_nocomp(a, b) << endl;
}

void test_unordered_map_correctness() {
	std::unordered_map<int, int> nodes_to_pins;
	std::vector<int> sinks {10354, 12010, 12262, 11206};
	nodes_to_pins.reserve(sinks.size());
	for (size_t s = 0; s < sinks.size(); ++s)
		nodes_to_pins[sinks[s]] = s + 1;

	cout << "hash table maps: ";
	for (auto pair : nodes_to_pins) cout << pair.first << "->" << pair.second << "  ";
	cout << endl;

	for (size_t s = 0; s < sinks.size(); ++s) {
		auto mapping = nodes_to_pins.find(sinks[s]);
		if (mapping == nodes_to_pins.end()) {
			cout << "did not find mapping for node " << sinks[s] << " the " << s << "th sink\n";
			assert(false);
		}
		sinks[s] = mapping->second;
	}
}

void test_greatest_continuous_seq_sum() {
	GETINPUT();
	int a;
	std::vector<int> seq;
	while (ss >> a) seq.push_back(a);
	cout << greatest_continuous_seq_sum(begin(seq), end(seq)) << endl;
}

void test_pairs_summing_to_target() {
	GETINPUT();
	int target;
	ss >> target;
	std::vector<int> vals {1,3,4,2,3,-5,4,7,6,8,11,20,13,-2,-1,0};
	auto pairs = pairs_summing_to_target(begin(vals), end(vals), target);
	for (const auto& pair : pairs) 
		cout << pair.first << " + " << pair.second << " = " << target << endl;
	cout << endl;
	auto alt_pairs = pairs_summing_to_target_alt(begin(vals), end(vals), target);
	for (const auto& pair : alt_pairs) 
		cout << pair.first << " + " << pair.second << " = " << target << endl;	
}

void test_shuffle() {
	GETINPUT();
	int elem;
	std::vector<int> elems;
	while (ss >> elem) elems.push_back(elem);
	shuffle(begin(elems), end(elems));
	for (auto e : elems) cout << e << ' '; cout << endl;
}

void test_twos_under_num() {
	GETINPUT();
	int num;
	ss >> num;
	cout << twos_under_num(num) << endl;
}

void test_misc() {
	// test_tic_tac_toe();
	// test_factorial_trailing_zeroes();
	// test_max_nocomp();
	// test_unordered_map_correctness();
	// test_greatest_continuous_seq_sum();
	// test_pairs_summing_to_target();
	// test_shuffle();
	test_twos_under_num();
}