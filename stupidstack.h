#pragma once
#include <iostream>
#include <iomanip>
#include <vector>
#include <stack>
#include "node.h"

// linked-list based stack
template <typename N>
class Stack {
	using T = typename N::value_type;
	N* tp;

public:
	Stack() : tp {nullptr} {}
	Stack(T top_data) : tp {new N {top_data}} {}
	// assume ownership if given a Linked_node (assuming it won't be modified outside)
	Stack(N* n) : tp {n} {}

	N* push(N* n) {
		n->next = tp;
		tp = n;
		return tp;
	}
	N* push(T data) {
		return push(new N {data});
	}
	N* pop() {
		if (!tp) return nullptr;
		N* old_tp = tp;
		tp = tp->next;
		return old_tp;
	}

	const N* top() {return tp;}
};

class Min_stack {
	using T = typename Linked_node::value_type;
	Stack<Linked_node> values;
	Stack<Linked_node> mins;
public:
	// only the first instance of a smaller value will be added (and last removed)
	void push(Linked_node* n) {
		if (!n) return;
		values.push(n);
		if (!mins.top() || n->data < mins.top()->data) 
			mins.push(n);
	}
	void push(T d) {return push(new Linked_node {d});}
	Linked_node* pop() {
		Linked_node* extracted = values.pop();
		if (!extracted) return nullptr;
		if (extracted == mins.top())
			mins.pop();
		return extracted;
	}

	const Linked_node* min() {
		return mins.top();
	}
};

template <typename T>
class Stack_set {
	// should always have at least 1 stack
	std::vector<std::stack<T>> ss;
	size_t max_height;
public:
	Stack_set(size_t mh) : ss{std::stack<T>{}}, max_height {mh} {}

	size_t push(const T& val) {
		ss.back().push(val);
		if (ss.back().size() >= max_height)
			ss.emplace_back();
		return ss.size() - 1;
	}

	T pop() {
		if (ss.back().empty()) ss.pop_back();
		// no longer needed
		T val = std::move(ss.back().top());
		ss.back().pop();
		return val;
	}
	T pop_at(size_t si) {
		T val = std::move(ss[si].top());
		ss[si].pop();
		if (ss[si].empty())
			ss.erase(ss.begin() + si);
		return val;
	}
	bool empty() {
		return ss[0].empty();
	}

	void print() {
		if (empty()) {std::cout << "empty\n"; return;}
		for (size_t s = 0; s < ss.size(); ++s) {
			std::cout << s << ": ";
			std::cout << "size: " << ss[s].size();
			std::cout << " top: " << ss[s].top();
			std::cout << '\n';
		}
	}
};

class Hanoi_towers {
	// use ints to represent disk number
	std::vector<std::stack<int>> towers;
	size_t game_size;
	size_t moves_taken;
public:
	Hanoi_towers(size_t N) : towers(3), game_size{N}, moves_taken{0} {
		for (int n = (int) N; n > 0; --n)
			towers[0].push(n);
	}
	size_t solve() {
		if (!moves_taken) move(0, 2, game_size);
		return moves_taken;
	}
	void move(size_t move_from, size_t move_to, size_t num_moved) {
		std::cout << "Moving " << num_moved << " from " << move_from << " to " << move_to << '\n';
		++moves_taken;
		// 1 element can be moved normally
		if (num_moved <= 1) {
			towers[move_to].push(towers[move_from].top());
			towers[move_from].pop();
			return;
		}
		// else need to use the other tower as temporary
		// 0 + 1 + 2 = 3 can use sum to deduce the index of the other tower
		size_t move_temp = 3 - move_to - move_from;
		move(move_from, move_temp, num_moved - 1);
		// the bottom disk
		move(move_from, move_to, 1);
		// move back top disks
		move(move_temp, move_to, num_moved - 1);
	}

	// destructively examine stacks
	void examine() {
		for (size_t t = 0; t < 3; ++t) {
			std::cout << t << ": ";
			while (!towers[t].empty()) {
				std::cout << towers[t].top() << ' ';
				towers[t].pop();
			}
			std::cout << std::endl;
		}
	}
};

void test_min_stack() {
	Min_stack ms;
	ms.push(10);
	ms.push(52);
	ms.push(8);
	ms.push(5);
	ms.push(7);
	ms.push(5);

	std::cout << ms.min() << std::endl;
	ms.pop();
	std::cout << ms.min() << std::endl;
	ms.pop();
	std::cout << ms.min() << std::endl;	
	ms.pop();
	std::cout << ms.min() << std::endl;	
	ms.pop();
	std::cout << ms.min() << std::endl;
	ms.pop();
	std::cout << ms.min() << std::endl;
	ms.pop();
	std::cout << ms.min() << std::endl;
	ms.pop();
	std::cout << ms.min() << std::endl;
}

void test_stack_set() {
	Stack_set<int> ss {5};
	for (int i = 10; i < 55; i += 2) ss.push(i);

	ss.print();

	auto val = ss.pop_at(2);

	std::cout << val << std::endl;
	ss.print();

	// auto val = ss.pop();
	// std::cout << val << std::endl;
	// while (!ss.empty()) ss.pop();

	// ss.print();
}

void test_hanoi() {
	Hanoi_towers ht {5};
	size_t moves_taken = ht.solve();
	std::cout << moves_taken << std::endl;
	ht.examine();
}