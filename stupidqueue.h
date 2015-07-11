#include "node.h"
#include <stack>
#include <iostream>
#include <sstream>



// a backwards pointing linked list (back <-- front)
class Queue {
	Linked_node *front, *back;
public:
	Queue() : front {nullptr}, back {nullptr} {}
	Queue(int d) : front {new Linked_node {d}}, back {front} {}
	Queue(Linked_node* n) : front {n}, back {front} {}

	void enqueue(Linked_node* latest) {
		// previously empty
		if (!back) {
			front = latest;
			back = front;
		}
		else {
			back->next = latest;
			back = latest;
		}
	}

	Linked_node* dequeue() {
		if (!front) return nullptr;
		Linked_node* expelled = front;
		front = front->next;
		return expelled;
	}
};

// implementation of queue using 2 stacks, O(1) for continuous enqueues or dequeues,
// but O(n) for alternating enqueues and dequeues
class Stack_queue {
	std::stack<int> enq, deq;

	// each dump assumes the other stack is empty
	void dumpto_enq() {
		while (!deq.empty()) {
			enq.push(deq.top());
			deq.pop();
		}
	}
	void dumpto_deq() {
		while (!enq.empty()) {
			deq.push(enq.top());
			enq.pop();
		}
	}

public:
	Stack_queue() {}
	void enqueue(int val) {
		// at each dump point, the other stack should be empty
		if (enq.empty()) dumpto_enq();
		enq.push(val);
	}

	int dequeue() {
		int val = peek();
		deq.pop();
		return val;
	}

	int peek() {
		if (deq.empty()) dumpto_deq();
		int val = deq.top();
		return val;		
	}

	bool empty() {
		return enq.empty() && deq.empty();
	}
	// destructively print queue
	void examine() {
		while (!empty()) {
			std::cout << dequeue() << ' ';
		}
		std::cout << std::endl;
	}
};


void test_stack_queue() {
	int data;
	std::string line;
	std::getline(std::cin, line);
	std::stringstream numbers {line};

	Stack_queue sq;
	while (numbers >> data) sq.enqueue(data);

	std::cout << sq.dequeue() << '\n';
	sq.enqueue(0);
	sq.examine();
}