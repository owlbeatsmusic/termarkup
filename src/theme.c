#include <string.h>
#include <stdio.h>

#include "common/ajw_print.h"
#include "common/ajw_string.h"
#include "common/ajw_bool.h"
#include "tokenizer.h"
#include "theme.h"

Style *styles[MAX_STYLES];
Style h1_style = {HEADING_1, "&= ", " =&", 3, 3, {NULL}};
Style h2_style = {HEADING_2, "&&= ", " =&&", 4, 4, {NULL}};
Style h3_style = {HEADING_3, "&&&= ", " =&&&", 5, 5, {NULL}};
Style side_arrow_style = {SIDE_ARROW, "> ", "", 2, 0, {NULL}};
Style divider_style = {DIVIDER, "~", "", 1, 0, {NULL}};
Style callout_style = {CALLOUT, "", "", 0, 0, {"-", "|", ".", ".", ".", "'", "'", "'"}};
Style text_style = {TEXT, "", "", 0, 0, {NULL}};
Style new_line_style = {NEW_LINE, "", "", 0, 0, {NULL}};


const int max_theme_key_size = 64;
const int max_theme_value_size = 256;

int padding_x = 5; // error(28, 25, 22, 20)
int padding_y = 3;
Bool border_bool = TRUE;
char *border_sheet[] = {"━", "┃", "┏", "┓", "┗", "┛"};
//char *border_sheet[] = {"-", "|", ".", ".", "'", "'"};
char *before_padding;
char *after_padding;


void theme_set_token_style(TokenType token, char *value) {
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
		char before_length[16];
		memset(before_length, 0, 16*sizeof(char));
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

	//int line = 0;
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
			
			//TokenType temp_token;
			if (str_compare_at_index(key, 0, "heading_1"))  theme_set_token_style(HEADING_1, value);
			if (str_compare_at_index(key, 0, "heading_2"))  theme_set_token_style(HEADING_2, value);
			if (str_compare_at_index(key, 0, "heading_3"))  theme_set_token_style(HEADING_3, value);
			if (str_compare_at_index(key, 0, "side_arrow")) theme_set_token_style(SIDE_ARROW, value);
			if (str_compare_at_index(key, 0, "divider"))    theme_set_token_style(DIVIDER, value);
			if (str_compare_at_index(key, 0, "callout"))    theme_set_token_style(CALLOUT, value);
			
			//line++;
			equal_sign_index = 0;
			first_equal_sign_bool = FALSE;
			if (i+1 < strlen(content)) start_of_line_index =  i+1;
		}
		else if (content[i] == '\n') {
			start_of_line_index = i+1;
		}
	}
}
