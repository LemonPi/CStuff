#include <vector>
#include <unordered_map>
#include <iomanip>
#include "sal/data/vector.h"
#include "sal/algo/utility.h"
#include "common.h"

using sal::Timer;
using std::setw;
constexpr int test_size = 10000000;

// higher level general functions ------------------
template <typename Indexable>
void init_indexable_push_back(Indexable& table) {
	table.reserve(test_size);
	using T = typename Indexable::value_type;
	for (int i = 0; i < test_size; ++i)
		table.push_back((T)i);
}
template <typename Indexable>
void init_indexable_assign(Indexable& table) {
	table.resize(test_size);
	using T = typename Indexable::value_type;
	for (int i = 0; i < test_size; ++i)
		table[i] = (T)i;
}

template <typename Indexable>
void profile_read(const Indexable& table) {
	using T = typename Indexable::value_type;
	Timer time;
	T sum {0};
	for (int i = 0; i < test_size; ++i)
		sum += table[sal::randint(test_size-1)];
	cout << "random reads: " << time.tonow() / 1000.0 << endl;	
}
template <typename T>
void profile_read(const T* table) {
	Timer time;
	T sum {0};
	for (int i = 0; i < test_size; ++i)
		sum += table[sal::randint(test_size-1)];
	cout << "random reads: " << time.tonow() / 1000.0 << endl;	
}
template <typename T>
void profile_read(T* table) {
	profile_read(const_cast<const T*>(table));
}

template <typename Indexable>
void profile_write(Indexable&& table) {
	Timer time;
	for (int i = 0; i < test_size; ++i)
		table[sal::randint(test_size-1)] = i;
	cout << "random writes: " << time.tonow() / 1000.0 << endl;	
}

template <typename Indexable>
void profile_indexable(Indexable& table) {
	profile_read(table);
	profile_write(table);
}


// specific testing -------------------------------
void profile_unordered_map() {
	cout << "unordered map\n";
	std::unordered_map<int, int> table;
	Timer time;
	table.reserve(test_size);
	for (int i = 0; i < test_size; ++i)
		table[i] = i;
	cout << "initialization: " << time.tonow() / 1000.0 << endl;	
	// operator[] not const for hash tables (creates entry if not exists)
	time.restart();
	int sum {0};
	for (int i = 0; i < test_size; ++i)
		sum += table[sal::randint(test_size-1)];
	cout << "random reads: " << time.tonow() / 1000.0 << endl;	

	profile_write(table);
	cout << endl;
}
void profile_direct_access_table() {
	cout << "direct access table\n";
	std::vector<int> table;
	Timer time;
	table.reserve(test_size);
	for (int i = 0; i < test_size; ++i)
		table.push_back(i);
	cout << "initialization: " << time.tonow() / 1000.0 << endl;	

	profile_indexable(table);
	cout << endl;
}

int* g_lookup;
sal::Fixed_vector<int> g_fixed_vector_lookup;

void profile_global_lookup() {
	int sum {0};
	cout << "global lookup\n";
	Timer time;
	for (int i = 0; i < test_size; ++i)
		sum += g_lookup[sal::randint(test_size-1)];
	cout << "random reads: " << time.tonow() / 1000.0 << endl;	

	time.restart();
	for (int i = 0; i < test_size; ++i)
		g_lookup[sal::randint(test_size-1)] = i;
	cout << "random writes: " << time.tonow() / 1000.0 << endl;	
}

void profile_aliased_global_lookup() {
	cout << "aliased global lookup\n";
	profile_read(g_lookup);
	profile_write(g_lookup);
}

void profile_local_lookup(int* local_lookup) {
	cout << "local lookup\n";
	profile_read(local_lookup);
	profile_write(local_lookup);
}
void profile_const_local_lookup(const int* local_lookup) {
	cout << "const local lookup\n";
	profile_read(local_lookup);
}

void profile_global_fixed_vector_lookup() {
	int sum {0};
	cout << "global fixed vector lookup\n";
	Timer time;
	for (int i = 0; i < test_size; ++i)
		sum += g_fixed_vector_lookup[sal::randint(test_size-1)];
	cout << "random reads: " << time.tonow() / 1000.0 << endl;	

	time.restart();
	for (int i = 0; i < test_size; ++i)
		g_fixed_vector_lookup[sal::randint(test_size-1)] = i;
	cout << "random writes: " << time.tonow() / 1000.0 << endl;	
}

void profile_const_fixed_vector_lookup(const sal::Fixed_vector<int>& local_lookup) {
	cout << "const fixed vector lookup\n";
	profile_read(local_lookup);
}
void profile_fixed_vector_lookup(sal::Fixed_vector<int>& local_lookup) {
	cout << "fixed vector lookup\n";
	profile_read(local_lookup);
	profile_write(local_lookup);
}

void profile_lookup_speed() {
	int* local_lookup {new int[test_size]};
	g_lookup = {new int[test_size]};
	g_fixed_vector_lookup.resize(test_size);
	sal::Fixed_vector<int> fixedvec {test_size};
	for (int i = 0; i < test_size; ++i) {
		local_lookup[i] = g_lookup[i] = g_fixed_vector_lookup[i] = fixedvec[i] = i;
	}

	profile_global_lookup();
	cout << endl;
	profile_aliased_global_lookup();	// just as fast as the other const lookups
	cout << endl;
	profile_global_fixed_vector_lookup();	// not as fast as the const versions
	cout << endl;
	profile_const_local_lookup(local_lookup); // almost 2 times faster for reading than the other lookups!
	cout << endl;
	profile_local_lookup(local_lookup);
	cout << endl;
	profile_const_fixed_vector_lookup(fixedvec);
	cout << endl;
	profile_fixed_vector_lookup(fixedvec); // same read speed as const version since operator[] is overloaded for constness
	cout << endl;

	delete[] local_lookup;
	delete[] g_lookup;
}

using std::vector;

void profile_different_types() {
	vector<int> vecint;
	vector<long> veclong;
	vector<unsigned> vecunsigned;
	vector<short> vecshort;
	vector<char> vecchar;
	vector<size_t> vecsize;

	init_indexable_push_back(vecint);
	init_indexable_push_back(veclong);
	init_indexable_push_back(vecunsigned);
	init_indexable_push_back(vecshort);
	init_indexable_push_back(vecshort);
	init_indexable_push_back(vecchar);
	init_indexable_push_back(vecsize);
	
	cout << "int\n";
	profile_indexable(vecint);
	cout << endl;
	cout << "long\n";
	profile_indexable(veclong);
	cout << endl;
	cout << "unsigned\n";
	profile_indexable(vecunsigned);
	cout << endl;
	cout << "short\n";
	profile_indexable(vecshort);
	cout << endl;
	cout << "char\n";
	profile_indexable(vecchar);
	cout << endl;
	cout << "size_t\n";
	profile_indexable(vecsize);
	cout << endl;
}

int arg1(int a) {
	return a;
}
int arg2(int a, int b) {
	return a + b;
}
int arg3(int a, int b, int c) {
	return a + b + c;
}
int arg4(int a, int b, int c, int d) {
	return a + b + c + d;
}
int arg5(int a, int b, int c, int d, int e) {
	return a + b + c + d + e;
}
int arg6(int a, int b, int c, int d, int e, int f) {
	return a + b + c + d + e + f;
}
int arg7(int a, int b, int c, int d, int e, int f, int g) {
	return a + b + c + d + e + f + g;
}
int arg8(int a, int b, int c, int d, int e, int f, int g, int h) {
	return a + b + c + d + e + f + g + h;
}
int arg9(int a, int b, int c, int d, int e, int f, int g, int h, int i) {
	return a + b + c + d + e + f + g + h + i;
}
int arg10(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j) {
	return a + b + c + d + e + f + g + h + i + j;
}

void profile_func_call_overhead() {
	int sum {0};
	Timer time;
	cout << "1 arg  ";
	for (int i = 0; i < test_size; ++i) sum += arg1(i);
	cout << time.tonow() / 1000.0 << endl;

	time.restart();
	cout << "2 arg  ";
	for (int i = 0; i < test_size; ++i) sum += arg2(i, i);
	cout << time.tonow() / 1000.0 << endl;

	time.restart();
	cout << "3 arg  ";
	for (int i = 0; i < test_size; ++i) sum += arg3(i, i, i);
	cout << time.tonow() / 1000.0 << endl;

	time.restart();
	cout << "4 arg  ";
	for (int i = 0; i < test_size; ++i) sum += arg4(i, i, i, i);
	cout << time.tonow() / 1000.0 << endl;

	time.restart();
	cout << "5 arg  ";
	for (int i = 0; i < test_size; ++i) sum += arg5(i, i, i, i, i);
	cout << time.tonow() / 1000.0 << endl;

	time.restart();
	cout << "6 arg  ";
	for (int i = 0; i < test_size; ++i) sum += arg6(i, i, i, i, i, i);
	cout << time.tonow() / 1000.0 << endl;

	time.restart();
	cout << "7 arg  ";
	for (int i = 0; i < test_size; ++i) sum += arg7(i, i, i, i, i, i, i);
	cout << time.tonow() / 1000.0 << endl;

	time.restart();
	cout << "8 arg  ";
	for (int i = 0; i < test_size; ++i) sum += arg8(i, i, i, i, i, i, i, i);
	cout << time.tonow() / 1000.0 << endl;

	time.restart();
	cout << "9 arg  ";
	for (int i = 0; i < test_size; ++i) sum += arg9(i, i, i, i, i, i, i, i, i);
	cout << time.tonow() / 1000.0 << endl;

	time.restart();
	cout << "10 arg ";
	for (int i = 0; i < test_size; ++i) sum += arg10(i, i, i, i, i, i, i, i, i, i);
	cout << time.tonow() / 1000.0 << endl;

	cout << sum << endl;
}

void run_profile() {
	// direct access is about 4 times as fast for everything
	// profile_direct_access_table();
	// profile_unordered_map();

	// const is twice as fast for reading, global is default to non-const, but can alias with same performance
	// profile_lookup_speed();	

	// pretty much the same read and write time
	// profile_different_types();	

	// 1 arg is fastest, 2 arg take about 1.5 as much overhead, 3-4 take about 2 times, while 5 and beyond take 3 times penalty
	profile_func_call_overhead();
}