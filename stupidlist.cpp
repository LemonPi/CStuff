#include <unordered_set>
#include <iostream>
#include <string>
#include <sstream>
#include <cstdio>
#include "node.h"

using Node = Linked_node;

std::ostream& operator<<(std::ostream& os, Node* head) {
	while (head) {
		os << head->data << ' ';
		head = head->next;
	}
	return os << '\n';
}

// returns tail
Node* rem_dup(Node* head) {
	if (!head) return nullptr;
	std::unordered_set<Node::value_type> vals {head->data};
	while (head->next) {
		while (head->next && vals.find(head->next->data) != vals.end()) {
			Node* temp = head->next;
			head->next = head->next->next;
			delete temp;
		}
		// exit when head->next->data not found, meaning should insert new value
		vals.insert(head->next->data);
		head = head->next;
	}
	return head;
}

Node* rem_dup_nobuf(Node* head) {
	if (!head) return nullptr;
	while (head->next) {
		for (Node* runner = head; runner->next; runner = runner->next) {
			while (head->data == runner->next->data) {
				Node* to_del = runner->next;
				runner->next = runner->next->next;
				delete to_del;
			}
		}
		head = head->next;
	}
	return head;
}

// last is the last non-null node; n = 0 gives this node
Node* find_nth_last (Node* head, size_t n) {
	if (!head) return nullptr;
	Node* runner = head;
	while (n--) {
		if (runner->next) runner = runner->next;
		else return nullptr;
	}
	// head now lags behind runner by n nodes
	while (runner->next) {
		runner = runner->next;
		head = head->next;
	}
	// runner now at last node
	return head;
}

// returns nullptr if not found, else the new node in its place
Node* delete_node(Node* head, Node* to_del) {
	if (!head || !to_del) return nullptr;
	if (head == to_del) {
		head = head->next;
		delete to_del;
		return head;
	}

	while (head->next) {
		if (head->next == to_del) {
			head->next = head->next->next;
			delete to_del;
			return head->next;
		}
		head = head->next;
	}

	return nullptr;
}

// really bad way of doing addition, returns head of sum list
Node* add_list(Node* head1, Node* head2) {
	if (!head1) return head2;
	if (!head2) return head1;
	int digit = head1->data + head2->data;
	head1 = head1->next;
	head2 = head2->next;
	int carry = 0;
	if (digit > 9) {
		digit -= 10;
		carry = 1;
	}

	Node* sum = new Node {digit};
	Node* sum_head = sum;

	while (head1 || head2) {
		// one list longer than the other
		if (!head1) {
			digit = head2->data + carry;
			carry = 0;
			head2 = head2->next;
		}
		else if (!head2) {
			digit = head1->data + carry;
			carry = 0;
			head1 = head1->next;
		}
		else {
			digit = head1->data + head2->data + carry;
			head2 = head2->next;
			head1 = head1->next;
		}

		if (digit > 9) {
			digit -= 10;
			carry = 1;
		}

		sum->next = new Node {digit};
		sum = sum->next;
	}
	// carry over at the end
	if (carry) sum->next = new Node {1};

	return sum_head;
}

// does not work... (lower digits gets automatically promoted )
Node* add_rev_list_rec(Node* head1, Node* head2, int& carry);
Node* add_rev_list(Node* head1, Node* head2) {
	int carry = 0;
	Node* upper_sums = add_rev_list_rec(head1, head2, carry);
	if (carry) {
		Node* top_digit = new Node {1};
		top_digit->next = upper_sums;
		return top_digit;
	}
	else {
		return upper_sums;
	}
}
// inner recursion
Node* add_rev_list_rec(Node* head1, Node* head2, int& carry) {
	if (!head1 && !head2) {carry = 0; return nullptr;}
	int digit = 0;
	Node* lower_sums;
	if (!head1) {
		lower_sums = add_rev_list_rec(nullptr, head2->next, carry);
		digit = head2->data + carry;
	}
	else if (!head2) {
		lower_sums = add_rev_list_rec(head1->next, nullptr, carry);
		digit = head1->data + carry;
	}
	else {
		lower_sums = add_rev_list_rec(head1->next, head2->next, carry);
		digit = head1->data + head2->data + carry;
	}

	if (digit > 9) {
		digit -= 10;
		carry = 1;
	}
	else
		carry = 0;

	Node* this_sum = new Node {digit};
	this_sum->next = lower_sums;
	return this_sum;
}


Node* find_loop_start(Node* head) {
	// head does not actually have to be at one end
	Node* current = head;
	Node* runner = head;
	// if head is nullptr it will naturally exit without special if check
	while (runner && runner->next) {
		// advance until collision
		runner = runner->next->next;
		current = current->next;
		if (runner == current) break;
	}
	if (!runner || !runner->next) return nullptr;
	// current travelled N, runner travelled 2N
	// let A = shared acyclic length, B = length from loop start until collision, L = loop length, C = collision position
	// then N = A + B
	// collision occurred at (N - A) % L = (2N - A) % L ---> N = L
	// C = A + B % L
	// C + A = A + (A + B) % L = A; so from collision point, advancing A will collide with advancing A from head
	runner = head;
	while (runner != current) {
		runner = runner->next;
		current = current->next;
	}
	return current;
}


Node* init_head() {
	int data;
	std::string line;
	std::getline(std::cin, line);
	std::stringstream numbers {line};
	numbers >> data;
	Node* head = new Node{data};
	while (numbers >> data) head->append(data);

	return head;	
}

void test_nth() {
	Node* head = init_head();

	size_t nth;
	std::string line;
	std::getline(std::cin, line);
	std::stringstream numbers {line};	
	numbers >> nth;

	Node* nth_last = find_nth_last(head, nth);
	std::cout << nth_last;	
}

void test_delete() {
	Node* head = init_head();

	Node* some_node = head->next->next;
	Node* behind_some_node = delete_node(head, some_node);
	std::cout << "to del: " << some_node->data << '\n';
	std::cout << "behind: " << behind_some_node;
	std::cout << "whole:  " << head;	
}

void test_add() {
	Node* head1 = init_head();
	Node* head2 = init_head();

	Node* sum = add_rev_list(head1, head2);
	std::cout << sum;
}

void test_loop() {
	Node* head = new Node {0};
	Node* last;
	for (int data = 1; data < 10; ++data) {
		last = head->append(data);
	}
	Node* loop_start = head->next->next->next;
	last->next = loop_start;

	Node* loop_start_test = find_loop_start(head);
	if (loop_start != loop_start_test) {
		std::cout << "find loop start ... FAILED\n";
	}
	else {
		std::cout << "fnid loop start ... OK\n";
	}
}

int main() {
	// test_nth();
	// test_delete();
	// test_add();
	test_loop();
}