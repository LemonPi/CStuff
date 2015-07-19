#include <iostream>
#include <vector>
// #include "stupidstack.h"
// #include "stupidqueue.h"
// #include "stupidgraph.h"
// #include "stupidbits.h"
// #include "stupidoop.h"
// #include "stupidrecursion.h"
// #include "stupidsorts.h"
#include "stupidmaths.h"

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
	// test_graph();
	// test_bits();
	// test_oop();
	// test_recursion();
	// test_ownership();
	// test_sorts();
	test_maths();
}