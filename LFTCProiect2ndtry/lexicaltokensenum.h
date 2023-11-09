#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lexer.h"
#include <string.h>
#include <ctype.h>

const char* enumLabels[COUNTER_ENUM] = {
		"ID",
		"TYPE_INT", "TYPE_REAL", "TYPE_STR", "RETURN", "END", "WHILE", "ELSE", "IF", "FUNCTION", "VAR",
		"COMMA", "FINISH", "COLON", "SEMICOLON", "LPAR", "RPAR",
		"ASSIGN", "EQUAL", "ADD", "SUB", "MUL", "DIV", "AND", "OR", "NOT", "NOTEQ", "LESS", "GREATER", "GREATEREQ",
		"INT", "REAL", "STR",
		"SPACE", "COMMENT", "TAB"
};