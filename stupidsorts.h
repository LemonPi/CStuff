#include <iterator>
#include <vector>
#include <algorithm>
#include "sal/algo/sort/simple_sorts.h"
#include "common.h"

// hash must have size_t size() = #buckets; size_t operator(const T&) = bucket for object 
template <typename Iter, typename Hash>
void bucket_sort(Iter begin, Iter end, Hash&& hash) {
	using T = typename std::iterator_traits<Iter>::value_type;
	std::vector<std::vector<T>> buckets(hash.size());
	// scatter into buckets
	for (auto cur = std::make_move_iterator(begin); cur != std::make_move_iterator(end); ++cur) {
		size_t b {hash(*cur)};
		std::cout << b << ' ' << *cur << std::endl;
		buckets[b].emplace_back(*cur);	// moved
	}
	// sort each bucket separately
	for (auto& bucket : buckets)
		std::sort(bucket.begin(), bucket.end());
	// gather back into original container
	for (auto& bucket : buckets) {
		for (auto cur = std::make_move_iterator(bucket.begin());
			  cur != std::make_move_iterator(bucket.end()); ++cur) {
			*begin = *cur;	// move back
			++begin;
		}
	}
}

template <typename Iter, typename Hash>
void bucket_ins_sort(Iter begin, const Iter end, Hash&& hash) {
	using T = typename std::iterator_traits<Iter>::value_type;
	std::vector<std::vector<T>> buckets(hash.size());
	// scatter into buckets
	for (auto cur = std::make_move_iterator(begin); cur != std::make_move_iterator(end); ++cur) {
		size_t b {hash(*cur)};
		std::cout << b << ' ' << *cur << std::endl;
		buckets[b].emplace_back(*cur);	// moved
	}
	// gather unsorted elements back into array
	auto beg = begin;
	for (auto& bucket : buckets) {
		for (auto cur = std::make_move_iterator(bucket.begin());
			  cur != std::make_move_iterator(bucket.end()); ++cur) {
			*beg = *cur;	// move back
			++beg;
		}
	}	
	// insertion sort on the original container
	sal::lin_sort(begin, end);
}

struct Number_bucket_hash {
	size_t num_buckets;
	double proportion_of_max;

	Number_bucket_hash() = default;
	template <typename Iter>
	Number_bucket_hash(Iter begin, Iter end, size_t n) : num_buckets{n}, 
		proportion_of_max{(num_buckets-1) / (double)*std::max_element(begin, end)} {}

	size_t size() const {
		return num_buckets;
	}
	template <typename T>
	size_t operator()(const T& val) const {
		return (size_t) (val * proportion_of_max);
	}
};

// merge b into a assuming it has enough space
template <typename Indexable>
void merge_into(Indexable&& a, Indexable&& b, size_t a_size) {
	// sorted arrays with enough of a buffer behind a (after a_size elements)
	// merge from the back of a
	int back = a_size + b.size() - 1;
	int i = a_size - 1;
	int j = b.size() - 1;
	while (i >= 0 && j >= 0) {
		if (a[i] > b[j]) a[back] = a[i--];
		else 			 a[back] = b[j--];
		--back;
	}
	while (j >= 0) a[back--] = b[j--];
	// a's elements would already be in place
}

struct Anagram_comp {
	template <typename Indexable>
	bool operator()(const Indexable& a, const Indexable& b) const {
		double a_val {1}, b_val {1};
		for (const auto& elem : a) a_val *= elem;
		for (const auto& elem : b) b_val *= elem;
		return a_val < b_val;
	}
};
template <typename Iter>
void anagram_sort(Iter begin, const Iter end) {
	std::sort(begin, end, Anagram_comp{});
}

// binary search on a rotated sorted sequence
template <typename Iter>
Iter rotated_find(Iter begin, const Iter end, const typename std::iterator_traits<Iter>::value_type& key) {
	int l = 0, u = std::distance(begin, end) - 1;
	while (l <= u) {
		int m {(l + u) >> 1};
		if (begin[m] == key) return begin + m;
		// min element is to the right of m
		else if (begin[l] <= begin[m]) {
			// element is on other slice
			if (key < begin[l]) l = m + 1;
			// element is on this slice
			else if (key < begin[m]) u = m - 1;
			else 				     l = m + 1;
		}
		// part of the tiny sharp end of the right slice
		else if (key < begin[m]) u = m - 1;
		// part of the fat end of the right slice
		else if (key < begin[u]) l = m + 1;
		// part of the left slice
		else u = m - 1;
	}
	return end;
} 

// requires random access iterator for efficient implementation of binary search
template <typename Iter>
Iter interspersed_search(Iter begin, const Iter end, const std::string& key) {
	// modified binary search over container of strings which have empty strings
	// interspersed with actual data (which are relatively sorted)

	// due to incrementation, the first element will never be checked; do so explicitely
	if (*begin == key) return begin;

	int l {0}, u {end - begin - 1};
	while (l <= u) {
		int m = (l + u) / 2;
		while (begin[m].empty()) {
			++m;
			if (m > u) return end;
		}
		// reached actual string
		if (key == begin[m]) return begin + m;
		else if (key < begin[m]) u = m - 1;
		else 					 l = m + 1;
	}
	return end;
}

// sorted row and cols, returns (row,col) or (-1,-1) for not found
template <typename Mat2D, typename T>
std::pair<int,int> matrix_search(const Mat2D& mat, const T& key) {
	int row = 0, col = mat[0].size() - 1;
	while (row < mat.size() && col >= 0) {
		if (key == mat[row][col]) return {row, col};
		// must be in lower rows since greater than row's max element
		else if (key > mat[row][col]) ++row;
		// smaller than row's max element (and not in previous rows), so must be in a previous col
		else 						  --col;
	}
	return {-1, -1};
}

struct Circus_person {
	int ht, wt;
};

// tower is a sequence of Circus_person where each previous person must have ht and wt both <
template <typename Indexable>
std::vector<Circus_person> make_circus_tower(Indexable&& ppl) {
	// sort by height and weight on 2 passes
	std::sort(ppl.begin(), ppl.end(), [](const Circus_person& a, const Circus_person& b) {
		return a.ht < b.ht; });	
	std::stable_sort(ppl.begin(), ppl.end(), [](const Circus_person& a, const Circus_person& b) {
		return a.wt < b.wt; });

	auto stackable = [&](size_t a, size_t b) {return ppl[a].ht < ppl[b].ht && ppl[a].wt < ppl[b].wt;};

	size_t seq_start {0};
	bool found_another_start {false};

	for (const auto& cp : ppl) std::cout << '(' << cp.ht << ',' << cp.wt << ") ";
	std::cout << std::endl;

	// indices
	std::vector<size_t> tallest_tower;
	while (seq_start < ppl.size()) {
		std::cout << "starting sequence at " << seq_start << std::endl;
		size_t last_stackable {seq_start};
		std::vector<size_t> tower {seq_start};
		for (size_t p = seq_start + 1; p < ppl.size(); ++p) {
			if (stackable(last_stackable, p)) {
				last_stackable = p;
				tower.push_back(p);
			}	
			// the unstackable element becomes the next potential starting point
			else if (!found_another_start) {
				seq_start = p;
				found_another_start = true;
			}
		}
		if (tower.size() > tallest_tower.size()) tallest_tower = std::move(tower);
		// reached the end without finding new starts
		if (!found_another_start) break;
		// reset for next sequence
		else found_another_start = false;	
	}

	// convert indices to persons
	std::vector<Circus_person> actual_tower;
	for (size_t p : tallest_tower) actual_tower.push_back(ppl[p]);
	return actual_tower;
}

void test_bucket_sort() {
	GETINPUT();
	std::vector<int> nums;
	int num;
	while (ss >> num) nums.push_back(num);
	auto hash = Number_bucket_hash{nums.begin(), nums.end(), 5};
	bucket_ins_sort(nums.begin(), nums.end(), std::move(hash));

	for (int n : nums) std::cout << n << ' ';
	std::cout << std::endl;
}

void test_merge_into() {
	std::vector<int> a {1,3,5,8,10,21,42,50};
	std::vector<int> b {-5,-2,0,4,6,9,10,70};
	size_t a_size {a.size()};
	a.resize(a.size()+b.size());
	merge_into(a,b,a_size);
	for (int elem : a) std::cout << elem << ' '; 
	std::cout << std::endl;
}

// anagrams should be beside each other
void test_anagram_sort() {
	std::vector<std::string> strs {
		"oldwestaction",
		"sdlkfj",
		"panama",
		"askldf",
		"isntrearrangementrave",
		"schoolmaster",
		"sdlkfjlkasdf",
		"silent",
		"iwoerq",
		"irearrangementservant",
		"amanap",
		"kljklqwejqwe",
		"askldjfklasjlklweqrww",
		"klasdfjlqw124lklwefpq",
		"qkljqwklrjklqwrjlwqea",
		"theclassroom",
		"listen",
		"weiorq",
		"clinteastwood",
		"asldkfjasdffd",
		"wqio124o21pws",
		"ldskafjlio241",
		"internetanagramserver"
	};
	anagram_sort(strs.begin(), strs.end());
	for (const auto& s : strs) std::cout << s << std::endl;
}

void test_rotated_find() {
	std::vector<int> elems {15,16,19,20,25,1,3,4,5,7,10,14};
	std::cout << rotated_find(elems.begin(), elems.end(),5) - elems.begin() << std::endl;
}

void test_interspersed_search() {
	GETINPUT();
	std::string s;
	ss >> s;
	std::vector<std::string> elems {"","","at","","","","ball","","","car","","","dad"};
	auto find = interspersed_search(elems.begin(), elems.end(), s);
	if (find != elems.end()) std::cout << find - elems.begin() << std::endl;
	else std::cout << -1 << std::endl;
}

using std::vector;
void test_matrix_search() {
	GETINPUT();
	int elem;
	ss >> elem;
	vector<vector<int>> mat {{1, 2, 5},
							 {3, 4, 6},
							 {7, 8, 9}};
	auto find = matrix_search(mat, elem);
	std::cout << find.first << ' ' << find.second << std::endl;
}

void test_make_circus_tower() {
	vector<Circus_person> ppl {
		{65,100}, {70,150}, {56,90}, {75,190}, {60,95}, {68,110}
	};
	vector<Circus_person> tower {make_circus_tower(ppl)};
	std::cout << tower.size() << ": ";
	for (const auto& cp : tower) std::cout << '(' << cp.ht << ',' << cp.wt << ") ";
	std::cout << std::endl;
}

void test_sorts() {
	// test_bucket_sort();
	// test_merge_into();
	// test_anagram_sort();
	// test_rotated_find();
	// test_interspersed_search();
	// test_matrix_search();
	test_make_circus_tower();
}