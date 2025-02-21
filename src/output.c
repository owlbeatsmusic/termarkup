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

int line = 0;

void output_append(char *string) {
	output_index += strlen(string);
	strncat(output, string, output_index);
	return;
}

void output_format_token_to_fit(Token *token, char output_grid[output_lines][output_width][32]) {
	if (token->token_type == NEW_LINE) {
		return;
	}

	/*  LEFT & RIGHT BORDER  */
	strcpy(output_grid[line][padding_x], border_sheet[1]);				  // left
	strcpy(output_grid[line][output_width-padding_x-1], border_sheet[1]); // right


	/*  BEFORE  */
	// set the first index on the screen grid to the before string and remove spaces to compensate
	strcpy(output_grid[line][padding_x+1], styles[token->token_type]->before);
	int before_characters_to_remove = styles[token->token_type]->before_length - 1; // -1 because the previous index has the whole before string
	for (int i = 0; i < before_characters_to_remove; i++) { 
		strcpy(output_grid[line][padding_x+2+i], "");
	}

	line++;
}


void output_generate(void) {
	
	/*  INITIALIZE  */
	// Create the "Canvas", a grid of characters to draw to.
	char output_grid[output_lines][output_width][32]; // 4 because of unicode character being more than one character long
	for (int y = 0; y < output_lines; y++) {
		for (int x = 0; x < output_width; x++) { // +1 for safety when setting border right corners
			strcpy(output_grid[y][x], ".");
 		}
	}


	/*  BORDER  */
	if (border_bool == TRUE) {

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

	/*  ADD ALL TOKENS  */
	for (int i = 0; i < num_tokens; i++) {
		output_format_token_to_fit(&tokens[i], output_grid);
	} 


	/* PRINT OUTPUT  */
	for (int y = 0; y < output_lines; y++) {
		for (int x = 0; x < output_width; x++) { // +1 for safety when setting border right corners
			printf("%s", output_grid[y][x]);
 		}
		printf("\n");
	}

	printf("%d   lines\n", output_lines);
}
