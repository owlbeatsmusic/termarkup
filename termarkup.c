/*
	input.tmkp
	

*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define TOKENS_SIZE 512 // could probably estmate it better but works for now


// "tool" funtions
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

	NEW_LINE,
	END_FILE,
} Token;

typedef struct {
	Token token;
	char *content;
} TokenContent;

TokenContent tokens[TOKENS_SIZE];

void add_token(int *tokens_index, Token token, char *content) {
	tokens[tokens_index].token = token;
	tokens[tokens_index].content = (char *)malloc(sizeof(content));
	strcpy(tokens[tokens_index].content, content);
	tokens_index++;
}

void tokenize(char *file_content, int file_size) {
	int tokens_index = 0;
	for (int i = 0; i < file_size; i++) {
		if (file_content[i] == '\n') {
			add_token(&tokens_index, NEW_LINE, " ");
		}
	}
}

void dev_print_tokens() {
	char *token_names[] = {
		"HEADING_1",
		"HEADING_2",
		"HEADING_3",
		"SIZE_ARROW",
		"DVIIDER",
		"CALLOUT",

		"NEW_LINE",
		"END_FILE"
	};
	printf("[");
	for (int i = 0; i < sizeof(tokens) / sizeof(TokenContent); i++) {
		if (tokens[i].content == NULL) continue;
		printf("[%s, %s], ", token_names[tokens[i].token], tokens[i].content);
	}
	printf("]\n");
}

void write_formatted() {

}

int main(int argc, char *argv[]) {
	if (!strcmp(argv[1], "-help")) {
		printf("\n\x1B[2m\t            string      int\x1B[0m\n");
		printf("\t./termarkup [file_path] [file_width]\n\n");
		return 0;
	}
	char *file_path = argv[1];
	int width;
	if (sscanf(argv[2], "%d", &width) != 1) {
		error_prefix_print();
		printf("could not convert second argument to integer\n");
		return -1;
	}
	
	// reading the file
	FILE *file = fopen(file_path, "r");
	if (file == NULL) {
		error_prefix_print();
		printf("could not open file(%s)\n", file_path);
		return -1;
	}
	fseek(file, 0, SEEK_END);
	int file_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	char *file_content = (char *)malloc(file_size + 1);
	if (file_content == NULL) {
		error_prefix_print();
		printf("failed to allocate memory for \"file_content\"\n");
	}
	file_content[file_size] = '\0';

	size_t read_size = fread(file_content, 1, file_size, file);
	if (read_size != file_size) {
		free(file_content);
		error_prefix_print();
		printf("failed to read file");
		return -1;
	}

	tokenize(file_content, file_size);
	dev_print_tokens();

	free(file_content);
}




