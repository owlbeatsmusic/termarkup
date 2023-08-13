/*
	output.tmkp
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
	tokens[*tokens_index].token = token;
	tokens[*tokens_index].content = (char *)malloc(sizeof(content));
	strcpy(tokens[*tokens_index].content, content);
	*tokens_index += 1;
}

int str_compare_at_index(char *content, int index, char* compare) {
	for (int i = 0; i < sizeof(compare)-1; i++) {
		printf("%c == %c : %d\n", content[i+index], compare[i], index);
		if (content[index+i] != compare[i]) return 0; 
	}
	printf("true1\n");
	return 1;
}

void tokenize(char *content, int file_size) {
	int tokens_index = 0;
	for (int i = 0; i < file_size; i++) {
		if (content[i] == '\n') {
			add_token(&tokens_index, NEW_LINE, " ");
		}
		else if (str_compare_at_index(content, i, "*!") && content[i+2] != '!') {
			printf("true2\n");
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
		printf("\n\x1B[2m\t            string       string        int\x1B[0m\n");
		printf("\t./termarkup [input_file] [output_file] [file_width]\n\n");
		return 0;
	}
	char *input_file_path  = argv[1];
	char *output_file_path = argv[2];
	int width;
	if (sscanf(argv[3], "%d", &width) != 1) {
		error_prefix_print();
		printf("could not convert third argument to int (use -help)\n");
		return -1;
	}

	
	// reading the file
	FILE *input_file = fopen(input_file_path, "r");
	if (input_file == NULL) {
		error_prefix_print();
		printf("could not open input file(%s)\n", input_file_path);
		return -1;
	}
	fseek(input_file, 0, SEEK_END);
	int input_file_size = ftell(input_file);
	fseek(input_file, 0, SEEK_SET);

	char *input_file_content = (char *)malloc(input_file_size + 1);
	if (input_file_content == NULL) {
		error_prefix_print();
		printf("failed to allocate memory for \"input_file_content\"\n");
	}
	input_file_content[input_file_size] = '\0';

	size_t input_file_read_size = fread(input_file_content, 1, input_file_size, input_file);
	if (input_file_read_size != input_file_size) {
		free(input_file_content);
		error_prefix_print();
		printf("failed to read input file");
		return -1;
	}


	tokenize(input_file_content, input_file_size);
	dev_print_tokens();

	free(input_file_content);
}




