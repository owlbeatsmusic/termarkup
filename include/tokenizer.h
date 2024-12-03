#ifndef TOKENIZER_H_
#define TOKENIZER_H_

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
} Token;

typedef struct {
	Token token;
	char *content;
	Modifier modifier;

} TokenContent;

extern const int max_tokens;
extern int num_tokens;

extern TokenContent tokens[];

extern TokenContent new_line_token_default;


extern void tokenizer_tokenize(char *content);

#endif
