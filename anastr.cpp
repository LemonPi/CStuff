#include <string>
#include <cstdio>

constexpr size_t charset_size(char c) {return 256;}

using std::string;
bool is_anagram(const string& a, const string& b) {
	if (a.size() != b.size()) return false;

	int char_offsets[charset_size(a[0])] = {0};
	for (size_t c = 0; c < a.size(); ++c) {
		char_offsets[a[c]] += 1;
		char_offsets[b[c]] -= 1;
	}

	// offset of 0 means a balance of equal number of that character
	for (int offset : char_offsets)
		if (offset != 0) return false;

	return true;
}

int main(int argc, char** argv) {
	while (--argc) {
		// still at least 2 non-program name arguments left
		if (argc - 1 > 0) {
			bool anagram = is_anagram(string(argv[argc]), string(argv[argc-1]));
			printf("%s and %s are anagrams: %s", argv[argc], argv[argc-1], anagram? "true" : "false");
		}
	}
}