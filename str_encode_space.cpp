#include <string>
#include <cstdio>

using std::string;
const string& str_encode_space(string& str) {
	size_t orig_len = str.size();
	size_t num_spaces = 0;
	for (char c : str)
		if (c == ' ') ++num_spaces;

	size_t req_len = orig_len + num_spaces * 2;
	str.resize(req_len);

	// fill in string from back
	// int index used to better deal with decrement and with orig_len being 0
	for (int c = (int)orig_len - 1; c >= 0; --c) {
		if (str[c] == ' ') {
			str[--req_len] = '0';
			str[--req_len] = '2';
			str[--req_len] = '%';
			// prefix decrement since originally length as index is 1 past end
		}
		else
			str[--req_len] = str[c];
	}
	return str;
}

int main(int argc, char** argv) {
	while (--argc) {
		string str {argv[argc]};
		str_encode_space(str);		
		printf("before:  %s\nencoded: %s", argv[argc], str.c_str());
	}
}