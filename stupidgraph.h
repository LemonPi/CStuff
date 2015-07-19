#pragma once
#include <iostream>
#include <type_traits>
#include <vector>
#include <list>
#include "node.h"

using N = Binary_node;

int min_height(const N* root) {
	if (!root) return 0;
	return 1 + std::min(min_height(root->left), min_height(root->right));
}
int max_height(const N* root) {
	if (!root) return 0;
	return 1 + std::max(max_height(root->left), min_height(root->right));
}
bool is_balanced(const N* root) {
	return (max_height(root) - min_height(root) <= 1);
}

template <typename Nd, typename Op>
void inorder_walk(Nd* root, Op op) {
	if (root) {
		inorder_walk(root->left, op);
		op(root);
		inorder_walk(root->right, op);
	}
}

class BST {
	N* root;
	N** tree_find(N** start, int key) {
		while (*start && (*start)->data != key) {
			if (key < (*start)->data) start = &(*start)->left;
			else start = &(*start)->right;
		}
		// either the location of found node, or where node should be
		return start;
	}
public:
	BST() : root{nullptr} {};
	void insert(int data) {
		if (!root) {root = new N {data}; return;}
		N** to_ins = tree_find(&root, data);
		// item doesn't already exist
		if (*to_ins == nullptr)
			*to_ins = new N {data};

	}
	N* find(int data) {
		N** to_find = tree_find(&root, data);
		return *to_find;
	}

	void print() const {
		inorder_walk(root, [](const N* n){std::cout << n->data << ' ';});
		std::cout << '\n';
	}

	N* get_root() {return root;}
};

using Layers = std::vector<std::list<N*>>;
void layer_nodes_recurse(N* root, size_t layer, Layers& layers);
Layers layer_nodes(N* root) {
	if (!root) return {};
	Layers layers {std::list<N*> {root}};	
	layer_nodes_recurse(root->left, 1, layers);
	layer_nodes_recurse(root->right, 1, layers);
	return layers;
}

void layer_nodes_recurse(N* root, size_t layer, Layers& layers) {
	if (!root) return;
	if (layer >= layers.size()) layers.resize(layer + 1);	// empty construct lists
	layers[layer].push_back(root);
	layer_nodes_recurse(root->left, layer + 1, layers);
	layer_nodes_recurse(root->right, layer + 1, layers);
}

Layers layer_nodes_iter(N* root) {
	if (!root) return {};
	Layers layers {std::list<N*> {root}};
	size_t level {0};
	while (true) {
		std::list<N*> layer;
		for (N* parent : layers[level]) {	// for every parent in previous layer
			if (parent->left) layer.push_back(parent->left);
			if (parent->right) layer.push_back(parent->right);
		}
		// nothing on this layer
		if (layer.empty()) break;
		layers.push_back(layer);
		++level;
	}
	return layers;
}

template <typename Node>
Node* tree_min(Node* node) {
	// go left as far as you can
	if (!node) return nullptr;
	while (node->left)
		node = node->left;
	return node;
}

template <typename Node>
bool is_left_child(Node* node) {
	return node == node->parent->left;
}

template <typename Node>
Node* tree_successor(Node* node) {
	if (!node) return nullptr;
	if (node->right) return tree_min(node->right);
	// keep going up tree until a node that is a left child (meaning its parent is larger)
	while (node->parent && !is_left_child(node))
		node = node->parent;
	return node->parent;
}

// touches every node rooted at root once, O(n) time and space (stack might get large)
template <typename Node>
Node* first_common_ancestor(Node* root, Node* a, Node* b) {
	if (!root) return nullptr;
	if (root == a || root == b) return root;
	Node* left {first_common_ancestor(root->left, a, b)};
	Node* right {first_common_ancestor(root->right, a, b)};
	// found nothing
	if (!left && !right) return nullptr;
	// by elimination found something on the other side
	if (!left) return right;
	if (!right) return left;
	// else both found something (must be different)
	return root;
}

template <typename Node>
void sum_print(Node* root, int sum_tot, int sum_left) {
	std::cout << root->data;
	// did an entire sum finish
	if (sum_tot == sum_left) std::cout << '\n';
	// continuation of path
	else std::cout << " <- ";
}

template <typename Node>
bool sum_tree(Node* root, int sum_tot, int sum_left) {
	if (!root) return false;	// unfinished sum
	bool sum_found {false};
	// don't stop after finding the value (sum_left == 0) since there could be negative vals
	if (sum_left == root->data) {
		sum_print(root, sum_tot, sum_left);
		sum_found = true;
	}
	if (sum_tree(root->left, sum_tot, sum_left - root->data) ||
		 sum_tree(root->right, sum_tot, sum_left - root->data)) {
		sum_print(root, sum_tot, sum_left);
		sum_found = true;
	}
	return sum_found;
}
template <typename Node>
bool sum_tree(Node* root, int sum_tot) {
	// initial calls made without 3rd argument will start at sum_tot
	return sum_tree(root, sum_tot, sum_tot);
}
// O(n lgn) time (turns out each sum attempt takes only lgn time) O(1) space
template <typename Node>
void sum_tree_all(Node* root, int sum_tot) {
	// try summing to sum_tot from every node in order
	if (root) {
		sum_tree_all(root->left, sum_tot);
		sum_tree(root, sum_tot);
		sum_tree_all(root->right, sum_tot);
	}
}

template <typename Node>
bool is_subtree(Node* tree, Node* subtree) {
	// tree supposedly includes subtree
	if (!tree || !subtree) return false;	// nullptr for subtree is invalid
	// find subtree's root in tree
	Node* tree_subtree_start = tree_find(tree, subtree->data);
	if (!tree_subtree_start) return false;
	return traverse_match(tree_subtree_start, subtree);
}
void test_binary_search_tree() {
	BST tree;
	int data[] {5, 3, 4, 2, 7, 6, 1, 8, 5};
	for (int datum : data) tree.insert(datum);

	tree.print();
}

void print_layers(const Layers& layers) {
	for (size_t layer = 0; layer < layers.size(); ++layer) {
		std::cout << layer << '(' << layers[layer].size() << ") :";
		for (N* node : layers[layer])
			std::cout << node->data << ' ';
		std::cout << '\n';
	}	
}

void test_layer_nodes() {
	BST tree;
	int data[] {5, 3, 4, 2, 7, 6, 1, 8, 5};
	for (int datum : data) tree.insert(datum);

	auto layers = layer_nodes_iter(tree.get_root());
	print_layers(layers);
}

void test_first_common_ancestor() {
	BST tree;
	int data[] {7,3,8,1,4,9,2,6,5};
	for (int datum : data) tree.insert(datum);
	print_layers(layer_nodes_iter(tree.get_root()));

	N* a {tree.find(2)};
	N* b {tree.find(9)};
	N* ancestor {first_common_ancestor(tree.get_root(), a, b)};
	std::cout << ancestor->data << std::endl;
}

void test_sum_tree() {
	BST tree;
	int data[] {10, 6,15, 4,8,13,18, 2,7,12,16,20, 1};
	for (int datum : data) tree.insert(datum);
	print_layers(layer_nodes_iter(tree.get_root()));
	

	sum_tree_all(tree.get_root(), 25);
}

class Static_graph {
public:
	// POD
	struct Edge {
		int dest;	// index of destination vertex
		int weight;
		Edge() = default;
		Edge(int d, int w) : dest{d}, weight{w} {}
	};
private:
	using Adj = std::vector<std::vector<Edge>>;
	Adj adj;	// adjacency list
public:
	Static_graph() = default;
	explicit Static_graph(size_t num_vertices) : adj(num_vertices) {}
	template <typename Iter>
	struct Adj_iter {
		using reference = Adj_iter<Iter>&;
		using const_reference = const Adj_iter<Iter>&;
		Iter edge;

		reference operator++() {++edge; return *this;}
		reference operator--() {--edge; return *this;}
		bool operator==(const_reference other) {return other.edge == edge;}
		bool operator!=(const_reference other) {return other.edge != edge;}
		// return destination index on *
		int operator*() {return edge->dest;}
		int weight() {return edge->weight;}
	};
	template <typename Iter>
	struct V_iter {
		using reference = V_iter<Iter>&;
		using const_reference = const V_iter<Iter>&;
		using iterator = typename Iter::value_type::iterator;
		using const_iterator = typename Iter::value_type::const_iterator;
		Iter vertex;

		reference operator++() {++vertex; return *this;}
		reference operator--() {--vertex; return *this;}
		size_t operator+(const_reference other) {return other.vertex + vertex;}
		size_t operator-(const_reference other) {return vertex - other.vertex;}
		bool operator==(const_reference other) {return other.vertex == vertex;}
		bool operator!=(const_reference other) {return other.vertex != vertex;}
		// return source vertex index on * (need to diff with adj.begin())
		Iter operator*() {return vertex;}
		Adj_iter<iterator> begin() {return {vertex->begin()};}
		Adj_iter<iterator> end() {return {vertex->end()};}
	};

	using iterator = 				V_iter<Adj::iterator>;
	using const_iterator = 			V_iter<Adj::const_iterator>;
	using adjacent_iterator = 		Adj_iter<iterator::iterator>;
	using const_adjacent_iterator = Adj_iter<const_iterator::const_iterator>;
	// vertex iteration
	iterator begin() {return {adj.begin()};}
	iterator end() {return {adj.end()};}
	adjacent_iterator begin(size_t vertex) {return {adj[vertex].begin()};}
	adjacent_iterator end(size_t vertex) {return {adj[vertex].end()};}
	const_adjacent_iterator begin(size_t vertex) const {return {adj[vertex].begin()};}
	const_adjacent_iterator end(size_t vertex)   const {return {adj[vertex].end()};}

	bool empty() const 				{return adj.empty();}
	size_t vertex_number() const 	{return adj.size();}
	size_t size() const 			{return adj.size();}
	size_t edge_number() const {
		size_t edges;
		for (const auto& v : adj)
			for (const auto& u : v)
				++edges;
		return edges;
	}

	// modification
	size_t add_node() {
		adj.emplace_back();
		return adj.size() - 2;
	}

	// directed and assuming vertices exist
	void add_edge(size_t source, size_t dest, int weight = 1) {
		adj[source].emplace_back(dest, weight);
	}

	void print() {
		for (size_t v = 0; v < vertex_number(); ++v) {
			std::cout << v << ": ";
			for (auto u = begin(v); u != end(v); ++u) {
				std::cout << "(" << *u << "," << u.weight() << ") " ;
			}
			std::cout << '\n';		
		}
	}
};

enum class DFS : char {
	UNEXPLORED = 0, EXPLORING, EXPLORED
};

constexpr int no_parent = -1;
struct Static_vertex_property {
	int parent;	// index, = no_parent if no parent
	DFS status;
	Static_vertex_property() : parent{no_parent}, status{DFS::UNEXPLORED} {}
	Static_vertex_property(int p) : parent{p}, status{DFS::UNEXPLORED} {}
};
// vertex index corresponds to entry in property map
using Static_property_map = std::vector<Static_vertex_property>;

// single source depth first search from source s in graph g
Static_property_map dfs(const Static_graph& g, int s) {
	// default initialize property vertices
	Static_property_map property(g.vertex_number());	
	property[s].status = DFS::EXPLORING;

	int parent {-1};
	std::stack<int> exploring;
	exploring.push(s);

	while (!exploring.empty()) {
		int u = exploring.top(); exploring.pop();
		property[u].parent = parent;

		for (auto v_i = g.begin(u); v_i != g.end(u); ++v_i) {
			int v = *v_i;
			if (property[v].status == DFS::UNEXPLORED) {
				exploring.push(v);
				property[v].status = DFS::EXPLORING;
				property[v].parent = u;
			}
		}

		property[u].status = DFS::EXPLORED;
		parent = u;
	}

	return property;
}

void test_static_graph() {
	Static_graph g {7};	// 7 vertices
	g.add_edge(0, 1);
	g.add_edge(0, 2);
	g.add_edge(0, 3);
	g.add_edge(1, 0);
	g.add_edge(1, 4);
	g.add_edge(2, 1);
	g.add_edge(3, 5);
	g.add_edge(4, 2);
	g.add_edge(5, 2);
	g.add_edge(5, 6);
	g.print();

	int source = 4, sink = 6;
	auto property = dfs(g, source);
	if (property[sink].parent != no_parent) {
		std::cout << sink;
		while (true) {
			if (property[sink].parent != no_parent) {
				sink = property[sink].parent;
				std::cout << " <- " << sink;
			}
			else break;
		}
		std::cout << std::endl;
	}
	else
		std::cout << sink << " could not be reached from " << source << std::endl;
}

void test_graph() {
	// test_binary_search_tree();
	// test_static_graph();
	// test_layer_nodes();
	// test_first_common_ancestor();
	// test_sum_tree();	
}