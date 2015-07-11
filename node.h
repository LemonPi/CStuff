#pragma once
#include <iostream>
struct Linked_node {
	using value_type = int;

	Linked_node* next {nullptr};
	int data;

	Linked_node(value_type val) : data{val} {}
	value_type operator*() {return data;}

	Linked_node* insert(value_type val) {
		Linked_node* next = new Linked_node{val};
		return next;
	}

	Linked_node* append(value_type val) {
		Linked_node* end = new Linked_node{val};
		Linked_node* insert_at = this;
		while (insert_at->next) insert_at = insert_at->next;
		insert_at->next = end;
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