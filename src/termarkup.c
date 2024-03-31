#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define MAX_THEME_KEY_SIZE 64
#define MAX_THEME_VALUE_SIZE 256
#define MAX_THEME_KEY_VALUE_PAIRS 32
#define MAX_TOKENS 512 // could probably estmate it better but works for now
#define MAX_WIDTH 512
#define MIN_WIDTH 10

#define WARNING_PRINT "[\033[0;33mwarning\033[0m]:"
#define ERROR_PRINT   "[\033[0;31merror\033[0m]:"
#define DEBUG_PRINT   "[\033[0;35mdebug\033[0m]:"
#define DONE_PRINT    "[\033[0;32mdone\033[0m]:"




// tokenizing & output

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




// themes & styles

typedef struct {
	char *key;
	char *value;
} ThemeKeyValue;

ThemeKeyValue theme_key_value_pairs[MAX_THEME_KEY_VALUE_PAIRS];

typedef struct {
	char *before;
	char *after;
	int before_length;
	int after_length;
	
	char *sheet[8];
} Style;

Style h1_style = {"&= ", " =&", 3, 3, NULL};
Style h2_style = {"&&= ", " =&&", 4, 4, NULL};
Style h3_style = {"&&&= ", " =&&&", 5, 5, NULL};
Style side_arrow_style = {"> ", "", 2, 0, NULL};
Style divider_style = {"~", "", 1, 0, NULL};
Style callout_style = {"", "", 0, 0, {"-", "|", ".", ".", ".", "'", "'", "'"}};
Style text_style = {"", "", 0, 0, NULL};
/*
Style h1_style = {"*- ", " -*", 2, 2, NULL};
Style h2_style = {"**- ", " -**", 3, 3, NULL};
Style h3_style = {"***- ", " -***", 4, 4, NULL};
Style side_arrow_style = {"┗ ", "", 2, 0, NULL};
Style divider_style = {"━", "", 1, 0, NULL};
Style callout_style = {"", "", 0, 0, {"━", "┃", "┏", "┳", "┓", "┗", "┻", "┛"}};
Style text_style = {"", "", 0, 0, NULL};
*/


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

char *str_create_divider(int length, char *symbol) {
	//char *div = (char *)malloc((length) * (strlen(symbol)) * sizeof(char));
	char *div = (char *)malloc((length) * (strlen(symbol)) * sizeof(char));
	if (div == NULL) {
		printf("%s failed to allocate memory for \"div\"\n", ERROR_PRINT);
		return NULL;
	}
	memset(div, 0, (length) * (strlen(symbol)));
	for (int i = 0; i < length; i++) {
		strcpy(div+(i*strlen(symbol)), symbol);
	}
	div[length * strlen(symbol)] = '\0';
	return div;	
}




// tokenizer

void add_token(int *tokens_index, Token token, char *content) {
	tokens[*tokens_index].token = token;
	tokens[*tokens_index].content = (char *)malloc(strlen(content)+1);
	strcpy(tokens[*tokens_index].content, content);
	tokens[*tokens_index].content[strlen(content)] = '\0';
	*tokens_index += 1;
	return;
}

void tokenize(char *content) {

	int tokens_index = 0;
	for (int i = 0; i < strlen(content); i++) {
		
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
			
			// create string-content of a token (like the
			// actual text of a header for example)
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
			
			// then append token and continue to next token
			add_token(&tokens_index, temp_token, text_buffer);
			i += strlen(text_buffer)-1;
		}
		output_lines++;	
	}
}



// output generation

void format_token_to_fit(TokenContent *token, char *before, char *after, int non_ascii_offset, int multiple_lines_boolean, int fit_to_full_width) {

	int cut_output_size = strlen(token->content) + strlen(before) + strlen(after);
	char cut_output[cut_output_size];
	memset(cut_output, 43, cut_output_size);
	cut_output[0] = '\0';	
	

	// cut content to fit in width
	char token_content_copy[strlen(token->content)];
	memset(token_content_copy, 0, strlen(token->content));
	strcpy(token_content_copy, token->content);

	int characters_to_cut = (strlen(before) + strlen(token->content) + strlen(after)+1) - output_width;	
	if (token->token == CALLOUT) characters_to_cut += (11-2*(strlen(callout_style.sheet[1])-1)) - 6*(1-multiple_lines_boolean);
	if (multiple_lines_boolean == 0) characters_to_cut += non_ascii_offset;
	if (characters_to_cut > 0) {
		for (int i = 1; i < characters_to_cut-non_ascii_offset; i++) {
			token_content_copy[strlen(token_content_copy)-1] = '\0';
		}
	}
	

	// place "after" at the end of width if bool is true
	int padding_size = output_width-(strlen(before)+strlen(token_content_copy)+strlen(after));
	if (token->token == CALLOUT) padding_size -= 4 - 2 * (strlen(callout_style.sheet[1])-1);
	char *full_width_padding = "";
	if (padding_size > 0 & fit_to_full_width == 1) {
		full_width_padding = (char*)malloc(padding_size*sizeof(char));
		if (full_width_padding == NULL) {
			printf("%s memory allocation for full_width_padding failed.\n", ERROR_PRINT);
			return;
		}
		memset(full_width_padding, 32, padding_size);
	}
	
	
	// creating the final "line" and appending it tothe output
	if (token->token == CALLOUT) 
		sprintf(cut_output, "%s %c %s %s%s%s", before, token->content[0], callout_style.sheet[1], token_content_copy+1, full_width_padding, after);
	else {
		str_append_to_output(before); // because sprintf is weird
		sprintf(cut_output, "%s%s%s", token_content_copy, full_width_padding, after);
	}
	str_append_to_output(cut_output);


	// recursion to add multiple lines if bool is true
	int lines = 0;
	if (multiple_lines_boolean == 1 & strlen(token_content_copy) != 0) {
		lines = ceil(strlen(token->content)/(double)(strlen(token_content_copy)));
		if (token->token == CALLOUT) lines = ceil((strlen(token->content))/(double)(strlen(token_content_copy)-1));
	}

	for (int i = 1; i < lines; i++) {
		char before_padding[strlen(before)];
		char after_padding[strlen(after)];
		memset(before_padding, 32, strlen(before)-non_ascii_offset);	
		memset(after_padding, 32, strlen(after));
		str_append_to_output("\n");
		
		token->content += output_width-strlen(before)-strlen(after)+non_ascii_offset;
		if (token->token == CALLOUT) {
			token->content-=12 - 2 * (strlen(callout_style.sheet[1]) - 1);
			token->content[0] = ' ';	
			format_token_to_fit(token, before, after, 0, 0, 1);
			continue;
		}

		// ugly hack but i gave up ok		
		if (strlen(before)+strlen(after) == 0) format_token_to_fit(token, "", "", non_ascii_offset, 0, 1);
		else if (strlen(after) == 0) format_token_to_fit(token, before_padding, "", non_ascii_offset, 0, 1);	
		else if (strlen(before) == 0)  format_token_to_fit(token, "", after_padding, non_ascii_offset, 0, 1);
		else format_token_to_fit(token, before_padding, after_padding, non_ascii_offset, 0, 1);
	}
}

void generate_output() {
	output = malloc(sizeof(char) * output_width * output_lines); // mem safety in main

	// same order as defined in enum (tokens)
	Style styles[] = {
		h1_style,
		h2_style,
		h3_style,
		side_arrow_style,
		divider_style,
		callout_style,
		text_style
	};
	for (int i = 0; i < MAX_TOKENS - 1; i++) { // -1 for END_FILE
		if (tokens[i].content == NULL) break;
		if (tokens[i].token == NEW_LINE) str_append_to_output("\n");
		else if (tokens[i].token == DIVIDER) {
			char *div = str_create_divider(output_width, divider_style.before);
			str_append_to_output(div);
			free(div);
		}
		else if (tokens[i].token == CALLOUT) {
			
			char *div = str_create_divider(output_width-6, callout_style.sheet[0]);
			char *short_div = str_create_divider(3, callout_style.sheet[0]);

			// above calloiut
			char border_output[output_width];
			sprintf(border_output, "%s%s%s%s%s%s", callout_style.sheet[2], short_div, callout_style.sheet[3], div, callout_style.sheet[4], "\n");
			str_append_to_output(border_output);	

			// main part
			format_token_to_fit(&tokens[i], callout_style.sheet[1], callout_style.sheet[1], 6, 1, 1);
			str_append_to_output("\n");

			// under
			sprintf(border_output, "%s%s%s%s%s%s", callout_style.sheet[5], short_div, callout_style.sheet[6], div, callout_style.sheet[7], "\n");
			str_append_to_output(border_output);	
			

			free(div);
			free(short_div);
		}
		else format_token_to_fit(
				&tokens[i], 
				styles[tokens[i].token].before, 
				styles[tokens[i].token].after, 
				strlen(styles[tokens[i].token].before) - styles[tokens[i].token].before_length, 
				1, 
				0);
	}
	output[output_index] = '\0';
}




// development functions

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




// themes

void set_theme(char *content) {

	int line = 0;
	int start_of_line_index = 0;
	int equal_sign_index = 0;
	int first_equal_sign_bool = 0;

	for (int i = 0; i < strlen(content); i++) {
		if (content[i] == '=' & first_equal_sign_bool == 0) {
			first_equal_sign_bool = 1;	
			equal_sign_index = i;
		}
		if (content[i] == '\n' & first_equal_sign_bool == 1) {

			char key[MAX_THEME_KEY_SIZE];
			strncpy(key, content + start_of_line_index, equal_sign_index - start_of_line_index);
			
			int j = 0;
			while (key[j] != 32) j++; // space before key

			// TODO : move string back to remove whitespace


			char value[MAX_THEME_VALUE_SIZE];
			strncpy(value, content + equal_sign_index+1, i-equal_sign_index-1);
			
			ThemeKeyValue tkv = {key, value};
			theme_key_value_pairs[line] = tkv;	

			line++;
			equal_sign_index = 0;
			first_equal_sign_bool = 0;
			if (i+1 < strlen(content)) start_of_line_index =  i+1;
		}
	}
}




// main & cli

char *file_to_string(char *file_path) {
	FILE *file = fopen(file_path, "r");
	if (file == NULL) {
		printf("%s failed to open file(%s)\n", ERROR_PRINT, file_path);
		return NULL;
	}
	fseek(file, 0, SEEK_END);
	size_t file_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	char *file_content = (char *)malloc(file_size + 1);
	if (file_content == NULL) {
		fclose(file);
		printf("%s failed to allocate memory for contents of \"%s\"\n", ERROR_PRINT, file_path);
		return NULL;
	}
	file_content[file_size] = '\0';

	size_t file_read_size = fread(file_content, 1, file_size, file);
	if (file_read_size != file_size) {
		free(file_content);
		fclose(file);
		printf("%s failed to read file (%s)\n", ERROR_PRINT, file_path);
		return NULL;
	}
	fclose(file);
	return file_content;
}

int main(int argc, char *argv[]) {
	
	// cli
	if (argc < 4 | argc == 0) {
		printf("\n\x1B[2m\t    string       string        int           string\x1B[0m\n");
		printf("./termarkup [input_file] [output_file] [file_width] ([theme_file])\n\n");
		return 0;
	}
	if (!strcmp(argv[1], "-version")) {
		printf("termarkup v1.0 by owlbeatsmusic\n");
		return 0;
	}
	else if (!strcmp(argv[1], "-help")) {
		printf("\n\x1B[2m\t    string       string        int           string\x1B[0m\n");
		printf("./termarkup [input_file] [output_file] [file_width] ([theme_file])\n\n");
		return 0;
	}
	char *input_file_path  = argv[1];
	char *output_file_path = argv[2];
	char *theme_file_path  = argv[4];
	
	if (sscanf(argv[3], "%d", &output_width) != 1) {
		printf("%s could not convert third argument to int (use -help)\n", ERROR_PRINT);
		return -1;
	}
	if (output_width > MAX_WIDTH) {
		printf("%s width is too large. maximum is %d\n", ERROR_PRINT, MAX_WIDTH);
		return -1;
	}
	if (output_width < MIN_WIDTH) {
		printf("%s width is too small. minimum is %d\n", ERROR_PRINT, MIN_WIDTH);
		return -1;
	}
	


	// reading the input file

	// open output file before starting
        FILE *output_file = fopen(output_file_path, "w");
	if (output_file == NULL) {
		printf("%s failed to open output file(%s)\n", ERROR_PRINT, output_file_path);
		return -1;
	}

	// open theme and set token-styles to theme
	char *theme_file_content = file_to_string(theme_file_path);
	if (theme_file_content == NULL) return -1;
	set_theme(theme_file_content);	
	free(theme_file_content);

	// read input file and tokenize
	char *input_file_content = file_to_string(input_file_path); 
	if (input_file_content == NULL) return -1;
	tokenize(input_file_content);
	free(input_file_content);

	// generate output and create finish up
	generate_output();
	if (output == NULL) {
		printf("%s failed to allocate memory for \"output\" (generation failed)\n", ERROR_PRINT);
		return -1;
    	} 
	output[strlen(output)] = '\0';
	fprintf(output_file, "%s", output);
	printf("%s output:\n%s\n", DEBUG_PRINT, output);
	fclose(output_file);
	free((void*)output);
	

	// complete
	printf("%s termarkup file outputted (%s)\n", DONE_PRINT, output_file_path);
	if(non_ascii_found_boolean) printf("%s one or more non-ascii charcters were found and was removed\n", WARNING_PRINT);
	return 0;
}




