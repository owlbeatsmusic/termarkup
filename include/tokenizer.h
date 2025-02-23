#ifndef TOKENIZER_H_
#define TOKENIZER_H_

#include <stdint.h>

typedef enum {
	DEFAULT,
	CENTER
} Modifier;

typedef enum {
	HEADING_1,
	HEADING_2,
	HEADING_3,
	SIDE_ARROW,
	DIVIDER,
	CALLOUT,
	
	TEXT,

	NEW_LINE,
	END_FILE,
	
	BORDER,  // not actually a token type but works as one when reading theme file

} TokenType;

typedef struct {
	TokenType token_type;
	char *content;
	Modifier modifier;
	uint8_t is_first_line; // 0=not  1=first  2=last
} Token;

extern const uint16_t max_tokens;
extern uint16_t num_tokens;

extern Token tokens[];

extern Token new_line_token_default;


extern void tokenizer_tokenize(char *content);

#endif
