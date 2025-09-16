#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "common/ajw_print.h"
#include "common/ajw_string.h"
#include "tokenizer.h"
#include "theme.h"

Style *styles[MAX_STYLES];
Style h1_style = {HEADING_1, "*- ", " -*", 3, 3, {NULL}};
Style h2_style = {HEADING_2, "**- ", " -**", 4, 4, {NULL}};
Style h3_style = {HEADING_3, "***- ", " -***", 5, 5, {NULL}};
Style side_arrow_style = {SIDE_ARROW, "╰ ", "", 2, 0, {NULL}};
Style divider_style = {DIVIDER, "-", "", 1, 0, {NULL}};
Style callout_style = {CALLOUT, "       ", " ", 7, 1, {"-", "|", "┏", "┳", "┓", "┗", "┻", "┛"}};
Style text_style = {TEXT, "", "", 0, 0, {NULL}};
Style new_line_style = {NEW_LINE, "", "", 0, 0, {NULL}};

const uint16_t max_theme_key_size = 64;
const uint16_t max_theme_value_size = 256;

uint16_t padding_x = 5; // error(28, 25, 22, 20)
uint16_t padding_y = 2;
bool show_border = true;
char *border_sheet[] = {"━", "┃", "┏", "┓", "┗", "┛"};
//char *border_sheet[] = {"-", "|", ".", ".", "'", "'"};
char *before_padding;
char *after_padding;


void theme_set_token_style(TokenType token_type, char *value) {
	int quotation_indicies[32];
	int j = 0;
	for (int i = 0; i < strlen(value); i++) {
		if (value[i] == '"') {
			quotation_indicies[j] = i;
			j++;
		}
	}
	if (token_type == DIVIDER) {
		styles[token_type]->before = str_get_string_between_quotations(value, quotation_indicies, 0, 1);
	}
	else if (token_type == CALLOUT) {
		for (int i = 0; i < 8; i++) {
			styles[token_type]->sheet[i] = str_get_string_between_quotations(value, quotation_indicies, i*2,  i*2+1);
		}
	}
	else if (token_type == BORDER) {
		char *show_border_string = str_get_string_between_quotations(value, quotation_indicies, 0, 1);
		if (strcmp(show_border_string, "false") == 0) {
			show_border = false;
		}

		// first "checkpoint"
		int first_int_start_index = 0;
		for (int i = quotation_indicies[1]; i < strlen(value); i++) {
			if (value[i] == ',') {
				first_int_start_index = i+1;
				break;
			}
		}

		// padding x
		char padding_x_string[16];
		memset(padding_x_string, 0, 16*sizeof(char));
		int k = 0;
		while (value[first_int_start_index + k] != ',') {
			// TODO: handle what happens if no other comma
			padding_x_string[k] = value[first_int_start_index + k];
			k++;	
		}
		k++;
		padding_x = atoi(padding_x_string);	

		// padding y
		char padding_y_string[32];
		int l = 0;
		while (value[first_int_start_index + k + l] != ']') {
			// TODO: handle what happens if no end bracket
			padding_y_string[l] = value[first_int_start_index + k + l];
			l++;	
		}	
		padding_y = atoi(padding_y_string);

		for (int i = 0; i < 6; i++) {
			border_sheet[i] = str_get_string_between_quotations(value, quotation_indicies, 2+i*2,  2+i*2+1);
		
		}
	}
	else {

		styles[token_type]->before = str_get_string_between_quotations(value, quotation_indicies, 0, 1);
		styles[token_type]->after = str_get_string_between_quotations(value, quotation_indicies, 2, 3);

		// first "checkpoint"
		int first_int_start_index = 0;
		for (int i = quotation_indicies[3]; i < strlen(value); i++) {
			if (value[i] == ',') {
				first_int_start_index = i+1;
				break;
			}
		}

		// before length
		char before_length[16];
		memset(before_length, 0, 16*sizeof(char));
		int k = 0;
		while (value[first_int_start_index + k] != ',') {
			// TODO: handle what happens if no other comma
			before_length[k] = value[first_int_start_index + k];
			k++;	
		}
		k++;
		styles[token_type]->before_length = atoi(before_length);	

		// after length
		char after_length[32];
		int l = 0;
		while (value[first_int_start_index + k + l] != ']') {
			// TODO: handle what happens if no end bracket
			after_length[l] = value[first_int_start_index + k + l];
			l++;
		}	
		styles[token_type]->after_length = atoi(after_length);	
			
	}
}

void theme_set(char *content) {

	//int line = 0;
	int start_of_line_index = 0;
	int equal_sign_index = 0;
	bool first_equal_sign_bool = false;

	for (int i = 0; i < strlen(content); i++) {
		if (content[i] == '=' & first_equal_sign_bool == false) {
			first_equal_sign_bool = true;	
			equal_sign_index = i;
		}
		if (content[i] == '\n' & first_equal_sign_bool == true) {

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
			
			//TokenType temp_token;
			if (str_compare_at_index(key, 0, "heading_1"))  theme_set_token_style(HEADING_1, value);
			if (str_compare_at_index(key, 0, "heading_2"))  theme_set_token_style(HEADING_2, value);
			if (str_compare_at_index(key, 0, "heading_3"))  theme_set_token_style(HEADING_3, value);
			if (str_compare_at_index(key, 0, "side_arrow")) theme_set_token_style(SIDE_ARROW, value);
			if (str_compare_at_index(key, 0, "divider"))    theme_set_token_style(DIVIDER, value);
			if (str_compare_at_index(key, 0, "callout"))    theme_set_token_style(CALLOUT, value);
			if (str_compare_at_index(key, 0, "border"))     theme_set_token_style(BORDER, value); // not actually a token type
			 
			//line++;
			equal_sign_index = 0;
			first_equal_sign_bool = false;
			if (i+1 < strlen(content)) start_of_line_index =  i+1;
		}
		else if (content[i] == '\n') {
			start_of_line_index = i+1;
		}
	}
}

void theme_initialize(void) {
	styles[0] = &h1_style;
	styles[1] = &h2_style;
	styles[2] = &h3_style;
	styles[3] = &side_arrow_style;
	styles[4] = &divider_style;
	styles[5] = &callout_style;
	styles[6] = &text_style;
	styles[7] = &new_line_style;
}
