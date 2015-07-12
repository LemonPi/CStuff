#include <iostream>
#include <bitset>
#include <string>
#include <climits>	// CHAR_BIT
#include <sstream>

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
	std::bitset<32> int_bits {int_part};
	// no decimal part following
	if (decimal_index == num.size()) return int_bits.to_string();

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

void test_dec_to_bin() {
	std::string line;
	std::getline(std::cin, line);
	auto str_bits = dec_to_bin(line);
	std::cout << str_bits << std::endl;
}

void test_neighbour_values() {
	std::string line;
	std::getline(std::cin, line);
	std::stringstream ss {line};
	int num; ss >> num;
	auto neighbours = neighbour_values(num);
	std::bitset<32> original {num};
	std::bitset<32> s {neighbours.first};
	std::bitset<32> l {neighbours.second};
	std::cout << original << " (" << num << ")\n";
	std::cout << s << " (" << neighbours.first << ")\n";
	std::cout << l << " (" << neighbours.second << ")\n";
	if (__builtin_popcount(num) != __builtin_popcount(neighbours.first) ||
		__builtin_popcount(num) != __builtin_popcount(neighbours.second))
		std::cout << "Neightbour values ... FAILED\n";
}