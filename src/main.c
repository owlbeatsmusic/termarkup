#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "common/ajw_file.h"
#include "common/ajw_print.h"
#include "common/ajw_bool.h"
#include "tokenizer.h"
#include "output.h"
#include "theme.h"


const int main_max_width = 512;
const int main_min_width = 10;


int main(int argc, char *argv[]) {
	
	// cli
	if (argc < 4 | argc == 0) {
		printf("\n\x1B[2m\t    string       string        int           string\x1B[0m\n");
		printf("./termarkup [input_file] [output_file] [file_width] ([theme_file])\n\n");
		return 0;
	}
	if (!strcmp(argv[1], "-version")) {
		printf("termarkup v1.1 by owlbeatsmusic\n");
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
	if (output_width > main_max_width) {
		printf("%s width is too large. maximum is %d\n", error_print, main_max_width);
		return -1;
	}
	if (output_width < main_min_width) {
		printf("%s width is too small. minimum is %d\n", error_print, main_min_width);
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
