#ifndef THEME_H_
#define THEME_H_

#include <stdlib.h>
#include <stdint.h>

#include "tokenizer.h"

#define MAX_STYLES 8
#define MAX_BEFORE_AFTER_LENGTH 32

typedef struct {
	TokenType token;
	char *before;
	char *after;
	uint8_t before_length;
	uint8_t after_length;
	
	char *sheet[MAX_STYLES];
} Style;

extern Style *styles[];
extern Style h1_style;
extern Style h2_style;
extern Style h3_style;
extern Style side_arrow_style;
extern Style divider_style;
extern Style callout_style;
extern Style text_style;
extern Style new_line_style;

extern const uint16_t max_theme_key_size;
extern const uint16_t max_theme_value_size;

extern uint16_t padding_x;
extern uint16_t padding_y;
extern bool show_border;
extern char *border_sheet[];
//char *border_sheet[] = {"-", "|", ".", ".", "'", "'"};
extern char *before_padding;
extern char *after_padding;



extern void theme_set(char *content);

extern void theme_initialize(void);

#endif
