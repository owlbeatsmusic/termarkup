#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define MAX_TOKENS 512 // could probably estmate it better but works for now
#define MAX_WIDTH 256

#define DONE_PRINT   	"[\033[0;32mdone\033[0m]:"
#define WARNING_PRINT   "[\033[0;33mwarning\033[0m]:"
#define ERROR_PRINT 	"[\033[0;31merror\033[0m]:"
#define DEBUG_PRINT	"[\033[0;35mdebug\033[0m]:"

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

int non_ascii_found_boolean = 0;
unsigned int output_width;
unsigned int output_lines;
unsigned int output_index = 0;
char *output;

void add_token(int *tokens_index, Token token, char *content) {
	tokens[*tokens_index].token = token;
	tokens[*tokens_index].content = (char *)malloc(strlen(content)+1);
	strcpy(tokens[*tokens_index].content, content);
	tokens[*tokens_index].content[strlen(content)] = '\0';
	*tokens_index += 1;
	return;
}

int str_compare_at_index(char *content, int index, char* compare) {
	for (int i = 0; i < strlen(compare); i++) {
		if (content[index+i] != compare[i]) return 0; 
	}
	return 1;
}

void str_append_to_output(char *string) {
	output_index += strlen(string);
	strncat(output, string, output_index);

	return;
}

void tokenize(char *content, int file_size) {
	int tokens_index = 0;
	for (int i = 0; i < file_size; i++) {
		Token temp_token = TEXT;
		if (str_compare_at_index(content, i, "\n")) {
			add_token(&tokens_index, NEW_LINE, " ");
			temp_token = NEW_LINE;
		}
		else if (str_compare_at_index(content, i, "---")) {
			add_token(&tokens_index, DIVIDER, " ");
			temp_token = DIVIDER;
			i += 2;
		}
		else if (str_compare_at_index(content, i, "*-") && content[i+2] != '-') {
			temp_token = HEADING_1;
			i += 2;		
		} 
		else if (str_compare_at_index(content, i, "**-") && content[i+3] != '-') {
			temp_token = HEADING_2;
			i += 3;	
		}
		else if (str_compare_at_index(content, i, "***-") && content[i+4] != '-') {
			temp_token = HEADING_3;
			i += 4;
		}
		else if (str_compare_at_index(content, i, "+-")) {
			temp_token = SIDE_ARROW;
			i += 2;
			
		}
		else if (str_compare_at_index(content, i, "#")) {
			temp_token = CALLOUT;
			i += 1;
			
		}
		if (temp_token != NEW_LINE && temp_token != DIVIDER) {
			char text_buffer[MAX_WIDTH];
			memset(text_buffer, 0, sizeof(text_buffer));
			
			// create string content of a token
			int j = 0;
			while (content[i+j] != '\n') {
				// TODO: non ascii handling
				if (content[i+j] > 127 | content[i+j] < 0) { 
					non_ascii_found_boolean = 1;
					i++;
					continue;
				}
				if (j > output_width) output_lines++;
				text_buffer[j] = content[i+j];
				j++;
			}
			text_buffer[strlen(text_buffer)] = '\0';
			add_token(&tokens_index, temp_token, text_buffer);
			i += strlen(text_buffer)-1;
		}
		//TODO: if ( token does not give a multiple line output)
		output_lines++;	
	}
}

void format_token_to_fit(TokenContent *token, char *before, char *after, int non_ascii_offset, int multiple_lines_boolean, int fit_to_full_width) {

	int cut_output_size = strlen(token->content) + strlen(before) + strlen(after);
	char cut_output[cut_output_size];
	memset(cut_output, 0, cut_output_size);
	cut_output[0] = '\0';	
	
	// cut content to fit in width
	char token_content_copy[strlen(token->content)];
	strcpy(token_content_copy, token->content);
	int characters_to_cut = (strlen(before) + strlen(token->content) + strlen(after)+1) - output_width;	
	if (token->token == CALLOUT) characters_to_cut += 11 - 6*(1-multiple_lines_boolean);
	if (characters_to_cut > 0) {
		for (int i = 1; i < characters_to_cut-non_ascii_offset; i++) {
			token_content_copy[strlen(token_content_copy)-1] = '\0';
		}
	}
	token_content_copy[strlen(token_content_copy)]= '\0';
	
	// place "after" at the end of width if bool is true
	int padding_size = output_width-(strlen(before)+strlen(token_content_copy)+strlen(after));
	if (token->token == CALLOUT) padding_size -= 4;
	char *full_width_padding = "";
	if (padding_size > 0 & fit_to_full_width == 1) {
		full_width_padding = (char*)malloc(padding_size*sizeof(char));
		if (full_width_padding == NULL) {
			printf("%s memory allocation for full_width_padding failed.\n", ERROR_PRINT);
			return;
		}
		memset(full_width_padding, 32, padding_size*sizeof(char));
	}
	
	if (token->token == CALLOUT) 
		sprintf(cut_output, "%s %c | %s%s%s", before, token->content[0], token_content_copy+1, full_width_padding, after);
	else 
		sprintf(cut_output, "%s%s%s%s", before, token_content_copy, full_width_padding, after);
	str_append_to_output(cut_output);

	// recursion to add multiple lines if bool is true
	int lines = 0;
	if (multiple_lines_boolean == 1 & strlen(token_content_copy) != 0) {
		lines = ceil(strlen(token->content)/(double)(strlen(token_content_copy)));
	}
	for (int i = 1; i < lines; i++) {
		int before_padding_size = strlen(before);
		int after_padding_size  = strlen(after);
		char before_padding[before_padding_size];
		char after_padding[after_padding_size];
		memset(before_padding, 32, before_padding_size);	
		memset(after_padding, 32, after_padding_size);
		before_padding[before_padding_size] = '\0';
		after_padding[after_padding_size] = '\0';
		str_append_to_output("\n");
		
		token->content += output_width-strlen(before)-strlen(after);
		if (token->token == CALLOUT) {
			token->content-=6;
			token->content[0] = ' ';
			
			format_token_to_fit(token, before, after, 0, 0, 1);
			continue;
		}
			
		// ugly hack but i fucking gave up ok		
		if (before_padding_size+after_padding_size == 0) format_token_to_fit(token, "", "", 0, 0, 1);
		else if (before_padding_size == 0) format_token_to_fit(token, "", after_padding, 0, 0, 1);	
		else if (after_padding_size == 0)  format_token_to_fit(token, before_padding, "", 0, 0, 1);
		else format_token_to_fit(token, before_padding, after_padding, 0, 0, 1);
	}
}

void generate_output() {
	output = malloc(sizeof(char) * output_width * output_lines);
	if (output == NULL) {
		printf("%s memory allocation for output failed.\n", ERROR_PRINT);
		return;
	}
	
	for (int i = 0; i < MAX_TOKENS; i++) {
		if (tokens[i].content == NULL) break;
		if (tokens[i].token == NEW_LINE) str_append_to_output("\n");
		else if (tokens[i].token == TEXT)  format_token_to_fit(&tokens[i], "", "", 0, 1, 0);
		else if (tokens[i].token == HEADING_1)  format_token_to_fit(&tokens[i], "*- ", " -*", 0, 1, 0);
		else if (tokens[i].token == HEADING_2)  format_token_to_fit(&tokens[i], "**- ", " -**", 0, 1, 0);
		else if (tokens[i].token == HEADING_3)  format_token_to_fit(&tokens[i], "***- ", " -***", 0, 1, 0);
		else if (tokens[i].token == SIDE_ARROW)	format_token_to_fit(&tokens[i], "╰", "", 2, 1, 0); // 2 for "╰"
		else if (tokens[i].token == CALLOUT) {
			char border[output_width];
			
			strcpy(border, "┏---┳");
			memset(border+9, 45, output_width-6); // 45 = '-'
			strcpy(border+strlen(border), "┓");
			str_append_to_output(border);
			str_append_to_output("\n");
			
			format_token_to_fit(&tokens[i], "|", "|", 6, 1, 1);
			str_append_to_output("\n");

			strcpy(border, "┗---┻");
			memset(border+9, 45, output_width-6); // 45 = '-'
			strcpy(border+strlen(border)-3, "┛");
			str_append_to_output(border);
		}
			else if (tokens[i].token == DIVIDER) {
			char div[output_width];
			memset(div, 45, output_width); // 45 = '-'
			div[output_width] = '\0';
			str_append_to_output(div);
		}
	}
	output[output_index] = '\0';
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
		printf("%s failed to open input file(%s)\n", ERROR_PRINT, input_file_path);
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

	//printf("%s file open and read succes\n", DONE_PRINT);
	
	tokenize(input_file_content, input_file_size);
	free(input_file_content);
	
	//dev_print_tokens();
	//printf("%s tokenizing finished\n", DONE_PRINT);

	generate_output();
	printf("%s output generated\n", DONE_PRINT);
	if (output == NULL) {
		printf("%s failed to allocate memory for \"output\" (generation failed)\n", ERROR_PRINT);
    	} 
	printf("%s OUTPUT \n\n%s", DEBUG_PRINT, output);
        FILE *output_file = fopen(output_file_path, "w");
	if (input_file == NULL) {
		printf("%s failed to open output file(%s)\n", ERROR_PRINT, output_file_path);
		return -1;
	}
	output[strlen(output)] = '\0';
	fprintf(output_file, "%s", output);
	fclose(output_file);
	free((void*)output);

	printf("%s termarkup file outputted (%s)\n", DONE_PRINT, output_file_path);
	if(non_ascii_found_boolean) printf("%s one or more non-ascii charcters were found and was removed\n", WARNING_PRINT);
	return 0;
}




