#pragma once
#include <sstream>
#include <iostream>
#include <string>


using std::cin;
using std::cout;
using std::endl;

#ifndef GETINPUT
#define GETINPUT() \
	std::string line;\
	std::getline(std::cin, line);\
	std::stringstream ss {line};
#endif

template <typename Sequence>
void print(const Sequence& v, std::ostream& os = std::cout) {
    for (auto x : v)
        os << x << ' ';
    os << '\n';
}
