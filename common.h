#pragma once
#include <sstream>
#include <iostream>
#include <string>
#ifndef GETINPUT
#define GETINPUT() \
	std::string line;\
	std::getline(std::cin, line);\
	std::stringstream ss {line};
#endif