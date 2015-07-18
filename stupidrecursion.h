#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <unordered_set>
#include <set>
#include <iterator>
#include <algorithm>
#include "sal/algo/numeric.h"	// choose

#ifndef GETINPUT
#define GETINPUT() \
	std::string line;\
	std::getline(std::cin, line);\
	std::stringstream ss {line};
#endif

// a is modified as if call was a *= b
template <typename T>
void mul_square_mat_2(T a[4], const T b[4]) {
	T top_l {a[0]*b[0] + a[1]*b[2]};
	T top_r {a[0]*b[1] + a[1]*b[3]};
	T bot_l {a[2]*b[0] + a[3]*b[2]};
	T bot_r {a[2]*b[1] + a[3]*b[3]};
	a[0] = top_l;
	a[1] = top_r;
	a[2] = bot_l;
	a[3] = bot_r;
}

using fib_num = unsigned long long;
fib_num fibonacci(size_t n) {
	fib_num mat[4] {1, 1,
					1, 0};	// fibonacci Q matrix
	fib_num res[4] {1, 0,
					0, 1};	// 2x2 identity
	// matrix exponentiation gives us sqrt(n) performance for fibonacci
	while (n > 0) {
		if (n & 1) mul_square_mat_2(res, mat);
		// divide problem in half
		n >>= 1;
		// a^4 = (a*a)^2
		mul_square_mat_2(mat, mat);
	}
	return res[1];	// can return either top right or bottom left
}
// really bad...
fib_num fibonacci_recursive(size_t n) {
	if (n < 2) return n;
	return fibonacci_recursive(n - 1) + fibonacci_recursive(n - 2);
}
// slightly better
fib_num fibonacci_tail_recursive(size_t n, fib_num fn = 0, fib_num fn_1 = 1) {
	if (n < 1) return fn;
	return fibonacci_tail_recursive(n - 1, fn_1, fn + fn_1);
}


// from top left corner of a rectangular grid, how many ways to get to bottom right without backtracking
size_t path_ways_naive(size_t row, size_t col) {
	// only 1 way down linear corridor
	if (row == 1 || col == 1) return 1;
	// take step down and to the right
	return path_ways_naive(row-1, col) + path_ways_naive(row, col-1);
}
size_t path_ways_math(size_t row, size_t col) {
	// need to take row-1 steps down, col-1 steps right
	// total of row+col - 2 steps
	return sal::choose(row+col - 2, row-1);
}

void path_traceback(size_t r, size_t c, size_t trace_index, 
	std::vector<std::string>& traces, const std::vector<size_t>& paths,
	size_t row, size_t col) {

	size_t trace_depth {traces[0].capacity()};

	// out of bounds
	if (r > row-1 || c > col-1) return;

	// came from both directions
	if (paths[r*col + c] == paths[(r+1)*col + c] + paths[r*col + c + 1]) {
		// create another branch for going right
		traces.emplace_back(traces[trace_index]);
		size_t branch_index = traces.size() - 1;
		traces[branch_index].reserve(trace_depth);
		traces[trace_index]  += 'D';
		traces[branch_index] += 'R';
		path_traceback(r + 1, c, trace_index, traces, paths, row, col);
		path_traceback(r, c + 1, branch_index, traces, paths, row, col);
	}
	else if (paths[r*col + c] == paths[(r+1)*col + c]) {
		traces[trace_index] += 'D';
		path_traceback(r + 1, c, trace_index, traces, paths, row, col);
	}
	else if (paths[r*col + c] == paths[r*col + c + 1]) {
		traces[trace_index] += 'R';
		path_traceback(r, c + 1, trace_index, traces, paths, row, col);		
	}
}

size_t path_ways_dp(size_t row, size_t col, const std::unordered_set<int>& no_go) {
	auto valid_move = [&](int r, int c)->bool {
		return no_go.find(r*col + c) == no_go.end();
	};

	std::vector<size_t> paths(row*col, 0);
	// last row and column
	for (size_t c = 0; c < col; ++c) paths[(row-1)*col + c] = 1;
	for (size_t r = 0; r < row; ++r) paths[r*col + (col-1)] = 1;

	// start 1 row above and 1 col to the left of the target
	for (int r = row-2; r >= 0; --r) {
		for (int c = col-2; c >= 0; --c) {
			size_t paths_to_here {0};
			// coming from the bottom
			if (valid_move(r+1,c)) paths_to_here += paths[(r+1)*col + c];
			// coming from the right
			if (valid_move(r,c+1)) paths_to_here += paths[r*col + c + 1];
			paths[r*col + c] = paths_to_here;
		}
	}
	// do traceback to find all paths
	for (size_t r = 0; r < row; ++r) {
		for (size_t c = 0; c < col; ++c) {
			if (valid_move(r,c)) std::cout << std::setw(4) << paths[r*col + c];
			else std::cout << std::setw(4) << 'x';
		}
		std::cout << std::endl;
	}

	// each path is represented as a string with each character being R || D
	std::vector<std::string> traceback;
	traceback.emplace_back();
	// row-1 steps down, col-1 steps right
	size_t trace_depth {row-1 + col-1};
	traceback[0].reserve(trace_depth);	

	path_traceback(0,0,0,traceback,paths,row,col);

	std::vector<std::string> valid_tracebacks;
	for (std::string& trace : traceback) 
		if (trace.size() == trace_depth) valid_tracebacks.emplace_back(std::move(trace));

	for (const auto& trace : valid_tracebacks)
		std::cout << trace << std::endl;

	return paths[0];
}

template <typename T>
using Superset = std::set<std::set<T>>;

template <typename T>
Superset<T> all_subsets(const std::set<T>& elems) {
	Superset<T> superset {std::set<T>{}};	// start with the empty set
	for (const T& elem : elems) {
		// temporary container for sets to add for this round
		// initialized the the single element itself
		std::vector<std::set<T>> to_add {std::set<T>{elem}};
		// make subset by combining with existing subsets
		for (const std::set<T>& subset : superset) {
			to_add.push_back(subset);
			to_add.back().insert(elem);
		}
		// move back into superset
		for (size_t s = 0; s < to_add.size(); ++s) {
			superset.emplace(std::move(to_add[s]));
		}
	}
	// by the end will have the combination between all possible subsets
	return superset;
}

using std::vector;
template <typename Iter, typename T>
struct all_perms_wrapper {

	static vector<vector<T>> all_perms(Iter begin, Iter end) {
		if (begin == end) return {};
		if (std::distance(begin, end) == 1) return {vector<T>{*begin}};
		int perm_len {(int)std::distance(begin, end)};
		// else permute every following element
		T leading_elem {*begin};
		auto remaining_perms = all_perms(++begin, end);
		// for each permutation insert leading_elem at each point
		vector<vector<T>> all_perms;
		// should reserve perm_len!
		for (const auto& perm : remaining_perms) {
			for (size_t insert_leading = 0; insert_leading < perm_len; ++insert_leading) {
				vector<T> newperm((size_t)perm_len);
				newperm[insert_leading] = leading_elem;
				// copy over, but skip the leading character 
				for (size_t i = 0,j = 0; j < perm.size(); ++i,++j) {
					if (i == insert_leading) ++i;
					newperm[i] = perm[j];
				}
				all_perms.emplace_back(std::move(newperm));
			}
		}
		return all_perms;
	}
};


template <typename Iter>
vector<vector<typename Iter::value_type>> all_perms(Iter begin, Iter end) {
	return all_perms_wrapper<Iter, typename Iter::value_type>::all_perms(begin, end);
}


void combine_paren_rec(vector<std::string>& parens, size_t lp, size_t rp, int paren_lv, size_t combo_index) {
	if (lp) {
		if (paren_lv) {
			// branch into closing and opening a parenthesis
			std::string closed_branch {parens[combo_index]};
			closed_branch += ')';			
			parens.emplace_back(std::move(closed_branch));
			combine_paren_rec(parens, lp, rp-1, paren_lv-1, parens.size()-1);	// close rp
		}
		parens[combo_index] += '(';
		combine_paren_rec(parens, lp-1, rp, paren_lv+1, combo_index);		// open lp
	}
	// out of lp, just close
	else {
		while (rp) {
			--rp; --paren_lv;
			parens[combo_index] += ')';
		}
		if (paren_lv != 0) std::cerr << "lp:" << lp << " rp:" << rp << " paren_lv:" << paren_lv << std::endl;
	}
}

vector<std::string> combine_paren(size_t paren_pairs) {
	vector<std::string> parens {std::string{}};
	parens[0].reserve(paren_pairs*2);
	combine_paren_rec(parens, paren_pairs, paren_pairs, 0, 0);
	return parens;
}



template <typename Color>
void paint(vector<vector<Color>>& screen, Color orig, Color new_col, size_t x, size_t y) {
	// make sure screen has 1px border to prevent going out of bounds
	if (screen[x][y] != orig) return;
	screen[x][y] = new_col;
	// spill to neighbours
	paint(screen,orig,new_col,x,y+1);
	paint(screen,orig,new_col,x,y-1);
	paint(screen,orig,new_col,x-1,y);
	paint(screen,orig,new_col,x+1,y);
}
// user calls this (unless they specify original color)
template <typename Color>
void paint(vector<vector<Color>>& screen, Color new_col, size_t x, size_t y) {
	paint(screen, screen[x][y], new_col, x, y);
}


// count number of ways to generate sum using num_vals elements from vals
template <typename T>
size_t count_combos(const vector<T>& vals, T sum) {
    // create table to avoid recomputation of same subproblem
    // table stores # of solutions for certain sum (as index)
    vector<int> table(sum+1, 0);

    table[0] = 1;   // base case of 1 solution

    for (const T& value_unit : vals) {
    	// std::cout << value_unit << ": ";
        for (T val = value_unit; val <= sum; ++val) {
        	// std::cout << table[val - value_unit] << ' ';
            table[val] += table[val - value_unit];
        }
        // std::cout << std::endl;
    }

    return table[sum];
}

bool check_diagonal_attacks(const vector<int>& cols, int row) {
	for (int r = 0; r < row; ++r) {
		int diff {std::abs(cols[r] - cols[row])};
		// difference in col would equal difference in row for diagonal
		if (diff == row - r) return true;
	}
	return false;	// no attacks
}
vector<vector<std::pair<int,int>>> n_queens(size_t n) {
	vector<vector<std::pair<int,int>>> sols;
	// n is size of column and row
	// restrict each queen to a row, and constrain game to column permutation with unique indices
	vector<int> cols; cols.reserve(n);
	// start with the first permutation of columns {0,1,2,3..n}
	for (int c = 0; c < n; ++c) cols.push_back(c);

	// n! number of permutations
	while (std::next_permutation(cols.begin(), cols.end())) {
		// check for diagonal attacks
		int conflict {0};
		for (int q = 0; q < n; ++q)
			conflict |= check_diagonal_attacks(cols, q);

		if (!conflict) {
			vector<std::pair<int,int>> sol;
			for (int q = 0; q < n; ++q) sol.emplace_back(q, cols[q]);
			sols.emplace_back(std::move(sol));
		}
	}
	return sols;
}

void test_fibonacci() {
	GETINPUT();
	size_t n;
	ss >> n;
	std::cout << fibonacci(n) << std::endl;
	// std::cout << fibonacci_recursive(n) << std::endl;
	std::cout << fibonacci_tail_recursive(n) << std::endl;
}

void test_path_ways() {
	GETINPUT();
	int row, col;
	ss >> row >> col;
	std::unordered_set<int> no_go {
		3*col + 2,
		// 0*col + 4
	};
	// naive is O(2^(row+col))
	// std::cout << path_ways_naive(row, col) << std::endl;
	std::cout << path_ways_math(row, col) << std::endl;
	std::cout << path_ways_dp(row, col, no_go) << std::endl;

}

void test_all_subsets() {
	GETINPUT();
	int elem;
	std::set<int> elems;
	while (ss >> elem) elems.insert(elem);
	auto superset = all_subsets(elems);
	for (const auto& subset : superset) {
		std::cout << '{';
		for (int e : subset) std::cout << e << ' ';
		std::cout << "}\n";
	}
}

void test_all_perms() {
	GETINPUT();
	char c;
	std::vector<char> s;
	while (ss >> c) s.push_back(c);

	auto allperms = all_perms(s.begin(), s.end());
	for (const auto& perm : allperms) {
		for (char c : perm) {
			std::cout << c;;
		}
		std::cout << std::endl;
	}
}

void test_paren_pairs() {
	GETINPUT();
	size_t paren_pairs;
	ss >> paren_pairs;
	auto pair_combos = combine_paren(paren_pairs);
	for (const auto& combo : pair_combos)
		std::cout << combo << std::endl;
}

void test_count_combos() {
	vector<int> coins {1, 5, 10, 25};
	GETINPUT();
	int n_cents;
	ss >> n_cents;
	std::cout << count_combos(coins, n_cents) << std::endl;
}

void test_n_queens() {
	GETINPUT();
	size_t n;
	ss >> n;
	auto sols = n_queens(n);
	for (const auto& sol : sols) {
		for (const auto& coordinate : sol) {
			std::cout << '(' << coordinate.first << ',' << coordinate.second << ") ";
		} 
		std::cout << std::endl;
	}
	std::cout << sols.size() << std::endl;
}

void test_recursion() {
	// test_fibonacci();
	// test_path_ways();
	// test_all_subsets();
	// test_all_perms();
	// test_paren_pairs();
	// test_count_combos();	
	test_n_queens();
}