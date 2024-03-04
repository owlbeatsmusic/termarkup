/*
	output.tmkp
*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define TOKENS_SIZE 512 // could probably estmate it better but works for now

#define MSG_PRINT   "[\033[0;33mtmpk\033[0m]:"
#define ERROR_PRINT "[\033[0;31merror\033[0m]:"
#define DEBUG_PRINT "[\033[0;35mdebug\033[0m]:"


typedef enum {
	HEADING_1,
	HEADING_2,
	HEADING_3,
	SIDE_ARROW,
	DIVIDER,
	CALLOUT,
	
	TEXT,

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
	for (int i = 0; i < strlen(compare); i++) {
		if (content[index+i] != compare[i]) return 0; 
	}
	return 1;
}

void tokenize(char *content, int file_size) {
	int tokens_index = 0;
	for (int i = 0; i < file_size; i++) {
		Token temp_token = TEXT;
		if (content[i] == '\n') {
			add_token(&tokens_index, NEW_LINE, " ");
			temp_token = NEW_LINE;
		}
		else if (str_compare_at_index(content, i, "*!") && content[i+2] != '!') {
			temp_token = HEADING_1;
			i += 2;		
		} 
		else if (str_compare_at_index(content, i, "*!!") && content[i+3] != '!') {
			temp_token = HEADING_2;
			i += 3;	
		}
		else if (str_compare_at_index(content, i, "*!!!") && content[i+4] != '!') {
			temp_token = HEADING_3;
			i += 4;
		}
		else if (str_compare_at_index(content, i, "+-")) {
			temp_token = SIDE_ARROW;
			i += 2;
			
		}
		else if (str_compare_at_index(content, i, "---")) {
			add_token(&tokens_index, DIVIDER, " ");
			temp_token = DIVIDER;
			i += 3;
		}
		if (temp_token != NEW_LINE && temp_token != DIVIDER) {
		char text_buffer[512];
		memset(text_buffer, 0, sizeof(text_buffer));
			int j = -1;
			if (content[i] == ' ') i++; 
			while (content[i+j] != '\n') {
				text_buffer[j] = content[i+j];
				j++;
			}
			add_token(&tokens_index, temp_token, text_buffer);
			i += strlen(text_buffer);
		}
		
	}
}

void dev_print_tokens() {
	// same order as defined in enum
	char *token_names[] = {
		"HEADING_1",
		"HEADING_2",
		"HEADING_3",
		"SIDE_ARROW",
		"DVIIDER",
		"CALLOUT",

		"TEXT",

		"NEW_LINE",
		"END_FILE"
	};
	printf("[");
	for (int i = 0; i < sizeof(tokens) / sizeof(TokenContent); i++) {
		if (tokens[i].content == NULL) continue;
		printf("[%s, %s], \n", token_names[tokens[i].token], tokens[i].content);
	}
	printf("]\n");
}

void write_formatted() {
	
}

int main(int argc, char *argv[]) {
	if (argc < 4) {
		printf("\n\x1B[2m\t            string       string        int\x1B[0m\n");
		printf("\t./termarkup [input_file] [output_file] [file_width]\n\n");
		return 0;
	}
	if (!strcmp(argv[1], "-help")) {
		printf("\n\x1B[2m\t            string       string        int\x1B[0m\n");
		printf("\t./termarkup [input_file] [output_file] [file_width]\n\n");
		return 0;
	}
	char *input_file_path  = argv[1];
	char *output_file_path = argv[2];
	
	int width;
	if (sscanf(argv[3], "%d", &width) != 1) {
		printf("%s could not convert third argument to int (use -help)\n", ERROR_PRINT);
		return -1;
	}
	
	// reading the file
	FILE *input_file = fopen(input_file_path, "r");
	if (input_file == NULL) {
		printf("%s could not open input file(%s)\n", ERROR_PRINT, input_file_path);
		return -1;
	}
	fseek(input_file, 0, SEEK_END);
	int input_file_size = ftell(input_file);
	fseek(input_file, 0, SEEK_SET);

	char *input_file_content = (char *)malloc(input_file_size + 1);
	if (input_file_content == NULL) {
		printf("%s failed to allocate memory for \"input_file_content\"\n", ERROR_PRINT);
	}
	input_file_content[input_file_size] = '\0';

	size_t input_file_read_size = fread(input_file_content, 1, input_file_size, input_file);
	if (input_file_read_size != input_file_size) {
		free(input_file_content);
		printf("%s failed to read input file", ERROR_PRINT);
		return -1;
	}

	tokenize(input_file_content, input_file_size);
	dev_print_tokens();

	free(input_file_content);
}




