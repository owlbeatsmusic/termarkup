#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "common/ajw_string.h"
#include "common/ajw_print.h"
#include "theme.h"
#include "tokenizer.h"
#include "output.h"


Bool non_ascii_found_bool = FALSE;
unsigned int output_width;
unsigned int output_lines;
unsigned int output_index = 0;
char *output;


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
	int characters_to_cut = (styles[token->token]->before_length + strlen(token->content) + styles[token->token]->after_length +1) - output_width;	
	
	// TODO: explain hy this addidtion why this works
	if (token->token == CALLOUT) characters_to_cut += 5 + 2 * (strlen(callout_style.sheet[1])) + 2 * (1-strlen(callout_style.sheet[0])); 
	if (characters_to_cut > 0) {
		for (int i = 1; i < characters_to_cut; i++) {
			token_content_copy[strlen(token_content_copy)-1] = '\0';
		}
	}
	


	// TODO: beskrvinig för det här steget
	int padding_size = output_width-(styles[token->token]->before_length+strlen(token_content_copy)+styles[token->token]->after_length);
	if (token->token == CALLOUT) padding_size -= 5; 
	else if (token->modifier == CENTER) {
		padding_size = padding_size/2 + 1;
	}
	char *full_width_padding[] = { "", "" }; // before & after
	int padding_sizes[2] = { 
		padding_size + (1-output_width%2) * (token->token != CALLOUT), 
		padding_size
		};
	if (padding_size < 2) padding_sizes[0] = 0;
	if (token->modifier != CENTER) padding_sizes[1] = 1;
	else padding_sizes[1] -= 1-strlen(token_content_copy)%2;
	for (int i = 0; i < 2; i++) {
		full_width_padding[i] = (char*)malloc(padding_sizes[i]*sizeof(char));
		if (full_width_padding[i] == NULL) {
			printf("%s memory allocation for full_width_padding[%d] failed.\n", error_print, i);
			return;
		}
		memset(full_width_padding[i], 32, padding_sizes[i]);
		full_width_padding[i][padding_sizes[i]-1] = '\0';
	}



	// creating the final "line" and appending it to the output
	if (token->token == CALLOUT) 
		sprintf(cut_output, "%s %c %s %s%s%s", before, token->content[0], callout_style.sheet[1], token_content_copy+1, full_width_padding[0], after);
	else if (token->modifier == CENTER) {
		output_append(full_width_padding[0]);
		output_append(before); // because sprintf is weird
		sprintf(cut_output, "%s%s%s", token_content_copy, after, full_width_padding[1]);
	}
	else {
		output_append(before); // because sprintf is weird
		sprintf(cut_output, "%s%s%s", token_content_copy, after, full_width_padding[0]); 
	}
	output_append(cut_output);
	if (border_bool == TRUE) output_append(border_sheet[1]);
	


	// recursion to add multiple lines if bool is true
	int lines = 0;
	if (multiple_lines_bool == 1 & strlen(token_content_copy) != 0) {
		lines = ceil(strlen(token->content)/(double)(strlen(token_content_copy)));
		if (token->token == CALLOUT) lines = ceil((strlen(token->content))/(double)(strlen(token_content_copy)-1));
	}
	for (int i = 1; i < lines; i++) {
		char before_token_padding[strlen(before)];
		char after_token_padding[strlen(after)];
		//memset(before_token_padding, 0, strlen(before));	
		//memset(after_token_padding, 0, strlen(after));	
		memset(before_token_padding, 32, styles[token->token]->before_length);	
		memset(after_token_padding, 32, styles[token->token]->after_length);
		
		output_append("\n");
	
		int offset = output_width-styles[token->token]->before_length-styles[token->token]->after_length;
		token->content += offset;

		output_append(before_padding);
		if (token->token == CALLOUT) {
			//token->content-= 10 - 2 * strlen(callout_style.sheet[1]);
			offset = 1 + 5 + 2 * (strlen(callout_style.sheet[1])) + 2 * (1-strlen(callout_style.sheet[0]));
			token->content -= offset;
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
	if (padding_size > 0) {
		free(full_width_padding[0]);
		free(full_width_padding[1]);
	}
}
	


void output_generate(void) {
	output = malloc(sizeof(char) * (output_width*3) * output_lines); // mem safety in main

	// create border / padding
	int before_padding_size = padding_x + border_bool * (strlen(border_sheet[1]-1));
	before_padding = (char*)malloc((before_padding_size+1) * sizeof(char));
	memset(before_padding, 32, before_padding_size);
	before_padding[before_padding_size] = '\0';

	// border (above)

	for (int i = 0; i < padding_y; i++) {
		output_append("\n");
	}
	char *border_div;
	if (border_bool == TRUE) {
		border_div = str_create_divider(output_width-2-2*padding_x, border_sheet[0]);
		
		char border_above[output_width*strlen(border_sheet[0])];
		sprintf(border_above, "%s%s%s%s%s", 
				before_padding+strlen(border_sheet[1]), 
				border_sheet[2], 
				border_div, 
				border_sheet[3], "\n");
		output_append(border_above);
		strcpy(before_padding+before_padding_size-strlen(border_sheet[1]), border_sheet[1]);
	}

	output_width -= 2 * (padding_x + border_bool);


	// same order as defined in enum (tokens)
	output_append(before_padding);
	for (int i = 0; i < num_tokens; i++) {
		if (tokens[i].content == NULL) break;
		
		if (tokens[i].token == NEW_LINE) {
			
			output_append("\n");
			
			if (tokens[i+1].token == NEW_LINE) {
				output_append(before_padding);
				output_format_token_to_fit(&new_line_token_preset, "", "", 0, FALSE, FALSE);
			}
			else {
				output_append(before_padding);
			}
		}
		else if (tokens[i].token == DIVIDER) {
			char *div = str_create_divider(output_width, divider_style.before);
			output_append(div);
			if (border_bool == TRUE) output_append(border_sheet[1]);
			free(div);
		}
		else if (tokens[i].token == CALLOUT) {
			
			char *div = str_create_divider(output_width-6, callout_style.sheet[0]);
			char *short_div = str_create_divider(3, callout_style.sheet[0]);

			// above calloiut
			char border_output[output_width*strlen(callout_style.sheet[0])];
			sprintf(border_output, "%s%s%s%s%s", 
					callout_style.sheet[2], 
					short_div, 
					callout_style.sheet[3], 
					div, 
					callout_style.sheet[4] 
					);
			output_append(border_output);	
			if (border_bool == TRUE) output_append(border_sheet[1]);
			output_append("\n");

			// main part
			output_append(before_padding);
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
			if (border_bool == TRUE) output_append(border_sheet[1]);
			

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
	
	if (border_bool == TRUE) {
		strcpy(before_padding+before_padding_size-strlen(border_sheet[1]), str_create_divider(strlen(border_sheet[1]), " "));
	
		char border_under[output_width*strlen(border_sheet[0])];
		sprintf(border_under, "%s%s%s%s%s%s", "\n", 
				before_padding+strlen(border_sheet[1]), 
				border_sheet[4], 
				border_div, 
				border_sheet[5], "\n");
		output_append(border_under);
		free(border_div);
	}
	output[output_index] = '\0';
	free(before_padding);
}
