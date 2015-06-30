#include <cstdio>
#include <cstring>
#include <string>

constexpr size_t charset_size(char c) {return 256;}

char* dup_rem_c(char* str) {
	if (!str) return NULL;
	size_t insert_at = 0;
	bool existing_char[charset_size(str[0])] = {0};	// initialize to false
	for (size_t look_at = 0; look_at < strlen(str); ++look_at) {
		char cur_c = str[look_at];
		if (!existing_char[cur_c]) {
			existing_char[cur_c] = true;
			str[insert_at++] = cur_c;
		}
	}
	// invariant is insert_at is the index for next insertion
	str[insert_at] = '\0';
	// should I retrun the length of the stripped string instead?
	return str;
}


using std::string;
string& dup_rem(string& str) {
	if (str.empty()) return str;
	size_t insert_at = 0;
	bool existing_char[256] = {0};	// initialize to false
	for (size_t look_at = 0; look_at < str.size(); ++look_at) {
		char cur_c = str[look_at];
		if (!existing_char[cur_c]) {
			existing_char[cur_c] = true;
			str[insert_at++] = cur_c;
		}
	}
	// invariant is insert_at is the index for next insertion
	str[insert_at] = '\0';

	// optional, use if optimizing for memory, don't use if optimizing for speed
	#ifdef OPTIMIZE_MEM
		str.resize(insert_at);
	#endif

	// should I retrun the length of the stripped string instead?
	return str;
}

int main(int argc, char** argv) {
	char str2[20];
	while (--argc) {
		// char* str = dup_rem_c(argv[argc]);
		strcpy(str2,argv[argc]);
		dup_rem_c(str2);
		// string str = dup_rem(string(argv[argc]));
		// printf("[%s]\n",str);
		printf("[%s]\n",str2);
	}	
}