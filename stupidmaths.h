#include "common.h"
#include <vector>
#include <iostream>
#include <cmath>
#include <iterator>
#include <unordered_map>

using std::vector;

template <typename T>
vector<T> prime_factorize(T num) {
	vector<T> factors;
	if (num < 4) return {num};
	// optimization for even factors so traversal can go by 2
	while ((num & 1) == 0) {
		factors.push_back(2);
		num >>= 1;
	}
	T d {3};
	while (num > 1) {
		while (num % d == 0) {
			factors.push_back(d);
			num /= d;
		}
		d += 2;
		if (d * d > num && num > 1) {
			// reduced num to its largest prime factor
			factors.push_back(num);
			return factors;
		}
	}
	return factors;
}

template <typename Iter>
size_t count_consecutive(Iter begin, Iter end) {
	const auto& first_elem = *begin;
	size_t count {1};
	while (++begin != end && *begin == first_elem) ++count;
	return count;
}

// lowest common multiple of a group of numbers
template <typename Iter>
typename std::iterator_traits<Iter>::value_type lcm(Iter begin, Iter end) {
	using T = typename std::iterator_traits<Iter>::value_type;
	std::unordered_map<T,size_t> factors_union;

	for (;begin != end; ++begin) {
		vector<T> factors {prime_factorize(*begin)};
		for (auto i = factors.begin(); i != factors.end();) {
			size_t conseq {1};
			T first_elem {*i};
			while (++i != factors.end() && *i == first_elem) ++conseq;
			// 0 if it wasn't created yet
			factors_union[first_elem] = std::max(conseq, factors_union[first_elem]);
		}
	}
	// gather up all factors
	T least_multiple {1};
	for (const auto& factor : factors_union) {
		// key is factor, value is max number of occurrances
		least_multiple *= std::pow(factor.first, factor.second);
	}
	return least_multiple;
}


void test_lcm() {
	GETINPUT();
	int a;
	vector<int> nums;
	while (ss >> a) nums.push_back(a);
	std::cout << lcm(nums.begin(), nums.end()) << std::endl;
}

void test_maths() {
	test_lcm();
}