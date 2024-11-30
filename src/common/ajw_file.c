#include <stdlib.h>
#include <stdio.h>

#include "common/ajw_print.h"


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

