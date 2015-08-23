#pragma once
#include "common.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <deque>
#include <string>

using std::string;
using std::vector;
vector<string> last_k_lines(const string& filename, size_t k) {
	std::ifstream file {filename};
	std::deque<string> lines;
	size_t estimated_bytes_per_line {100};
	// seeking optimization to only read the tail
	while (true) {	// haven't successfully read k lines yet
		file.seekg(-(estimated_bytes_per_line*k), file.end);
		size_t lines_left {k};
		string line;
		while (std::getline(file, line)) {
			// get k lines
			if (lines_left == 0) lines.pop_front();
			else --lines_left;
			lines.emplace_back(std::move(line));
		}
		if (!lines_left) break;
		estimated_bytes_per_line += 100;
	}

	vector<string> last_lines {lines.begin(), lines.end()};
	return last_lines;
}

struct Base {
	void func(int) 			{cout << "Base func(int)\n";}
	void func(int,int) 		{cout << "Base func(int,int)\n";}
	void not_overloaded()	{cout << "Base not_overloaded()\n";}
};
struct Derived : Base {
	void func(int)			{cout << "Derived func(int)\n";}
};

struct Virtual_base {
	virtual void func(int) 		{cout << "Virtual base func(int)\n";}
	virtual void func(int,int) 	{cout << "Virtual base func(int,int)\n";}
	virtual void not_overloaded(){cout << "Virtual base not_overloaded()\n";}
};
struct Virtual_derived : Virtual_base {
	void func(int)				{cout << "Virtual derived func(int)\n";}
};


class Traditional_base {
	// avoid virtual functions as API since they're fragile
public:
	virtual void do_simple(Base& b) {};
	virtual void do_complex(Base& b) {};
};

class Modern_base {
public:
	// simple pass through non-virtual interface that separates interface from implementation
	void do_simple(Base& b) {do_simple_step1(b);}
	void do_complex(Base& b) {complex_step(b); complex_finish();}
private:
	// can have more relationships in implementation (such as complex_finish always after complex_step) without
	// adding complexity to interface
	virtual void do_simple_step1(Base& b) {}
	virtual void complex_step(Base& b) {}
	virtual void complex_finish() {}
};

int sum_intfile(const std::string& filename) {
	std::ifstream is {filename};
	int sum {0}, cur {0};
	while (is >> cur) sum += cur;
	return sum;
}

struct RGB {
	unsigned char r,g,b;
	RGB() = default;
	template <typename T>
	RGB(T rr, T gg, T bb) : r{(unsigned char)rr}, g{(unsigned char)gg}, b{(unsigned char)bb} {}
};
template <typename T>
string tohex(T num) {
	std::stringstream ss;
	ss << std::setfill('0') << std::setw(sizeof(T)*2) << std::hex << (int)num;
	return ss.str();
}
string rgb2hex(RGB rgb) {
	return tohex(rgb.r) + tohex(rgb.g) + tohex(rgb.b);
}

// test the first byte of 0001 - bit endian has [0] on 0 while little endian has [0] on 1
bool using_little_endian() {
	short test = 0x01;
	char* bytes = reinterpret_cast<char*>(&test);
	return bytes[0];
}

class Obj {
public:
	Obj() 			{cout << "default constructor\n";}
	Obj(const Obj&) {cout << "copy constructor\n";}
	Obj(Obj&&) 		{cout << "move constructor\n";}
	~Obj()			{cout << "destructor\n";}
};



struct Teststruct {
	float a,b,c;
	// void* a;
	// void* c;
	// short b;
};
struct Teststruct2 {
	float a,b,c,d;
};

void test_unsigned_arithmetic() {
	int a {-5};
	size_t b {0}; 
	if ((size_t)a >= b) cout << "-5 >= 0u (comparing signed to unsigned)\n";
}

void test_last_k_lines() {
	GETINPUT();
	string filename;
	size_t k;
	ss >> filename >> k;
	auto last_lines = last_k_lines(filename, k);
	println(last_lines);
}

void test_name_hiding() {
	Derived d;
	d.func(1);
	d.not_overloaded();	// calls base method
	// d.func(1,1); // does not work because function not virtual

	Virtual_derived vd;
	vd.func(1);
	vd.not_overloaded();
	// vd.func(1,1);	// name hidden
}

void test_sum_intfile() {
	GETINPUT();
	string filename;
	ss >> filename;
	cout << sum_intfile(filename) << endl;
}

void test_rgb2hex() {
	GETINPUT();
	unsigned short r,g,b;
	ss >> r >> g >> b;
	cout << rgb2hex({r,g,b}) << endl;
}

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

void test_sizes() {
	cout << std::setw(12) << "bool: " << sizeof(bool) << endl;
	cout << std::setw(12) << "char: " << sizeof(char) << endl;
	cout << std::setw(12) << "int: " << sizeof(int) << endl;
	cout << std::setw(12) << "size_t: " << sizeof(size_t) << endl;
	cout << std::setw(12) << "long: " << sizeof(long) << endl;
	cout << std::setw(12) << "long long: " << sizeof(long long) << endl;
	cout << std::setw(12) << "float: " << sizeof(float) << endl;
	cout << std::setw(12) << "double: " << sizeof(double) << endl;
	cout << std::setw(12) << "3 floats: " << sizeof(Teststruct) << endl;
	cout << std::setw(12) << "4 floats: " << sizeof(Teststruct2) << endl;
}

void test_endianness() {
	if (using_little_endian()) cout << "little endian\n";
	else cout << "bit endian\n";
}

void test_knowledge() {
	// test_unsigned_arithmetic();
	// test_last_k_lines();
	// test_name_hiding();
	// test_sum_intfile();
	// test_rgb2hex();
	// test_ownership();
	test_sizes();
	// test_endianness();
}