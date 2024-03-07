/*
	output.tmkp
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define MAX_TOKENS 512 // could probably estmate it better but works for now

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

TokenContent tokens[MAX_TOKENS];
unsigned int output_width;
unsigned int output_lines;

void add_token(int *tokens_index, Token token, char *content) {
	tokens[*tokens_index].token = token;
	tokens[*tokens_index].content = (char *)malloc(sizeof(content));
	strcpy(tokens[*tokens_index].content, content);
	*tokens_index += 1;
	return;
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
		if (str_compare_at_index(content, i, "\n")) {
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
			i += 2;
		}
		if (temp_token != NEW_LINE && temp_token != DIVIDER) {
		char text_buffer[512];
		memset(text_buffer, 0, sizeof(text_buffer));
			int j = 0;
			if (content[i] == ' ') i++; 
			while (content[i+j] != '\n') {
				if (j > output_width) output_lines++;
				text_buffer[j] = content[i+j];
				j++;
			}
			add_token(&tokens_index, temp_token, text_buffer);
			i += strlen(text_buffer)-1;
		}
		//TODO: if ( token does not give a multiple line output)
		output_lines++;	
	}
}

unsigned int output_index = 0;

void append_to_string(char *dest, char *from) {
	output_index += strlen(from);
	strncat(dest, from, output_index);

	return;
}

char *cut_content_to_fit(char *content, char* before, char* after) {
	char *cut_output = malloc((strlen(content) + strlen(before) + strlen(after)) * sizeof(char));
	memset(cut_output, 0, strlen(cut_output) * sizeof(char));
	if (cut_output == NULL) {
		printf("%s memory allocation for cut_output failed.\n", ERROR_PRINT);
		return NULL;
	}
	
	cut_output[0] = '\0';

	unsigned int cut_output_index = 0;

	//strcpy(cut_output, before);
	//cut_output_index += strlen(before);

	printf("%s content=%s\n", DEBUG_PRINT, content);

//	printf("%s content+after=%lu\n", DEBUG_PRINT, output_width-((sizeof(content) + sizeof(after)) / sizeof(char)));
	printf("%s fmax=%d\n", DEBUG_PRINT, (int)fmax(0, (output_width + strlen(before))-(strlen(content) + strlen(after))));

	for (int i = 1; i < (int)fmax(0, (output_width - strlen(before))-(strlen(content) + strlen(after))); i++) {
		content[strlen(content)-1] = '\0';
		printf("%s 0\n", DEBUG_PRINT);
	}
	content[strlen(content)] = '\0';

	//strncat(cut_output, content, cut_output_index);
	//cut_output_index += strlen(content);
	//strncat(cut_output, after, cut_output_index);
	//cut_output_index += strlen(after);

	//strncat(cut_output, "\0", cut_output_index+1);
	
	sprintf(cut_output, "%s%s%s", before, content, after);

	return cut_output;
}

char *generate_output() {
	char *output = malloc(sizeof(char) * output_width * output_lines);
	if (output == NULL) {
		printf("%s memory allocation for output failed.\n", ERROR_PRINT);
		return NULL;
	}
	
	for (int i = 0; i < MAX_TOKENS; i++) {
		if (tokens[i].content == NULL) break;
		if (tokens[i].token == NEW_LINE) append_to_string(output, "\n");
		else if (tokens[i].token == HEADING_1) {
			char *fit_content =  cut_content_to_fit(tokens[i].content, "*- ", " -*");
			append_to_string(output, fit_content);
			free(fit_content);
		}
		else if (tokens[i].token == HEADING_2) {
			append_to_string(output, "**- ");
			append_to_string(output, tokens[i].content);
			append_to_string(output, " -**");
		}
		else if (tokens[i].token == HEADING_3) {
			append_to_string(output, "***- ");
			append_to_string(output, tokens[i].content);
			append_to_string(output, " -***");
		}
		else if (tokens[i].token == SIDE_ARROW) {
			append_to_string(output, "╰ ");
			append_to_string(output, tokens[i].content);
		}
		else if (tokens[i].token == DIVIDER) {
			char div[output_width];
			memset(div, 45, output_width); // 45 = '-'
			append_to_string(output, div);
		}
		else {
			append_to_string(output, tokens[i].content);
		}
	}
	output[output_index] = '\0';
	return output;
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
	
	if (sscanf(argv[3], "%d", &output_width) != 1) {
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
	size_t input_file_size = ftell(input_file);
	fseek(input_file, 0, SEEK_SET);

	char *input_file_content = (char *)malloc(input_file_size + 1);
	if (input_file_content == NULL) {
		fclose(input_file);
		printf("%s failed to allocate memory for \"input_file_content\"\n", ERROR_PRINT);
	}
	input_file_content[input_file_size] = '\0';

	size_t input_file_read_size = fread(input_file_content, 1, input_file_size, input_file);
	if (input_file_read_size != input_file_size) {
		free(input_file_content);
		fclose(input_file);
		printf("%s failed to read input file", ERROR_PRINT);
		return -1;
	}
	fclose(input_file);

	tokenize(input_file_content, input_file_size);
	free(input_file_content);
	
	dev_print_tokens();

	char *output = generate_output();

	if (output != NULL) {
		printf("\n%s OUTPUT \n\n%s", DEBUG_PRINT, output);
        	free((void*)output);
    	} else {
        	printf("Error: Memory allocation failed\n");
    	}
	
	return 0;
}




