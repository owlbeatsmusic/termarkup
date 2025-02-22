#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "common/ajw_string.h"
#include "common/ajw_print.h"
#include "theme.h"
#include "tokenizer.h"
#include "output.h"


bool non_ascii_found_bool = false;
uint16_t output_width;
uint16_t cut_output_width;
uint16_t output_lines;
uint16_t cut_output_lines;

uint16_t line = 0;

void output_format_token_to_fit(Token *token, char output_grid[output_lines][output_width][32]) {

	/* structure of function
		1. VARIABLES - LENGTHS & PADDINGS
		2. OUTPUT GENERATION
			2.1 LEFT & RIGHT BORDER
			2.2 BEFORE
			2.3 TEXT / CONTENT
			2.4 AFTER
	*/

	// 1. LENGTHS & PADDINGS
	int before_length = styles[token->token_type]->before_length;
	int after_length = styles[token->token_type]->after_length;
	if (token->token_type == CALLOUT & token->is_first_line == 0) {
		before_length = 7;
		after_length = 1;
	}

	int text_length = cut_output_width - (before_length + after_length);
	if (text_length >= strlen(token->content)) {
		text_length = strlen(token->content);
	}

	int total_line_empty_space = cut_output_width-text_length-before_length-after_length;
	int center_padding = 0;
	if (token->modifier == CENTER) center_padding = floor((double)total_line_empty_space/2); // floor because it looks better with the smaller gap in front


	/*  2.1 LEFT & RIGHT BORDER  */
	strcpy(output_grid[line][padding_x], border_sheet[1]);				  // left
	strcpy(output_grid[line][output_width-padding_x-1], border_sheet[1]); // right

	/*  2.2 BEFORE  */

	// store and clear before and after if not first line of token
	char temp_before[MAX_BEFORE_AFTER_LENGTH];
	char temp_after[MAX_BEFORE_AFTER_LENGTH];
	if (token->is_first_line == false) {
		if (token->token_type != NEW_LINE | token->token_type != DIVIDER | token->token_type != CALLOUT | token->token_type != TEXT) {
			strcpy(temp_before, styles[token->token_type]->before);
			strcpy(temp_after, styles[token->token_type]->after);

			if (before_length > 0) styles[token->token_type]->before[before_length] = '\0';
			for (int i = 0; i < before_length; i++) {
				styles[token->token_type]->before[i] = ' ';
			}
			if (after_length > 0) styles[token->token_type]->after[after_length] = '\0';
			for (int i = 0; i < after_length; i++) {
				styles[token->token_type]->after[i] = ' ';
			}
		}
	}
	

	// set the first index on the screen grid to the before string and remove spaces to compensate
	if (before_length > 0) {
		strcpy(output_grid[line][center_padding+padding_x+1], styles[token->token_type]->before);
		int before_characters_to_remove = before_length - 1; // -1 because the previous index has the whole before string
		for (int i = 0; i < before_characters_to_remove; i++) { 
			strcpy(output_grid[line][center_padding+padding_x+2+i], "");
		}
	}


	/*  2.3 TEXT / CONTENT  */
	if (token->token_type == CALLOUT) {
		if (token->is_first_line == 1) {
			strcpy(output_grid[line][padding_x+1], callout_style.sheet[2]);
			strcpy(output_grid[line][padding_x+1+4], callout_style.sheet[3]);
			strcpy(output_grid[line][padding_x+cut_output_width], callout_style.sheet[4]);
		}
		if (token->is_first_line == 0) {
			strcpy(output_grid[line][padding_x+1], callout_style.sheet[1]);
			strcpy(output_grid[line][padding_x+1+4], callout_style.sheet[1]);
			strcpy(output_grid[line][padding_x+cut_output_width], callout_style.sheet[1]);
		}
	}
	if (token->token_type == NEW_LINE) {}
	else if (token->token_type == DIVIDER) {
		for (int i = 0; i < cut_output_width; i++) {
			strcpy(output_grid[line][padding_x+1+i], divider_style.before);
		}
	}
	else {
		for (int i = 0; i < text_length & i < text_length; i++) { 
			output_grid[line][center_padding + padding_x+before_length+1+i][0] = token->content[i];
			output_grid[line][center_padding + padding_x+before_length+1+i][1] = '\0';
		}
	}


	/*  2.4 AFTER  */
	
	if (styles[token->token_type]->after_length > 0) {
		strcpy(output_grid[line][center_padding+padding_x+before_length+1+text_length], styles[token->token_type]->after);

		int after_characters_to_remove = after_length - 1; // -1 because the previous index has the whole before string
		for (int i = 0; i < after_characters_to_remove; i++) { 
			strcpy(output_grid[line][center_padding+padding_x+before_length+text_length+2+i], "");
		}

	}

	
	//  (2.5 RE-SET BEFORE AND AFTER)
	if (token->is_first_line == false) {
		if (token->token_type != NEW_LINE | token->token_type != DIVIDER | token->token_type != CALLOUT | token->token_type != TEXT) {
			for (int i = 0; i < strlen(temp_before); i++) {
				styles[token->token_type]->before[i] = temp_before[i];
			}
			for (int i = 0; i < strlen(temp_before); i++) {
				styles[token->token_type]->after[i] = temp_after[i];
			}
		}

	}
	
	

	line++;
}


void output_generate(void) {

	/* structure of function
		1. INITIALIZE GRID
		2. CREATE BORDER
		3. OUTPUT THE TOKENS WITH FORMATTING
		4. PRINT OUTPUT
	*/

	
	/*  1.INITIALIZE GRID */

	output_lines = cut_output_lines + 2*padding_y + 2; // +2 for border symbols

	// Create the "Canvas", a grid of characters to draw to.
	char output_grid[output_lines][output_width][32]; // 4 because of unicode character being more than one character long
	for (int y = 0; y < output_lines; y++) {
		for (int x = 0; x < output_width; x++) { // +1 for safety when setting border right corners
			strcpy(output_grid[y][x], " ");
 		}
	}


	/*  2. BORDER  */
	if (border_bool == true) {

		// corner symbols
		strcpy(output_grid[padding_y][padding_x], border_sheet[2]);								// top-left
		strcpy(output_grid[padding_y][output_width-padding_x-1], border_sheet[3]);			  	// top-right
		strcpy(output_grid[output_lines-padding_y-1][padding_x], border_sheet[4]);				// bottom_left
		strcpy(output_grid[output_lines-padding_y-1][output_width-padding_x-1], border_sheet[5]);	// bottom-right

		// top & bottom line        (v  remove both sides of padding and -1 if padding_x is 0)
		for (int i = padding_x + 1; i < output_width - padding_x - 1; i++) {
			strcpy(output_grid[padding_y][i], border_sheet[0]);
			strcpy(output_grid[output_lines-padding_y-1][i], border_sheet[0]);
		}
	}

	line = padding_y + 1; //one below top border 

	/*  3. ADD ALL TOKENS  */
	for (int i = 0; i < num_tokens; i++) {
		if (tokens[i].token_type == NEW_LINE && tokens[i+1].token_type != NEW_LINE) {
			continue;
		}
		output_format_token_to_fit(&tokens[i], output_grid);
	} 


	/* 4. PRINT OUTPUT  */
	for (int y = 0; y < output_lines; y++) {
		for (int x = 0; x < output_width; x++) { // +1 for safety when setting border right corners
			printf("%s", output_grid[y][x]);
 		}
		printf("\n");
	}

}
