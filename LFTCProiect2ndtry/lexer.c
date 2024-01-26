#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "lexer.h"
#include "utils.h"
#include "float.h"
#include "lexicaltokensenum.h"
//tema noastra e sa facem mai c9omolet cazurile din tokenize si sa facem si showtoken

Token tokens[MAX_TOKENS];
int nTokens;
int line = 1;		// the current line in the input file
// adds a token to the end of the tokens list and returns it
// sets its code and line

Token *addTk(int code) {
	if (nTokens == MAX_TOKENS)err("too many tokens");
	Token *tk = &tokens[nTokens];
	tk->code = code;
	tk->line = line;
	nTokens++;
	return tk;
}

// copy in the dst buffer the string between [begin,end)
char *copyn(char *dst, const char *begin, const char *end) {
	char *p = dst;
	if (end - begin > MAX_STR)err("string too long");
	while (begin != end)*p++ = *begin++;
	*p = '\0';
	return dst;
}

int isNumberInteger(char *sir) { //FOLOSIT DEOCAMDATA
	/*if (strlen(sir) > 1 || strlen(sir) == 0) {
		return 0;
	}
	else if (isdigit(sir)) {
		return 1;
	}
	else return 0;*/
	//prima versiune
	/*int retur = 1;
	int i = 0;
	while (sir[i] != '\0') //cat timp merge pana la eof, ii bine
	{
		if (isdigit(sir[i]))//vede daca e cifra
		{

		}
		else
		{
			retur = 0;//daca nu e digit, atunci nu ii face nimica si ii da 0
		}
		i++;
	}
	return retur;*/


	//a doua versiune
	int retur = 1;
	int i = 0;
	while (sir[i] != '\0')
	{
		if (isdigit(sir[i])) {}
		else
		{
			//if (sir[i] == '.') {//aici verifica daca pica un punct dupa primul integer, daca da inseamna ca avem real
			//	retur = 0;
			//}
			retur = 0;
		}
		i++;
	}
	
	return retur; 
}
//CE E MAI SUS E FOLOSIT
int isSomeNumber(char *sir) {
	int retur = 1;
	int i = 0;
	while (sir[i] != '\0')
	{
		if (isdigit(sir[i]) || sir[i]=='.') {}//pe ramura asta el verifica doar ca sa fie nr sau digit
		else
		{
			
			retur = 0;//aici cand deja depaseste textul cu pricina
		}
		i++;//aici se trece la urmatorul caracter de verificat in conditite
	}

	return retur;//daca ramane 1 inseamna ca e numar, daca nu inseamna ca nu e 
}
//CE E MAI SUS E FOLOSIT

void tokenize(const char *pch) { //aici o sa ne scriem noi toata functia la ce avem de lucru la acest lab
	//pch e present character din file sau textul dat

	int prima = 1;//pentru prima " a unui sir, la a doua ia valoarea 2
	int numarSchimbat = 0;//pentru functia isNumberInteger
	const char *start;//startul textului ce se analizeaza
	int contor=1;//pentru sir
	Token *tk;//pentru afisari si assignari
	char buf[MAX_STR + 1];//initial de folosit la strings, renuntat la idee

	for (;;) {
		switch (*pch) {
		case ' ':case '\t':pch++; break;//se merge in cascada prin space si tab si se merge mai departe 
		case '\r':		// handles different kinds of newlines (Windows: \r\n, Linux: \n, MacOS, OS X: \r or \n)
			if (pch[1] == '\n')pch++;
			// fallthrough to \n, adica nu ii pune break si merge mai departe cu executia chiar la \n, wow
		case '\n':
			line++;
			pch++;
			break;
		case '#':
			while (*pch != '\n' && *pch != EOF) {
				pch++;
				//contoras++;
				//line++;
				//printf("\nAm intrat \n");
			}
			break;
		case '\0':addTk(FINISH); return;
		case ',':addTk(COMMA); pch++; break;
		case '=':
			if (pch[1] == '=') {
				addTk(EQUAL);
				pch += 2;
			}
			else {
				addTk(ASSIGN);
				pch++;
			}
			break;
		case '!':
			if (pch[1] == '=') {
				addTk(NOTEQ);
				pch += 2;
			}
			else {
				addTk(NOT);
				pch++;
			}
			break;
		case '>':
			if (pch[1] == '=') {
				addTk(GREATEREQ);
				pch += 2;
			}
			else {
				addTk(GREATER);
				pch++;
			}
			break;
		case '+':
			addTk(ADD);
			pch++;
			break;
		case '-':
			addTk(SUB);
			pch++;
			break;
		case '*':
			addTk(MUL);
			pch++;
			break;
		case '/':
			addTk(DIV);
			pch++;
			break;
		case '&':
			if (pch[1] == '&') {
				addTk(AND);
				pch += 2;
			}
			else {
				continue;//merge mai departe in bucla la urmatoarea iteratie
			}
			break;
		case '|':
			if (pch[1] == '|') {
				addTk(OR);
				pch += 2;
			}
			else {
				continue;//-||- '&'
			}
			break;
		case '<':
			addTk(LESS);
			pch++;
			break;
		
		case ':':
			addTk(COLON);
			pch++;
			break;
		case ';':
			addTk(SEMICOLON);
			pch++;
			break;
		case '(':
			addTk(LPAR);
			pch++;
			break;
		case ')':
			addTk(RPAR);
			pch++;
			break;
		default:
			if (isalpha(*pch) || *pch == '_' || isdigit(*pch)  || *pch == '"') { //e de primul caracter, ce poate sa fie ca sa fie luat in considerare

				if (*pch == '"' && prima==1) {
					prima = 2;//aici daca a gasit o ghilimea ia val 2, adica valoarea ca sa nu mai vina aici
					for (start = pch++; *pch != '"'; pch++, contor++) { if (*pch == '\0') err("\nEnd of file error."); }//aici se afla marginile textului ce va fi pus intr-o unitate manevrabila
					char *text = copyn(buf, start, pch);
				}
				else if (*pch == '"' && prima == 2) {
					prima = 1;
					char *text = copyn(buf, (pch-contor), (pch));//aici
					pch++;
					tk = addTk(STR);
					strcpy(tk->text, text);
					contor = 0;
				}
				else {
				for (start = pch++; isalnum(*pch) || *pch == '_' || *pch == '.'; pch++) {}//aici se afla marginile textului ce va fi pus intr-o unitate manevrabila
				char *text = copyn(buf, start, pch); //aici se pune textul intr-o unitate manevrabila
				//de aci incolo se vede unde se incadreaza textul facut mai sus
				if (strcmp(text, "int") == 0)addTk(TYPE_INT);
				else if (strcmp(text, "float") == 0) {
					addTk(TYPE_REAL);
				}
				else if (strcmp(text, "double") == 0) { //sa fie double si neinaintat de "long"
					addTk(TYPE_REAL);
				}
				else if (strcmp(text, "var") == 0) { //la var
					addTk(VAR);
				}
				else if (strcmp(text, "if") == 0) {
					addTk(IF);
				}
				else if (strcmp(text, "function") == 0) {
					addTk(FUNCTION);
				}
				else if (strcmp(text, "else") == 0) {
					addTk(ELSE);
				}
				else if (strcmp(text, "while") == 0) {
					addTk(WHILE);
					//printf("\nCe text avem noi: %s\n", text);
				}
				else if (strcmp(text, "end") == 0) {
					addTk(END);
				}
				else if (strcmp(text, "return") == 0) {
					addTk(RETURN);
				}
				else if (isSomeNumber(text)) {
					if (isNumberInteger(text)) {
						tk = addTk(INT);
						tk->i = atoi(text);
					}
					else {
						tk = addTk(REAL);
						tk->r = atof(text);
					}

				}
				else {
					tk = addTk(ID);
					strcpy(tk->text, text);
				}
			}
			}
			else err("invalid char: %c (%d)", *pch, *pch); //daca nu am implementat in swtich caracterul dat de printf
		}
	}
}

void showTokens() {
	enum Tokens myToken;
	for (int i = 0; i < nTokens; i++) {
		Token *tk = &tokens[i];
		if (tk->code==0) {//verifica daca e id ca sa ii ataseze si labelul
			printf("line:%d and token:%s:%s\n", tk->line, enumLabels[tk->code],tk->text);
		}
		else if (tk->code == 30) {//verifica daca din enum are codul de INT
			printf("line:%d and token:%s:%d\n", tk->line, enumLabels[tk->code], tk->i);
		}
		else if (tk->code == 31) {//verifica daca din enum are codul de REAL
			printf("line:%d and token:%s:%f\n", tk->line, enumLabels[tk->code], tk->r);
		}
		else if (tk->code == 32) {//verfica daca din enum are codul de STR
			printf("line:%d and token:%s:%s\n", tk->line, enumLabels[tk->code], tk->text);
		}
		else {//printarea de toate zilele pentru tot felu de caractere 
			printf("line:%d and token:%s\n", tk->line, enumLabels[tk->code]);
		}
	}
}
