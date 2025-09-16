#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "common/ajw_print.h"
#include "common/ajw_string.h"

int str_compare_at_index(char *content, int index, char* compare) {
	for (int i = 0; i < strlen(compare); i++) {
		if (content[index+i] != compare[i]) return 0; 
	}	
	return 1;
}

char *str_get_string_between_quotations(char *value, int quotation_indicies[], int from, int to) {
	char *string = (char*)malloc(32 * sizeof(char));
	strncpy(string, value + quotation_indicies[from]+1, quotation_indicies[to]-quotation_indicies[from]-1);		
	string[quotation_indicies[to]-quotation_indicies[from]] = '\0';
	return string;
}
