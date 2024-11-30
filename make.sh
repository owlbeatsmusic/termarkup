#!/bin/zsh

gcc -Wall -pedantic -std=iso9899:1999 -I./include \
    \
    src/main.c \
    src/output.c \
    src/theme.c \
    src/tokenizer.c \
    src/common/ajw_file.c \
    src/common/ajw_print.c \
    src/common/ajw_string.c \
    \
    -o \
    termarkup
