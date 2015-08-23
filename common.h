#pragma once
#include <sstream>
#include <iostream>
#include <string>


using std::cin;
using std::cout;
using std::cerr;
using std::endl;

#ifndef GETINPUT
#define GETINPUT() \
	std::string line;\
	std::getline(std::cin, line);\
	std::stringstream ss {line};
#endif

#ifndef GETANOTHER
#define GETANOTHER() \
	std::getline(std::cin, line);\
	ss.clear();\
	ss.str(line);
#endif


#ifdef DEBUG_BUILD
	#define DEBUG(x) {cerr << x << endl;}
#else
	#define DEBUG(x) {}
#endif

template <typename Sequence>
void print(const Sequence& v, std::ostream& os = std::cout) {
    for (auto x : v)
        os << x << ' ';
    os << '\n';
}

template <typename Sequence>
void println(const Sequence& v, std::ostream& os = std::cout) {
    for (auto x : v)
        os << x << '\n';
}