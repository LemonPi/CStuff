#pragma once
#include <iostream>
#include <bitset>
#include <string>
#include <climits>	// CHAR_BIT
#include <sstream>
#include <cstdlib>
#include <iomanip>
#include <stdexcept>
#include "common.h"


using bit_rep = unsigned long long;
// substitute M in bits i (low) to j (high) inclusively of N
using b32 = uint32_t;
b32 substitute_bits(b32 N, b32 M, size_t i, size_t j) {
	// number of bits to be substituted
	size_t M_width {j - i + 1};
	// mask to extract bits outside of i to j of N
	// 00000...11111 (1s = M_width)
	b32 extract_mask = ~(~0 << M_width);
	// 000..11111000
	extract_mask <<= i;
	// 111..00000111
	extract_mask = ~extract_mask;
	// now bits i to j are 0
	N &= extract_mask;

	// clear bits of M ahead of its width
	M &= (1 << M_width) - 1;

	return N | (M << i);
}

std::string dec_to_bin(const std::string& num) {
	int decimal_index {-1};
	for (size_t i = 0; i < num.size(); ++i) {
		if (num[i] == '.') {
			decimal_index = i;
			break;
		}
	}
	// assume decimal found
	if (decimal_index == -1) decimal_index = num.size();
	std::stringstream converter {num.substr(0, decimal_index)};
	int int_part;
	converter >> int_part;
	// using bitset takes care of integer sign 
	std::bitset<32> int_bits {(bit_rep)int_part};
	// no decimal part following
	if (decimal_index == (int)num.size()) return int_bits.to_string();

	// clear converter
	converter.clear();
	converter.str(num.substr(decimal_index));	// including decimal
	double dec_part;
	converter >> dec_part;
	std::string dec_bits(32, '\0');	// don't print trailing 0s
	// give 32 bits to the decimal
	for (size_t i = 0; i < 32; ++i) {
		if (dec_part == 0) break;
		// fractional shift x*2^-1 to x*2^0
		// if after shifting > 1, then x must be 1 else 0
		// note that multiplying by 2 and subtracting 1 do not introduce rounding error
		dec_part *= 2;
		if (dec_part >= 1) {
			dec_bits[i] = '1';
			dec_part -= 1;
		}
		else
			dec_bits[i] = '0';
	}
	if (dec_part != 0) return "ERROR";
	return int_bits.to_string() + '.' + dec_bits;
}


template <typename T>
T set_bit(const T& num, size_t n) {
	return num | (1 << n);
}

template <typename T>
T clear_bit(const T& num, size_t n) {
	return num & ~(1 << n);
}

constexpr int none = -1;
template <typename T>
std::pair<T,T> neighbour_values(const T& num) {
	// next smallest and largest numbers with the same number of set bits
	// -1 indicate none have been found
	int first_0 = none, first_1 = none, first_10 = none, first_01 = none;
	int num_1 = 0, num_0 = 0;
	for (int i = 0; i < (int) sizeof(num)*CHAR_BIT; ++i) {
		if ((num >> i) & 1) {
			if (first_1 == none) first_1 = i;
			if (first_0 != none && first_10 == none) first_10 = i;
			if (first_01 == none) ++num_1;
		}
		else {
			if (first_0 == none) first_0 = i;
			if (first_1 != none && first_01 == none) first_01 = i;
			if (first_10 == none) ++num_0;
		}
		if (first_10 != none && first_01 != none) break;
	}
	std::cout << first_0 << ' ' << first_1 << ' ' << first_10 << ' ' << first_01 << ' ' << num_0 << ' ' << num_1 << '\n';

	T next_largest {set_bit(num, first_01)};
	// float remaining 1s down
	next_largest &= ~0 << first_01;			// clear lower bits
	next_largest |= (1 << (num_1 - 1)) - 1;	// 00...0011 mask

	T next_smallest {clear_bit(num, first_10)};
	// float remaining 0s down
	next_smallest |= (1 << first_10) - 1;	// set lower bits
	next_smallest &= ~0 << (num_0 - 1);		// 11...1100 mask

	return {next_smallest, next_largest};
}

int bitops_to_convert(int a, int b) {
	std::bitset<sizeof(int) * CHAR_BIT> difference {(bit_rep)(a ^ b)};
	return difference.count();
}

// assuming 32 bit int
constexpr int odd_mask 	= 0xAAAA;	// 1010 1010 1010 1010
constexpr int even_mask = 0x5555;	// 0101 0101 0101 0101
int swap_parity_bits(int num) {
	return ((num & odd_mask) >> 1) | ((num & even_mask) << 1);
}

template <typename bigint>	// big enough that only 1 bit can be accessed at a time
bigint find_missing(std::vector<bigint> nums) { 	// pass by copy as it will be locally used
	// use elimination with each bit filtering out half of the candidates for O(n) time
	bigint missing {0};
	for (size_t bit = 0; bit < sizeof(bigint)*CHAR_BIT; ++bit) {
		for (auto num : nums) std::cout << num << ' ';
		std::cout << '\n';
		if (nums.size() <= 1) break;
		// relative to the current bit
		std::vector<bigint> evens, odds;
		for (const bigint& num : nums) {
			if (num & (1 << bit)) odds.push_back(num);
			else evens.push_back(num);
		}
		// assuming nums is 0..n, #0s >= #1s since starting with 0
		// if 0s > 1s then removing a even number -> 0s == 1s, else 0s < 1s
		if (evens.size() <= odds.size()) {
			nums = std::move(evens);
		}
		else {
			nums = std::move(odds);
			missing |= 1 << bit;
		}
	}
	
	// if (nums.size() != 1) throw std::runtime_error("More than 1 missing from 0 to n");
	return missing;
}


// only for power of 2 bases
template <typename T>
T bit_round_down(const T& val, size_t base) {
	// all higher bits are set and lower bits are cleared
	return val & ~(base - 1);
}

// assuming alignment is a power of 2
void* aligned_malloc(size_t size, size_t alignment) {
	// % alignment can be [0,alignment-1] from the allocated location
	// also need to a pointer space to store the actual allocated pointer
	auto buffer_bytes = (alignment-1) + sizeof(void*);	
	void* actual_alloc {malloc(size + buffer_bytes)};
	if (!actual_alloc) return nullptr;
	// round up to the nearest alignment
	// adding buffer_bytes rounding down will be behind actual_alloc and have enough space for actual_alloc
	void** aligned_alloc {reinterpret_cast<void**>(bit_round_down(reinterpret_cast<size_t>(actual_alloc) + buffer_bytes, alignment))};
	aligned_alloc[-1] = actual_alloc;
	return aligned_alloc;
}

void aligned_free(void* aligned_alloc) {
	free(static_cast<void**>(aligned_alloc)[-1]);
}

template <typename T>
class Multiarray {
	T* elems;
	size_t col_size;
public:
	Multiarray() : elems{nullptr} {}
	Multiarray(size_t r, size_t c) : elems{new T[r*c]}, col_size{c} {}
	~Multiarray() {if (elems) delete[] elems;}

	T* operator[](size_t row) {return elems + row*col_size;}
};

template <typename T>
Multiarray<T> my2DAlloc(size_t r, size_t c) {
	return Multiarray<T>{r, c};
}

void test_dec_to_bin() {
	std::string line;
	std::getline(std::cin, line);
	auto str_bits = dec_to_bin(line);
	std::cout << str_bits << std::endl;
}

void test_neighbour_values() {
	GETINPUT();
	int num; ss >> num;
	auto neighbours = neighbour_values(num);
	std::bitset<32> original {(bit_rep)num};
	std::bitset<32> s {(bit_rep)neighbours.first};
	std::bitset<32> l {(bit_rep)neighbours.second};
	std::cout << original << " (" << num << ")\n";
	std::cout << s << " (" << neighbours.first << ")\n";
	std::cout << l << " (" << neighbours.second << ")\n";
	if (__builtin_popcount(num) != __builtin_popcount(neighbours.first) ||
		__builtin_popcount(num) != __builtin_popcount(neighbours.second))
		std::cout << "Neightbour values ... FAILED\n";
}

void test_bitops_to_convert() {
	GETINPUT();
	int a, b;
	ss >> a >> b;
	std::cout << bitops_to_convert(a, b) << std::endl;
}

void test_swap_parity_bits() {
	GETINPUT();
	int num;
	ss >> num;
	std::cout << std::bitset<32> {(bit_rep)num} << std::endl;
	std::cout << std::bitset<32> {(bit_rep)swap_parity_bits(num)} << std::endl;
}

void test_find_missing() {
	std::vector<int> nums;
	GETINPUT();
	int num;
	while (ss >> num) nums.push_back(num);
	int missing {find_missing(nums)};
	std::cout << missing << std::endl;
}

void test_aligned_malloc() {
	GETINPUT();
	size_t alignment;
	ss >> alignment;

	void* test = aligned_malloc(100, alignment);
	cout << std::hex << alignment << ": " << test << endl;

	aligned_free(test);
}

void test_multiarray() {
	GETINPUT();
	size_t row, col;
	ss >> row >> col;
	auto twod = my2DAlloc<int>(row, col);
	// populate
	for (size_t r = 0; r < row; ++r) 
		for (size_t c = 0; c < col; ++c) 
			twod[r][c] = r * c;

	// read
	for (size_t r = 0; r < row; ++r) {
		for (size_t c = 0; c < col; ++c)
			cout << std::setw(4) << twod[r][c] << ' ';
		cout << endl;
	}
}

void test_bits() {
	// test_dec_to_bin();
	// test_neighbour_values();
	// test_bitops_to_convert();
	// test_swap_parity_bits();
	// test_find_missing();	
	// test_aligned_malloc();
	test_multiarray();
}