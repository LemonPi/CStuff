#include "common.h"
#include <vector>
#include <queue>
#include <iostream>
#include <cmath>
#include <limits>
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

struct Cartesian {
	double x,y;
};
std::ostream& operator<<(std::ostream& os, Cartesian point) {
	return os << '(' << point.x << ',' << point.y << ") ";
}

constexpr double epsilon = 1e-10;	// tolerance for difference
constexpr double vertical_slope = std::numeric_limits<double>::max();
struct Line_cartesian {
	// internally represent as y = slope*x + intercept
	double slope, intercept;
	Line_cartesian() = default;
	Line_cartesian(double s, double i, bool v = false) : slope{s}, intercept{i} {}
	Line_cartesian(Cartesian a, Cartesian b) :
		slope{(std::abs(a.x - b.x) < epsilon)? vertical_slope : (a.y - b.y)/(a.x - b.x)},
		// x-intercept for vertical lines
		intercept{(slope == vertical_slope)? a.x : a.y - slope*a.x} {}

	bool contains_point(Cartesian point) const {
		if (slope == vertical_slope) return point.x == intercept;
		return std::abs((point.x * slope + intercept) - point.y) < epsilon;
	}
};
std::ostream& operator<<(std::ostream& os, Line_cartesian line) {
	return os << "y = " << line.slope << " * x + " << line.intercept << endl;	
}

bool will_intercept(Line_cartesian a, Line_cartesian b) {
	// will intercept if lines are not parallel (same slope and not same intercept)
	return !(std::abs(a.slope - b.slope) < epsilon && std::abs(a.intercept - b.intercept) > epsilon);
}

struct Square_cartesian {
	// (x,y) of top left corner
	double x,y,side;
	Cartesian center() {
		return {x + side/2, y + side/2};
	}
};

Line_cartesian bisect_squares(Square_cartesian a, Square_cartesian b) {
	return {a.center(), b.center()};
}

template <typename Iter>
vector<Cartesian> most_colinear_points(Iter begin, Iter end) {
	// from a group of points, find the largest subset of colinear points
	vector<Cartesian> most_col_points;
	for (Iter start = begin; start != end; ++start) {
		for (Iter second = start + 1; second != end; ++second) {

			vector<Cartesian> col_points {*start, *second};
			Line_cartesian line {*start, *second};
			
			// only check points afterwards to avoid double previous attempts
			for (Iter point = second + 1; point != end; ++point) {
				if (line.contains_point(*point)) {
					col_points.push_back(*point);
				}
			}
			if (col_points.size() > most_col_points.size())
				most_col_points = std::move(col_points);
		}
	}
	return most_col_points;
}

template <typename T>
const T& min(const T& a, const T& b, const T& c) {
	return std::min(a, std::min(b,c));
}
// kth number with only prime factors of 3,5,7
long long magic_num(size_t k) {
	long long magic {1};	// k = 0 --> 3^0 * 5^0 * 7^0
	std::queue<long long> m3, m5, m7;	// multiples
	m3.push(3);
	m5.push(5);
	m7.push(7);
	while (k--) {
		magic = min(m3.front(), m5.front(), m7.front());
		if (magic == m3.front()) {
			m3.pop();
			m3.push(3 * magic);
			m5.push(5 * magic);
			m7.push(7 * magic);
		}
		else if (magic == m5.front()) {
			m5.pop();
			m5.push(5 * magic);
			m7.push(7 * magic);
		}
		else {
			m7.pop();
			m7.push(7 * magic);
		}
	}
	return magic;
}

void test_lcm() {
	GETINPUT();
	int a;
	vector<int> nums;
	while (ss >> a) nums.push_back(a);
	std::cout << lcm(nums.begin(), nums.end()) << std::endl;
}


void test_will_intercept() {
	GETINPUT();
	double x,y;
	ss >> x >> y;
	Line_cartesian a {x,y};	
	GETANOTHER();
	ss >> x >> y;
	Line_cartesian b {x,y};
	cout << will_intercept(a, b) << endl;
}

void test_bisect_squares() {
	Square_cartesian a {0,0,5};
	Square_cartesian b {10,10,3};
	Line_cartesian line {bisect_squares(a,b)};
	cout << line;
}

void test_most_colinear_points() {
	vector<Cartesian> points {
		{5,4},{3,5},{6,3},{3,3},{2,3},{4,6},
		{3,2},{4,5},{2,7}
	};

	// should be 4 = {{5,4},{6,3},{4,5},{2,7}}
	auto col_points = most_colinear_points(points.begin(), points.end());
	for (Cartesian point : col_points) cout << point;
	cout << endl;
	cout << Line_cartesian{col_points[0], col_points[1]};
}

void test_magic_num() {
	GETINPUT();
	size_t k;
	ss >> k;
	cout << magic_num(k) << endl;
}

void test_maths() {
	// test_lcm();
	// test_will_intercept();
	// test_bisect_squares();
	// test_most_colinear_points();
	// test_magic_num();
}