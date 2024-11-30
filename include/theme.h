#ifndef THEME_H_
#define THEME_H_

#include <stdlib.h>

#include "common/ajw_bool.h"
#include "tokenizer.h"


typedef struct {
	Token token;
	char *before;
	char *after;
	int before_length;
	int after_length;
	
	char *sheet[8];
} Style;

extern Style *styles[];
extern Style h1_style;
extern Style h2_style;
extern Style h3_style;
extern Style side_arrow_style;
extern Style divider_style;
extern Style callout_style;
extern Style text_style;

extern const int max_theme_key_size;
extern const int max_theme_value_size;

extern int padding_x;
extern int padding_y;
extern Bool border_bool;
extern char *border_sheet[];
//char *border_sheet[] = {"-", "|", ".", ".", "'", "'"};
extern char *before_padding;
extern char *after_padding;



extern void theme_set(char *content);

#endif
