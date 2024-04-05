#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>



// cli & general

typedef enum {
	FALSE,
	TRUE
} Bool;

const int max_width = 512;
const int min_width = 10;

const char *warning_print = "[\033[0;33mwarning\033[0m]:";
const char *error_print   = "[\033[0;31merror\033[0m]:";
const char *debug_print   = "[\033[0;35mdebug\033[0m]:";
const char *done_print    = "[\033[0;32mdone\033[0m]:";



// variables: tokenizing & output

typedef enum {
	DEFAULT,
	CENTER
} Modifier;

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
	Modifier modifier;

} TokenContent;

const int max_tokens = 512;
TokenContent tokens[max_tokens];

Bool non_ascii_found_bool = FALSE;
unsigned int output_width;
unsigned int output_lines;
unsigned int output_index = 0;
char *output;



// variables: themes & styles

typedef struct {
	Token token;
	char *before;
	char *after;
	int before_length;
	int after_length;
	
	char *sheet[8];
} Style;

Style *styles[7];
Style h1_style = {HEADING_1, "&= ", " =&", 3, 3, NULL};
Style h2_style = {HEADING_2, "&&= ", " =&&", 4, 4, NULL};
Style h3_style = {HEADING_3, "&&&= ", " =&&&", 5, 5, NULL};
Style side_arrow_style = {SIDE_ARROW, "> ", "", 2, 0, NULL};
Style divider_style = {DIVIDER, "~", "", 1, 0, NULL};
Style callout_style = {CALLOUT, "", "", 0, 0, {"-", "|", ".", ".", ".", "'", "'", "'"}};
Style text_style = {TEXT, "", "", 0, 0, NULL};

const int max_theme_key_size = 64;
const int max_theme_value_size = 256;

int padding_x = 2;
int padding_y = 2;
Bool border_bool = 1;
char *border_sheet[] = {"━", "┃", "┏", "┓", "┗", "┛"};
char *before_padding;
char *after_padding;




// string functions

int str_compare_at_index(char *content, int index, char* compare) {
	for (int i = 0; i < strlen(compare); i++) {
		if (content[index+i] != compare[i]) return 0; 
	}
	return 1;
}


char *str_create_divider(int length, char *symbol) {
	char *div = (char *)malloc((length) * (strlen(symbol)) * sizeof(char));
	if (div == NULL) {
		printf("%s failed to allocate memory for \"div\"\n", error_print);
		return NULL;
	}
	memset(div, 0, (length) * (strlen(symbol)));
	for (int i = 0; i < length; i++) {
		strcpy(div+(i*strlen(symbol)), symbol);
	}
	div[length * strlen(symbol)] = '\0';
	return div;	
}


char *str_get_string_between_quotations(char *value, int quotation_indicies[], int from, int to) {
	char *string = (char*)malloc(16 * sizeof(char));
	strncpy(string, value + quotation_indicies[from]+1, quotation_indicies[to]-quotation_indicies[from]-1);		
	string[quotation_indicies[to]-quotation_indicies[from]] = '\0';
	return string;
}




// tokenizer

void tokenizer_add_token(int *tokens_index, Token token, char *content, Modifier modifier) {
	tokens[*tokens_index].token = token;
	tokens[*tokens_index].content = (char *)malloc(strlen(content)+1);
	strcpy(tokens[*tokens_index].content, content);
	tokens[*tokens_index].content[strlen(content)] = '\0';
	tokens[*tokens_index].modifier = modifier;
	*tokens_index += 1;
	return;
}

void tokenizer_tokenize(char *content) {

	int tokens_index = 0;
	for (int i = 0; i < strlen(content); i++) {
		
		Token temp_token = TEXT;
		Modifier temp_modifier = DEFAULT;

		if (str_compare_at_index(content, i, "%c")) {
			temp_modifier = CENTER;
			i += 2;
		}
		if (str_compare_at_index(content, i, "\n")) {
			tokenizer_add_token(&tokens_index, NEW_LINE, " ", temp_modifier);
			temp_token = NEW_LINE;
		}
		else if (str_compare_at_index(content, i, "---")) {
			tokenizer_add_token(&tokens_index, DIVIDER, " ", temp_modifier);
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
			char text_buffer[max_width];
			memset(text_buffer, 0, sizeof(text_buffer));
			
			// create string-content of a token (like the
			// actual text of a header for example)
			int j = 0;
			while (content[i+j] != '\n') {
				// TODO: non ascii handling
				if (content[i+j] > 127 | content[i+j] < 0) { 
					non_ascii_found_bool = TRUE;
					i++;
					continue;
				}
				if (j > output_width) output_lines++;
				text_buffer[j] = content[i+j];
				j++;
			}
			text_buffer[strlen(text_buffer)] = '\0';
			
			// then append token and continue to next token
			tokenizer_add_token(&tokens_index, temp_token, text_buffer, temp_modifier);
			i += strlen(text_buffer)-1;
		}
		output_lines++;	
	}
}




// output generation

void output_append(char *string) {
	output_index += strlen(string);
	strncat(output, string, output_index);
	return;
}

void output_format_token_to_fit(TokenContent *token, char *before, char *after, int non_ascii_offset, Bool multiple_lines_bool, Bool fit_to_full_width) {

	int cut_output_size = strlen(token->content) + strlen(before) + strlen(after);
	char cut_output[cut_output_size];

	// cut content to fit in width
	char token_content_copy[strlen(token->content)];
	memset(token_content_copy, 0, strlen(token->content));
	strcpy(token_content_copy, token->content);


	int characters_to_cut = (strlen(before) + strlen(token->content) + strlen(after)+1) - output_width;	
	if (token->token == CALLOUT) characters_to_cut += (11-2*(strlen(callout_style.sheet[1])-1)) - 6*(1-multiple_lines_bool);
	if (multiple_lines_bool == 0) characters_to_cut += non_ascii_offset;
	if (characters_to_cut > 0) {
		for (int i = 1; i < characters_to_cut-non_ascii_offset; i++) {
			token_content_copy[strlen(token_content_copy)-1] = '\0';
		}
	}
	


	// place "after" at the end of width if bool is true
	int padding_size = output_width-(styles[token->token]->before_length+strlen(token_content_copy)+styles[token->token]->after_length);
	if (token->token == CALLOUT) padding_size -= 6; 
	else if (token->modifier == CENTER) {
		padding_size /= 2;
		padding_size -= (styles[token->token]->before_length-1)/2;
	}
	char *full_width_padding = "";
	if (padding_size > 0 && (fit_to_full_width == 1 | token->modifier == CENTER)) {
		full_width_padding = (char*)malloc(padding_size*sizeof(char));
		if (full_width_padding == NULL) {
			printf("%s memory allocation for full_width_padding failed.\n", error_print);
			return;
		}
		memset(full_width_padding, 32, padding_size);
	}

	// creating the final "line" and appending it tothe output
	if (token->token == CALLOUT) 
		sprintf(cut_output, "%s %c %s %s%s%s", before, token->content[0], callout_style.sheet[1], token_content_copy+1, full_width_padding, after);
	else if (token->modifier == CENTER) {
		output_append(full_width_padding);
		output_append(before); // because sprintf is weird
		sprintf(cut_output, "%s%s", token_content_copy, after);
	}
	else {
		output_append(before); // because sprintf is weird
		sprintf(cut_output, "%s%s%s", token_content_copy, full_width_padding, after);
	}
	output_append(cut_output);
	


	// recursion to add multiple lines if bool is true
	int lines = 0;
	if (multiple_lines_bool == 1 & strlen(token_content_copy) != 0) {
		lines = ceil(strlen(token->content)/(double)(strlen(token_content_copy)));
		if (token->token == CALLOUT) lines = ceil((strlen(token->content))/(double)(strlen(token_content_copy)-1));
	}

	for (int i = 1; i < lines; i++) {
		char before_token_padding[strlen(before)];
		char after_token_padding[strlen(after)];
		memset(before_token_padding, 32, strlen(before)-non_ascii_offset);	
		memset(after_token_padding, 32, strlen(after));
		
		output_append("\n");
		
		token->content += output_width-strlen(before)-strlen(after)+non_ascii_offset;
		output_append(before_padding);
		if (token->token == CALLOUT) {
			token->content-=12 - 2 * (strlen(callout_style.sheet[1]) - 1);
			token->content[0] = ' ';	
			output_format_token_to_fit(token, before, after, 0, FALSE, TRUE);
			continue;
		}

		// ugly hack but i gave up ok		
		if (strlen(before)+strlen(after) == 0) output_format_token_to_fit(token, "", "", non_ascii_offset, FALSE, TRUE);
		else if (strlen(after) == 0) output_format_token_to_fit(token, before_token_padding, "", non_ascii_offset, FALSE, TRUE);	
		else if (strlen(before) == 0)  output_format_token_to_fit(token, "", after_token_padding, non_ascii_offset, FALSE, TRUE);
		else output_format_token_to_fit(token, before_token_padding, after_token_padding, non_ascii_offset, FALSE, TRUE);
	}
}

void output_generate() {
	output = malloc(sizeof(char) * output_width * output_lines); // mem safety in main

	// create border / padding
	int before_padding_size = padding_x + border_bool * strlen(border_sheet[1]);
	before_padding = (char*)malloc(before_padding_size * sizeof(char));
	memset(before_padding, 32, before_padding_size);

	// border (above)

	for (int i = 0; i < padding_y; i++) {
		output_append("\n");
	}
	char border_above[output_width*strlen(border_sheet[0])];
	sprintf(border_above, "%s%s%s%s%s", 
			before_padding+strlen(border_sheet[1]), 
			border_sheet[2], 
			str_create_divider(output_width-strlen(border_sheet[2])-strlen(border_sheet[3]), 
			border_sheet[0]), 
			border_sheet[3], "\n");
	output_append(border_above);
	
	if (border_bool == TRUE) strcpy(before_padding+before_padding_size-strlen(border_sheet[1]), border_sheet[1]);

	output_width -= 2 * (padding_x + border_bool);


	// same order as defined in enum (tokens)
	output_append(before_padding);
	for (int i = 0; i < sizeof(tokens)/sizeof(tokens[0]); i++) {
		if (tokens[i].content == NULL) break;
		
		if (tokens[i].token == NEW_LINE) {
			output_append("\n");
			output_append(before_padding);
		}
		else if (tokens[i].token == DIVIDER) {
			char *div = str_create_divider(output_width, divider_style.before);
			output_append(div);
			free(div);
		}
		else if (tokens[i].token == CALLOUT) {
			
			char *div = str_create_divider(output_width-6, callout_style.sheet[0]);
			char *short_div = str_create_divider(3, callout_style.sheet[0]);

			// above calloiut
			char border_output[output_width*strlen(callout_style.sheet[0])];
			sprintf(border_output, "%s%s%s%s%s%s%s", 
					callout_style.sheet[2], 
					short_div, 
					callout_style.sheet[3], 
					div, 
					callout_style.sheet[4], 
					"\n", before_padding);
			output_append(border_output);

			// main part
			output_format_token_to_fit(&tokens[i], callout_style.sheet[1], callout_style.sheet[1], 6, TRUE, TRUE);
			output_append("\n");

			// under
			sprintf(border_output, "%s%s%s%s%s%s", 
					before_padding, 
					callout_style.sheet[5], 
					short_div, callout_style.sheet[6], 
					div, 
					callout_style.sheet[7]);
			output_append(border_output);	
			

			free(div);
			free(short_div);
		}
		else output_format_token_to_fit(
				&tokens[i], 
				styles[tokens[i].token]->before, 
				styles[tokens[i].token]->after, 
				strlen(styles[tokens[i].token]->before) - styles[tokens[i].token]->before_length, 
				TRUE, FALSE);
	}

	// border (under)
	output_width += 2 * (padding_x + border_bool);
	
	if (border_bool == TRUE) strcpy(before_padding+before_padding_size-strlen(border_sheet[1]), str_create_divider(strlen(border_sheet[1]), " "));
	
	char border_under[output_width*strlen(border_sheet[0])];
	sprintf(border_under, "%s%s%s%s%s%s", "\n", 
			before_padding+strlen(border_sheet[1]), 
			border_sheet[4], 
			str_create_divider(output_width-strlen(border_sheet[4])-strlen(border_sheet[5]), 
			border_sheet[0]), 
			border_sheet[5], "\n");
	output_append(border_under);
	
	output[output_index] = '\0';
	free(before_padding);
}




// themes

void theme_set_token_style(Token token, char *value) {
	
	int quotation_indicies[32];
	int j = 0;
	for (int i = 0; i < strlen(value); i++) {
		if (value[i] == '"') {
			quotation_indicies[j] = i;
			j++;
		}
	}
	if (token == DIVIDER) {
		styles[token]->before = str_get_string_between_quotations(value, quotation_indicies, 0, 1);
	}
	else if (token == CALLOUT) {
		for (int i = 0; i < 8; i++) {
			styles[token]->sheet[i] = str_get_string_between_quotations(value, quotation_indicies, i*2,  i*2+1);
		}
	}
	else {
		styles[token]->before = str_get_string_between_quotations(value, quotation_indicies, 0, 1);
		styles[token]->after = str_get_string_between_quotations(value, quotation_indicies, 2, 3);
	
		// before length
		int first_int_start_index = 0;
		for (int i = quotation_indicies[3]; i < strlen(value); i++) {
			if (value[i] == ',') {
				first_int_start_index = i+1;
				break;
			}
		}
		char before_length[32];
		int k = 0;
		while (value[first_int_start_index + k] != ',') {
			// TODO: handle what happens if no other comma
			before_length[k] = value[first_int_start_index + k];
			k++;	
		}
		k++;
		styles[token]->before_length = atoi(before_length);	

		// after length
		char after_length[32];
		int l = 0;
		while (value[first_int_start_index + k + l] != ']') {
			// TODO: handle what happens if no end bracket
			after_length[l] = value[first_int_start_index + k + l];
			l++;	
		}	
		styles[token]->after_length = atoi(after_length);	
			
	}
}

void theme_set(char *content) {

	int line = 0;
	int start_of_line_index = 0;
	int equal_sign_index = 0;
	Bool first_equal_sign_bool = FALSE;

	for (int i = 0; i < strlen(content); i++) {
		if (content[i] == '=' & first_equal_sign_bool == FALSE) {
			first_equal_sign_bool = TRUE;	
			equal_sign_index = i;
		}
		if (content[i] == '\n' & first_equal_sign_bool == TRUE) {

			char key[max_theme_key_size];
			memset(key, 0, max_theme_key_size);
			strncpy(key, content + start_of_line_index, equal_sign_index - start_of_line_index);

			// remove whitespace before key
			int j = 0;
			while (key[j] == 32) j++;
			for (int k = 0; k < strlen(key)-j; k++) {
				key[k] = key[k+j];
			}
			key[strlen(key)-j] = '\0';

			char value[max_theme_value_size];
			strncpy(value, content + equal_sign_index+1, i-equal_sign_index-1);
			
			Token temp_token;
			if (str_compare_at_index(key, 0, "heading_1")) theme_set_token_style(HEADING_1, value);
			if (str_compare_at_index(key, 0, "heading_2")) theme_set_token_style(HEADING_2, value);
			if (str_compare_at_index(key, 0, "heading_3")) theme_set_token_style(HEADING_3, value);
			if (str_compare_at_index(key, 0, "side_arrow")) theme_set_token_style(SIDE_ARROW, value);
			if (str_compare_at_index(key, 0, "divider")) theme_set_token_style(DIVIDER, value);
			if (str_compare_at_index(key, 0, "callout")) theme_set_token_style(CALLOUT, value);
			
			line++;
			equal_sign_index = 0;
			first_equal_sign_bool = FALSE;
			if (i+1 < strlen(content)) start_of_line_index =  i+1;
		}
		else if (content[i] == '\n') {
			start_of_line_index = i+1;
		}
	}
}




// main & cli

char *file_to_string(char *file_path) {
	FILE *file = fopen(file_path, "r");
	if (file == NULL) {
		printf("%s failed to open file(%s)\n", error_print, file_path);
		return NULL;
	}
	fseek(file, 0, SEEK_END);
	size_t file_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	char *file_content = (char *)malloc(file_size + 1);
	if (file_content == NULL) {
		fclose(file);
		printf("%s failed to allocate memory for contents of \"%s\"\n", error_print, file_path);
		return NULL;
	}
	file_content[file_size] = '\0';

	size_t file_read_size = fread(file_content, 1, file_size, file);
	if (file_read_size != file_size) {
		free(file_content);
		fclose(file);
		printf("%s failed to read file (%s)\n", error_print, file_path);
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
	char *theme_file_path;
	if (argc > 3) theme_file_path  = argv[4];
	
	if (sscanf(argv[3], "%d", &output_width) != 1) {
		printf("%s could not convert third argument to int (use -help)\n", error_print);
		return -1;
	}
	if (output_width > max_width) {
		printf("%s width is too large. maximum is %d\n", error_print, max_width);
		return -1;
	}
	if (output_width < min_width) {
		printf("%s width is too small. minimum is %d\n", error_print, min_width);
		return -1;
	}
	


	// reading the input file

	// open output file before starting
        FILE *output_file = fopen(output_file_path, "w");
	if (output_file == NULL) {
		printf("%s failed to open output file(%s)\n", error_print, output_file_path);
		return -1;
	}

	// open theme and set token-styles to theme
	// TODO: fix this ugly mess
	styles[0] = &h1_style;
	styles[1] = &h2_style;
	styles[2] = &h3_style;
	styles[3] = &side_arrow_style;
	styles[4] = &divider_style;
	styles[5] = &callout_style;
	styles[6] = &text_style;
	if (theme_file_path != NULL) { 
		char *theme_file_content = file_to_string(theme_file_path);
		if (theme_file_content == NULL) {
			fclose(output_file);
			return -1;
		}
		theme_set(theme_file_content);	
		free(theme_file_content);
	}

	// read input file and tokenize
	char *input_file_content = file_to_string(input_file_path); 
	if (input_file_content == NULL) {
		fclose(output_file);
		return -1;
	}
	tokenizer_tokenize(input_file_content);
	free(input_file_content);

	// generate output and create finish up
	output_generate();
	if (output == NULL) {
		printf("%s failed to allocate memory for \"output\" (generation failed)\n", error_print);
		fclose(output_file);
		return -1;
    	} 
	output[strlen(output)] = '\0';
	fprintf(output_file, "%s", output);
	printf("%s output:\n%s\n", debug_print, output);
	fclose(output_file);
	free((void*)output);

	if (theme_file_path != NULL) {
		for (int i = 0; i < 6; i++) {
			if (styles[i]->token == CALLOUT) {
				for (int j = 0; j < 8; j++) free(styles[i]->sheet[j]);
			}
			else {
				free(styles[i]->before);
				if (styles[i]->token != DIVIDER) free(styles[i]->after);
			}
		}
	}


	// done
	printf("%s termarkup file outputted (%s)\n", done_print, output_file_path);
	if(non_ascii_found_bool == TRUE) printf("%s one or more non-ascii charcters were found and was removed\n", warning_print);
	return 0;
}




