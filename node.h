#pragma once
#include <iostream>
struct Linked_node {
	using value_type = int;

	Linked_node* next {nullptr};
	int data;

	Linked_node(value_type val) : data{val} {}
	Linked_node(const Linked_node* node) : next{node->next}, data{node->data} {}

	value_type operator*() {return data;}

	Linked_node* insert(value_type val) {
		Linked_node* n = new Linked_node{val};
		n->next = next;
		next = n;
		return n;
	}

	Linked_node* append(value_type val) {
		// returns inserted node (so that it can become a relative head)
		Linked_node* end = new Linked_node{val};
		Linked_node* insert_at = this;
		while (insert_at->next) insert_at = insert_at->next;
		insert_at->next = end;
		return end;
	}
};

std::ostream& operator<<(std::ostream& os, const Linked_node* n) {
	if (!n) return os;
	return os << n->data << ' ';
}

struct Binary_node {
	Binary_node *left {nullptr}, *right {nullptr};
	int data;

	Binary_node() = default;
	Binary_node(int d) : data{d} {}
};