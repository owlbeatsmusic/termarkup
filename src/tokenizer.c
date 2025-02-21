#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/ajw_bool.h"
#include "common/ajw_string.h"
#include "tokenizer.h"
#include "output.h"
#include "theme.h"
#include "main.h"


const int max_tokens = 512;
int num_tokens = 0;
Token tokens[max_tokens];
Token new_line_token_default = {TEXT, " ", DEFAULT};


void tokenizer_add_token(int *tokens_index, TokenType token, char *content, Modifier modifier, Bool is_first_line) {
	tokens[*tokens_index].token_type = token;
	tokens[*tokens_index].content = (char *)malloc(strlen(content)+1);
	strcpy(tokens[*tokens_index].content, content);
	tokens[*tokens_index].content[strlen(content)] = '\0';
	tokens[*tokens_index].modifier = modifier;
	tokens[*tokens_index].is_first_line = is_first_line;
	*tokens_index += 1;
	return;
}

void tokenizer_tokenize(char *content) {

	int tokens_index = 0;

	TokenType last_token_type = TEXT;
	Modifier last_modifier = DEFAULT;
	Bool temp_next_is_first_line = TRUE;

	for (int i = 0; i < strlen(content); i++) {
		
		TokenType temp_token_type = TEXT;
		Modifier temp_modifier = last_modifier;
		Bool temp_is_first_line = TRUE;

		if (str_compare_at_index(content, i, "%c")) {
			temp_modifier = CENTER;
			i += 2;
		}
		if (str_compare_at_index(content, i, "\n")) {
			tokenizer_add_token(&tokens_index, NEW_LINE, " ", temp_modifier, TRUE);
			temp_token_type = NEW_LINE;
			temp_next_is_first_line = TRUE;
			cut_output_lines++;	
		}
		else if (str_compare_at_index(content, i, "---")) {
			tokenizer_add_token(&tokens_index, DIVIDER, " ", temp_modifier, TRUE);
			temp_token_type = DIVIDER;
			temp_next_is_first_line = TRUE;
			i += 2;
		}
		else if (str_compare_at_index(content, i, "*-") && content[i+2] != '-') {
			temp_token_type = HEADING_1;
			i += 2;		
		} 
		else if (str_compare_at_index(content, i, "**-") && content[i+3] != '-') {
			temp_token_type = HEADING_2;
			i += 3;	
		}
		else if (str_compare_at_index(content, i, "***-") && content[i+4] != '-') {
			temp_token_type = HEADING_3;
			i += 4;
		}
		else if (str_compare_at_index(content, i, "+-")) {
			temp_token_type = SIDE_ARROW;
			i += 2;
			
		}
		else if (str_compare_at_index(content, i, "#")) {
			temp_token_type = CALLOUT;
			cut_output_lines += 2;
			i += 1;
			
		}
		if (temp_token_type != NEW_LINE && temp_token_type != DIVIDER) {
			char text_buffer[main_max_width];
			memset(text_buffer, 0, sizeof(text_buffer));
			
			// create string-content of a token (like the
			// actual text of a header for example)
			int j = 0;
			while (content[i+j] != '\n') {
				if (content[i+j] > 127 | content[i+j] < 0) { 
					non_ascii_found_bool = TRUE;
					i++;
					continue;
				}

				if (temp_next_is_first_line == FALSE) {
					temp_is_first_line = temp_next_is_first_line;
					temp_token_type = last_token_type;
				}

				int before_length = styles[temp_token_type]->before_length;
				int after_length  = styles[temp_token_type]->after_length;
				if (j > cut_output_width - (before_length + after_length) - 2) {
					cut_output_lines++;
					last_token_type = temp_token_type;
					last_modifier = temp_modifier;
					temp_next_is_first_line = FALSE;
					break;
				}
				else {
					last_token_type = TEXT;
					last_modifier = DEFAULT;
					temp_next_is_first_line = TRUE;
				}
				text_buffer[j] = content[i+j];
				j++;
			}
			text_buffer[strlen(text_buffer)] = '\0';
			
			// then append token and continue to next token
			tokenizer_add_token(&tokens_index, temp_token_type, text_buffer, temp_modifier, temp_is_first_line);
			i += strlen(text_buffer)-1;
			//output_lines++;	
		}
	}
	cut_output_lines++;
	tokenizer_add_token(&tokens_index, NEW_LINE, "", DEFAULT, TRUE);
	num_tokens = tokens_index;
}
