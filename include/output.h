#ifndef OUTPUT_H_
#define OUTPUT_H_

#include "common/ajw_bool.h"


extern Bool non_ascii_found_bool;
extern unsigned int output_width;
extern unsigned int cut_output_width;
extern unsigned int output_lines;
extern unsigned int output_index;
extern char *output;


extern void output_generate(void);

#endif 
