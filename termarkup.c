/*
	input.tmkp
	

*/
#include <stdio.h>

// tool funtions
void error_prefix_print() {
	printf("%s%serror%s: ", "\x1B[1m", "\x1B[31m", "\x1B[0m");
}

typedef enum {
	HEADING_1,
	HEADING_2,
	HEADING_3,
	SIDE_ARROW,
	DIVIDER,
	CALLOUT,

	END_LINE
} Token;

typedef struct {
	Token token;
	char *content;
} TokenContent;

void tokenize() {

}

void write_formatted() {

}

int main(int argc, char *argv[]) {
	char *file_path = argv[1];
	int width;
	if (sscanf(argv[2], "%d", &width) != 1) {
		error_prefix_print();
		printf("could not convert second argument to integer\n");
	}
}
