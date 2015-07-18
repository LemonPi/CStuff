#include <iostream>
#include <vector>
// #include "stupidstack.h"
// #include "stupidqueue.h"
// #include "stupidgraph.h"
// #include "stupidbits.h"
// #include "stupidoop.h"
#include "stupidrecursion.h"

using std::cout;
class Obj {
public:
	Obj() 			{cout << "default constructor\n";}
	Obj(const Obj&) {cout << "copy constructor\n";}
	Obj(Obj&&) 		{cout << "move constructor\n";}
	~Obj()			{cout << "destructor\n";}
};

void test_ownership() {
	std::vector<Obj> objs;
	objs.reserve(10);
	
	Obj l_value_obj;
	Obj l_value_obj_2;
	std::cout << "Emplacement ---------------------------\n";
	objs.emplace_back(std::move(l_value_obj));
	std::cout << "Emplacement without move --------------\n";
	objs.emplace_back(l_value_obj_2);
	std::cout << "Pushback without move -----------------\n";
	objs.push_back(l_value_obj_2);
}

int main() {
	// test_min_stack();
	// test_stack_set();
	// test_hanoi();
	// test_stack_queue();
	// test_binary_search_tree();
	// test_static_graph();
	// test_layer_nodes();
	// test_first_common_ancestor();
	// test_sum_tree();
	// test_dec_to_bin();
	// test_neighbour_values();
	// test_bitops_to_convert();
	// test_swap_parity_bits();
	// test_find_missing();
	// test_cards();
	// test_callcenter();
	// test_library();
	test_recursion();
	// test_ownership();
}