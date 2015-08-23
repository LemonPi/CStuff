#include <iostream>
#include <algorithm>
#include <vector>
#include "common.h"
#include "sal/algo/utility.h"
using std::vector;
using sal::Timer;

template <typename T, typename Cmp = std::less<T>>
class Heap {
	std::vector<T> elems;
	Cmp cmp;

	size_t parent(size_t i) const {return i >> 1;}
	size_t left(size_t i) const {return i << 1;}
	size_t right(size_t i) const {return (i << 1) + 1;}
	size_t head() const {return 1;}
	size_t tail() const {return elems.size()-1;}
	bool is_leaf(size_t i) const {return i > (elems.size() / 2);}

	void sift_down(size_t hole) {
		T item {std::move(elems[hole])};
		size_t child {left(hole)};
		while (child < elems.size()) {
			if (child + 1 < elems.size() && cmp(elems[child + 1], elems[child]))
				++child;
			if (cmp(elems[child], item)) {
				elems[hole] = std::move(elems[child]);
				hole = child;
				child = left(child);
			}
			else break;
		}
		elems[hole] = std::move(item);
	}
	void sift_up(size_t hole, T&& item) {
		while ((hole > 1) && (cmp(item, elems[parent(hole)]))) {
			// sift hole up
			DEBUG("sifting up " << item << " down " << elems[parent(hole)])
			elems[hole] = elems[parent(hole)];
			hole = parent(hole);
		}	
		DEBUG("sift finished on (" << elems[hole] << ") replacing with " << item)	
		elems[hole] = item;
	}
	// non-move version
	void sift_up(size_t hole, const T& item) {
		while ((hole > 1) && (cmp(item, elems[parent(hole)]))) {
			// sift hole up
			DEBUG("sifting up " << item << " down " << elems[parent(hole)])
			elems[hole] = elems[parent(hole)];
			hole = parent(hole);
		}	
		DEBUG("sift finished on (" << elems[hole] << ") replacing with " << item)	
		elems[hole] = item;
	}

public:
	size_t sifts;
	Heap() = default;
	Heap(size_t init_size, Cmp&& c = Cmp{}) : cmp{c} {
		elems.reserve(init_size+1);
		elems.push_back(T{});
	}			
	template <typename Container>
	Heap(Container&& previous_owner, Cmp&& c = Cmp{}) : elems{std::move(previous_owner)}, cmp{c} {
		elems.push_back(T{});
		std::swap(elems[0], elems.back());
		build_heap();
	}

	void build_heap() {
		for (size_t i = elems.size() >> 1; i != 0; --i)
			sift_down(i);
	}

	size_t size() const {return elems.size() - 1;}
	bool empty() const {return elems.size() <= 1;}

	void push_back(const T& item) {elems.push_back(item);}

	template <typename Container>
	void transfer_into(Container&& previous_owner) {
		elems = std::move(previous_owner);
		elems.push_back(T{});
		std::swap(elems[0], elems.back());
		build_heap();
	}
	void insert(const T& item) {
		elems.emplace_back();
		sift_up(tail(), item);
	}
	void emplace(T&& item) {
		elems.emplace_back();
		sift_up(tail(), std::move(item));
	}
	T extract_top() {
		if (empty()) return T{};
		T top {elems[1]};
		DEBUG("extracting " << top)

		size_t hole {1};
		size_t child {2};
		while (child + 1 < elems.size()) {

			DEBUG("left " << elems[child] << " right " << elems[child + 1])
			if (cmp(elems[child + 1], elems[child])) 
				++child;

			DEBUG("filling hole with " << elems[child])
			elems[hole] = elems[child];
			hole = child;
			child = left(child);
		}

		DEBUG("replacing hole (" << elems[hole] << ") with " << elems.back())
		sift_up(hole, std::move(elems.back()));
		elems.pop_back();

		return top;
	}
	typename vector<T>::iterator begin() {return elems.begin()+1;}
	typename vector<T>::iterator end() {return elems.end();}
	typename vector<T>::const_iterator begin() const {return elems.begin()+1;}
	typename vector<T>::const_iterator end() const {return elems.end();}

	bool is_valid() const {
		for (size_t i = 1; i <= elems.size() >> 1; ++i) {
			if (left(i) < elems.size() && cmp(elems[left(i)], elems[i])) return false;
			if (right(i) < elems.size() && cmp(elems[right(i)], elems[i])) return false;
		}
		return true;
	}
};

constexpr size_t testsize = 10000000;
template <typename T, typename Cmp>
void test_siftdown_build(vector<T>& res, Cmp&& cmp) {
	Heap<T, Cmp> heap{testsize};
	Timer time;
	for (const T& t : res)
		heap.push_back(t);
	heap.build_heap();
	cout << "siftdown: " << time.tonow() / 1000.0 << endl;
	// sal::print(heap);
	// cout << "is heap: " << heap.is_valid() << endl;
	std::sort(res.begin(), res.end(), cmp);
	time.restart();
	size_t i = 0;
	while (!heap.empty()) {
		// heap.extract_top();
		// cout << heap.extract_top() << ' ';
		auto top = heap.extract_top(); if (top != res[i++]) {cout << "mismatch with sorted " << top << ' ' << res[i-1] << endl; return;}
	};
	if (i != res.size()) cout << "did not finish extracting: " << i << " vs " << res.size() << endl;
	cout << endl;
	cout << "extract top: " << time.tonow() / 1000.0 << endl;
	cout << endl;
}
template <typename T, typename Cmp>
void test_siftup_build(vector<T>& res, Cmp&& cmp) {
	Heap<T, Cmp> heap{testsize};
	Timer time;
	for (const T& t : res)
		heap.insert(t);
	cout << "siftup: " << time.tonow() / 1000.0 << endl;
	std::sort(res.begin(), res.end(), cmp);
	cout << "is heap: " << heap.is_valid() << endl;
	time.restart();
	size_t i = 0;
	while (!heap.empty()) {
		// heap.extract_top();
		// cout << heap.extract_top() << ' ';
		auto top = heap.extract_top(); if (top != res[i++]) {cout << "mismatch with sorted " << top << ' ' << res[i-1] << endl; return;}
	};
	if (i != res.size()) cout << "did not finish extracting: " << i << " vs " << res.size() << endl;
	cout << endl;
	cout << "extract top: " << time.tonow() / 1000.0 << endl;
}

template <typename T>
void test_std_heap(vector<T>& vec) {
	Timer time;
	std::make_heap(vec.begin(), vec.end(), std::greater<int>());
	cout << "std: " << time.tonow() / 1000.0 << endl;
}

void test_heap() {
	vector<int> vec {sal::randgen(0, (int)testsize, testsize, 100)};
	// vector<int> vec {13,1,3,4,5,2,3,4,5,1,7};
	// vector<int> vec {3, 4, 6, 5, 1, 8, 11, 12};

	// for (int i = 11; i >= 0; --i) vec.push_back(i);
	// print(vec);
	test_siftdown_build(vec, std::greater<int>{});
	// test_siftup_build(vec, std::greater<int>{});
	// test_std_heap(vec);
}