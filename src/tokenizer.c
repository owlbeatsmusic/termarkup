#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/ajw_string.h"
#include "tokenizer.h"
#include "output.h"
#include "theme.h"
#include "main.h"


const int max_tokens = 512;
int num_tokens = 0;
Token tokens[max_tokens];
Token new_line_token_default = {TEXT, " ", DEFAULT};


void tokenizer_add_token(int *tokens_index, TokenType token, char *content, Modifier modifier) {
	tokens[*tokens_index].token_type = token;
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
		
		TokenType temp_token = TEXT;
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
			char text_buffer[main_max_width];
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
				if (j > cut_output_width-1) {
					output_lines++;
					break;
				}
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
	num_tokens = tokens_index;
}
