#ifndef AJW_STRING_H_
#define AJW_STRING_H_

int str_compare_at_index(char *content, int index, char* compare);
char *str_create_divider(int length, char *symbol);
char *str_get_string_between_quotations(char *value, int quotation_indicies[], int from, int to);

#endif
