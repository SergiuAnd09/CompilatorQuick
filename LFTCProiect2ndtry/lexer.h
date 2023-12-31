#pragma once

enum Tokens{
	ID
	// keywords
	, TYPE_INT, TYPE_REAL, TYPE_STR, RETURN, END, WHILE, ELSE, IF, FUNCTION, VAR //10
	// delimiters
	, COMMA, FINISH, COLON, SEMICOLON, LPAR, RPAR //6
	// operators
	, ASSIGN, EQUAL, ADD, SUB, MUL, DIV, AND, OR, NOT, NOTEQ, LESS, GREATER, GREATEREQ //13
	//constante
	,INT, REAL, STR//3
	//spatii
	,SPACE, COMMENT, TAB, COUNTER_ENUM//4 //COUNTER_ENUM e pus sa zica cate elemente sunt in enumerare
};

//const char* enumLabels[COUNTER_ENUM] = {
//		"ID",
//		"TYPE_INT", "TYPE_REAL", "TYPE_STR", "RETURN", "END", "WHILE", "ELSE", "IF", "FUNCTION", "VAR",
//		"COMMA", "FINISH", "COLON", "SEMICOLON", "LPAR", "RPAR",
//		"ASSIGN", "EQUAL", "ADD", "SUB", "MUL", "DIV", "AND", "OR", "NOT", "NOTEQ", "LESS", "GREATER", "GREATEREQ",
//		"INT", "REAL", "STR",
//		"SPACE", "COMMENT", "TAB"
//};

#define MAX_STR		127

typedef struct {
	int code;		// ID, TYPE_INT, ...
	int line;		// the line from the input file
	union {
		char text[MAX_STR + 1];		// the chars for ID, STR
		int i;		// the value for INT
		double r;		// the value for REAL
	};
}Token;

#define MAX_TOKENS		4096
extern Token tokens[];
extern int nTokens;

void tokenize(const char *pch);
void showTokens();
