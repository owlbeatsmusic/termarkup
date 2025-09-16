#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "common/ajw_string.h"
#include "tokenizer.h"
#include "output.h"
#include "theme.h"
#include "main.h"


const uint16_t max_tokens = 64;
uint16_t num_tokens = 0;
Token tokens[max_tokens];
Token new_line_token_default = {TEXT, " ", DEFAULT};


void tokenizer_add_token(int *tokens_index, TokenType token, char *content, Modifier modifier, uint8_t is_first_line) {
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
	bool temp_next_is_first_of_line = true;

	for (int i = 0; i < strlen(content); i++) {
		
		TokenType temp_token_type = TEXT;
		Modifier temp_modifier = last_modifier;
		bool temp_is_first_line = true;

		if (str_compare_at_index(content, i, "%c")) {
			temp_modifier = CENTER;
			i += 2;
		}
		if (str_compare_at_index(content, i, "\n")) {
			tokenizer_add_token(&tokens_index, NEW_LINE, " ", temp_modifier, true);
			temp_token_type = NEW_LINE;
			temp_next_is_first_of_line = true;
			cut_output_lines++;	
		}
		else if (str_compare_at_index(content, i, "---")) {
			tokenizer_add_token(&tokens_index, DIVIDER, " ", temp_modifier, true);
			temp_token_type = DIVIDER;
			temp_next_is_first_of_line = true;
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
			cut_output_lines += 1;
			i += 1;
		}
		if (temp_token_type != NEW_LINE && temp_token_type != DIVIDER) {
			char text_buffer[main_max_width];
			memset(text_buffer, 0, sizeof(text_buffer));
			
			// create string-content of a token (like the
			// actual text of a header for example)
			int j = 0;
			while (content[i+j] != '\n') {

				if (temp_token_type == CALLOUT & temp_is_first_line  == true) {
					last_token_type = temp_token_type;
					temp_next_is_first_of_line = false;
					text_buffer[j] = content[i+j];
					break;
				}
				
				if (content[i+j] > 127 | content[i+j] < 0) { 
					non_ascii_found_bool = true;
					i++;
					continue;
				}

				if (temp_next_is_first_of_line == false) {
					temp_is_first_line = temp_next_is_first_of_line;
					temp_token_type = last_token_type;
				}

				int before_length = styles[temp_token_type]->before_length;
				int after_length  = styles[temp_token_type]->after_length;
				if (j > cut_output_width - (before_length + after_length) - 2) {
					cut_output_lines++;
					last_token_type = temp_token_type;
					last_modifier = temp_modifier;
					temp_next_is_first_of_line = false;
					break;
				}
				else {
					last_token_type = TEXT;
					last_modifier = DEFAULT;
					temp_next_is_first_of_line = true;
				}
				text_buffer[j] = content[i+j];
				j++;
			}
			text_buffer[strlen(text_buffer)] = '\0';
			
			// then append token and continue to next token
			tokenizer_add_token(&tokens_index, temp_token_type, text_buffer, temp_modifier, temp_is_first_line);
			
			// These two conditions only are true at the same time when the last line of a callout was created
			if (temp_token_type == CALLOUT && last_token_type == TEXT) {
				tokenizer_add_token(&tokens_index, temp_token_type, " ", temp_modifier, 2); // "is_first_line=3" means it the last line
				cut_output_lines++;
			}
			
			i += strlen(text_buffer)-1;
			
		}
	}

	// fix for end of file
	tokenizer_add_token(&tokens_index, NEW_LINE, "", DEFAULT, 1);
	num_tokens = tokens_index;

	// TODO: why this line? Probably because of first or last line of file not adding to total output lines.
	cut_output_lines++;

	//for (int i = 0; i < sizeof(tokens)/sizeof(Token); i++) {
	//	printf("%d - %d\n", tokens[i].token_type, tokens[i].is_first_line);
	//}
}
