#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "common/ajw_print.h"


int str_compare_at_index(char *content, int index, char* compare) {
	for (int i = 0; i < strlen(compare); i++) {
		if (content[index+i] != compare[i]) return 0; 
	}	
	return 1;
}


// TODO: CALLOUT DIV BETER SIG INTE SOM DEN SKA VID OLIKA WIDTH
char *str_create_divider(int length, char *symbol) {
    int symbol_length = strlen(symbol);
	char *div = (char*)malloc((length + 1) * symbol_length * sizeof(char));
	if (div == NULL) {
		printf("%s failed to allocate memory for \"div\"\n", PRINT_ERROR);
		return NULL;
	}
    memset(div, 0, (length + 1) * symbol_length * sizeof(char)); 
	for (int i = 0; i < length; i++) {
		strcpy(div+(i*symbol_length), symbol);
	}
	div[length * symbol_length] = '\0';
	return div;	
}


char *str_get_string_between_quotations(char *value, int quotation_indicies[], int from, int to) {
	char *string = (char*)malloc(32 * sizeof(char));
	strncpy(string, value + quotation_indicies[from]+1, quotation_indicies[to]-quotation_indicies[from]-1);		
	string[quotation_indicies[to]-quotation_indicies[from]] = '\0';
	return string;
}
