#ifndef OUTPUT_H_
#define OUTPUT_H_

#include <stdbool.h>
#include <stdint.h>

extern bool non_ascii_found_bool;
extern uint16_t output_width;
extern uint16_t cut_output_width;
extern uint16_t output_lines;
extern uint16_t cut_output_lines;


extern void output_generate(FILE *output_file);

#endif 
