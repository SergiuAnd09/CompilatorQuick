#include <stdio.h>
#include <stdlib.h>
//#include <D:\LFTCProiect\LFTCProiect2ndtry\LFTCProiect2ndtry\lexer.h>
//#include <D:\LFTCProiect\LFTCProiect2ndtry\LFTCProiect2ndtry\utils.h>
#include "lexer.h"
#include "utils.h"
#include "parser.h"
#include "ad.h"
#include "at.h"

int main()
{
	//faza de analiza lexicala, ne da lista de atomi
	char *inbuf = loadFile("if.txt");
	tokenize(inbuf);
	//showTokens();
	//free(inbuf);
	//incepe aici faza de analiza sintactica
	parse();
	free(inbuf);
	return 0;
}
