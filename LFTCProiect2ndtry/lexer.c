#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "lexer.h"
#include "utils.h"
#include <float.h>

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

int againpeInteger(char sir[100])//nu e folosit deocamdata
{
	int i = 0;
	while (sir[i] != '\0')
	{
		if (isdigit(sir[i]))
		{

		}
		else
		{
			return 0;
		}
		i++;
	}

	return 1;
}

int againlaagainpefloat(char sir[100]) //nu e folosit deocamdata
{
	int i = 0;
	while (sir[i] != '\0')
	{
		if (isdigit(sir[i]) || sir[i] == '.')
		{

		}
		else
		{
			return 0;
		}
		i++;
	}

	return 1;
}

int isInteger(int number) { //nu e folosit deocamdata
	// Check if the absolute difference between the original number and its integer version is very close to zero
	return fabs(number - (int)number) < DBL_EPSILON;
}

int isNumberIntegerr(char *sir) {//nu e folosit deocamdata
	if (strlen(sir)>1 || strlen(sir)==0) {
		return 0;
	}
	else if (isdigit(sir)) {
		return 1;
	}

	
	
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


//int isFloatingPoint(char *str) {
//	char *endptr;
//	double value = strtod(str, &endptr);
//	
//
//	// Check if the conversion was successful
//	if (endptr == str) {
//		return 0; // Not a valid floating-point number
//	}
//
//	// Check if there are any non-whitespace characters left in the string
//	while (*endptr != '\0') {
//		if (!isspace(*endptr)) {
//			return 0; // Not a valid floating-point number
//		}
//		endptr++;
//	}
//
//	return 1; // Valid floating-point number
//}

int isFloatingPoint(char *sir) {//nu e folosita deocamdata

	int valoare = 1;
	int i = 0;
	while (*(sir + i) != '\0') {
		if (*(sir + i) == '.' || isdigit(*(sir+i))) {
			
		}
		else {
			valoare = 0;
		}
	}
	return valoare;
}

void tokenize(const char *pch) { //aici o sa ne scriem noi toata functia la ce avem de lucru la acest lab
	//pch e present character din file sau textul dat
	int prima = 1;//pentru prima " a unui sir, la a doua ia valoarea 2
	int numarSchimbat = 0;//pentru functia isNumberInteger
	const char *start;//startul textului ce se analizeaza
	int contor=1;//pentru sir
	int contoras=0;//pentru comentarii, NEFOLOSIT 
	const char *inceput;//in locul lui am facut aritmetica pe pointeri de la stringuri
	Token *tk;//pentru afisari si assignari
	char buf[MAX_STR + 1];//initial de folosit la strings, renuntat la idee
	char buffer[MAX_STR + 1];//nefolosit, folosit pana la urma doar buf si la strings
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
			/*case '&&':
				addTk(AND);
				pch++;
				break;*/
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
		/*case '"': if (prima == 2) {
			;
		}*/
				 // break;
		/*case '"': 

			break;*/
			//pch = pch - contoras;
			//printf
		default:
			if (isalpha(*pch) || *pch == '_' || isdigit(*pch)  || *pch == '"') { //pt // ||isdigit(*pch) adaugat, || *pch=='.' //e de primul caracter, ce poate sa fie ca sa fie luat in considerare



				if (*pch == '"' && prima==1) {
					prima = 2;//aici daca a gasit o ghilimea ia val 2, adica valoarea ca sa nu mai vina aici
					//for (start = pch++; isalnum(*pch) || *pch == '_' || *pch==' '|| *pch=='\t'; pch++,contor++) {}//aici se afla marginile textului ce va fi pus intr-o unitate manevrabila
					for (start = pch++; *pch != '"'; pch++, contor++) { if (*pch == '\0') err("\nEnd of file error."); }
					char *text = copyn(buf, start, pch);
					//printf("\nCat e contoru: %d si textu %s si caracteru vietii:%c\n", contor, text, *(pch));
					//if (*pch=='"') {
					//	//printf("\nadi\n");
					//	/*char *text = copyn(buf, start, (pch+1));
					//	printf("\nCat e contoru: %d si textu %s\n", contor, text);*/
					//}
					
					
				}
				else if (*pch == '"' && prima == 2) {
					prima = 1;
					char *text = copyn(buf, (pch-contor), (pch));//aici
					pch++;
					//printf("\nCat e contoru: %d si textu %s\n", contor, text);//1
					tk = addTk(STR);
					strcpy(tk->text, text);
					contor = 0;
				}


				else {
					//prima = 1;//aici reia valoarea de 1 pentru ca poate urma un alt sir
				// char *c = pch; *c = *pch;
				//printf("\nC e egal cu : %c\n", *c);
				//for (inceput = c++; *c != '"' && *c!=EOF &&*c!='\0'; c++) {  }
				//char *sir = copyn(buffer, inceput, c);//aici punem sirul 
				//printf("\nSirul pus in *sir este %s: ", *sir);
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
				/*else if (isInteger(isdigit(atoi(text))) {
					addTk(INT);
				}*//*else if(isNumberInteger(text)){
					addTk(INT);*/

				//else if (isNumberInteger(text)) {//daca nu merge inapoi la isNumberInteger(text)
				//	//printf("\nAvem urmatorul text la integer: %s\n", text);
				//	tk = addTk(INT);
				//	tk->i = atoi(text);

				//}
				//else if (isFloatingPoint(text)) {//aici am comentat inainte de marea revelatie cu float
					//tk = addTk(REAL);
					//tk->r = atof(text);
					//printf("\nAvem urmatorul text la real: %s\n", text);
				//}//s-ar putea sa trebuiasca un if else compus care sa acopere odata int si odata real,
				//ca sa se poata testa ambele cazuri caci ambele au in compozitie un integer la inceput

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

			/*else if (strcmp(text, "long double") == 0) {
				addTk(TYPE_REAL);
			}*/ //nu mai fac ca trebe cautat si inainte de double dinainte sa fie conditia sa nu aiba "long" inainte
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

const char* enumNames[COUNTER_ENUM] = {//inutil
	"ID",
	"FUNCTION",
	"INT",
	"FINISH"
};

const char* enumLabels[COUNTER_ENUM] = {
		"ID",
		"TYPE_INT", "TYPE_REAL", "TYPE_STR", "RETURN", "END", "WHILE", "ELSE", "IF", "FUNCTION", "VAR",
		"COMMA", "FINISH", "COLON", "SEMICOLON", "LPAR", "RPAR",
		"ASSIGN", "EQUAL", "ADD", "SUB", "MUL", "DIV", "AND", "OR", "NOT", "NOTEQ", "LESS", "GREATER", "GREATEREQ",
		"INT", "REAL", "STR",
		"SPACE", "COMMENT", "TAB"
};

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
