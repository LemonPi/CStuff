#include <string.h>
#include <stdio.h>

char* rev_str(char* str) {
	if (!str) return NULL;
	size_t str_length = strlen(str);
	char temp;
	for (size_t c = 0; c < str_length/2; ++c) {
		temp = str[c];
		str[c] = str[str_length - c - 1];
		str[str_length - c - 1] = temp;
		// std::swap(str[c], str[str_length - c - 1]);
	}
	return str;
}

int main(int argc, char** argv) {
	while (--argc) {
		char* str = rev_str(argv[argc]);
		printf("[%s]\n",str);
	}
}