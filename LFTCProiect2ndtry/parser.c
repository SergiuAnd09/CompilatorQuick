#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include "lexer.h"
#include "ad.h"
#include "at.h"
#include "gen.h"

int iTk;	// the iterator in tokens
Token *consumed;	// the last consumed token

bool defVar();
bool defFunc();
bool block();
bool baseType();
bool block();
bool funcParams();
bool funcParam();
bool instr();
bool expr();
bool exprLogic();
bool exprAssign();
bool exprComp();
bool exprAdd();
bool exprMul();
bool exprPrefix();
bool factor();



// same as err, but also prints the line of the current token
 void tkerr(const char *fmt,...){ //asta nu trebuie explicata, ea afiseaza error in line si afiseaza linia odata
									//cu eroarea
	fprintf(stderr,"error in line %d: ",tokens[iTk].line);
	va_list va;
	va_start(va,fmt);
	vfprintf(stderr,fmt,va);
	va_end(va);
	fprintf(stderr,"\n");
	exit(EXIT_FAILURE);
	}

 const char* labels[COUNTER_ENUM] = {
		"ID",
		"TYPE_INT", "TYPE_REAL", "TYPE_STR", "RETURN", "END", "WHILE", "ELSE", "IF", "FUNCTION", "VAR",
		"COMMA", "FINISH", "COLON", "SEMICOLON", "LPAR", "RPAR",
		"ASSIGN", "EQUAL", "ADD", "SUB", "MUL", "DIV", "AND", "OR", "NOT", "NOTEQ", "LESS", "GREATER", "GREATEREQ",
		"INT", "REAL", "STR",
		"SPACE", "COMMENT", "TAB"
 };//astea sunt toate etichetele sa le numim asa mapate pe ceea ce avem noi de facut

bool consume(int code){
	//printf("\nAvem urmatorul token lexical inainte de iful din consume: %s cu valoare iterator: %d\n", labels[tokens[iTk].code], iTk);
	if(tokens[iTk].code==code){
		consumed=&tokens[iTk++];//se tine minte adresa atomului curent, se trece apoi la urmatorul token, astfel
		//printf("\CONSUMED TOKEN IN consume() ESTE: %s cu iterator:%d\n", labels[consumed->code], iTk);							//consumand atomul curent
		
		return true;
		}
	return false;
	}

// program ::= ( defVar | defFunc | block )* FINISH
bool program(){//implementare regula program
	
	addDomain(); // creates the global domain

	addPredefinedFns(); // it will be inserted after the code for domain analysis, tipuri

	crtCode = &tMain;
	crtVar = &tBegin;
	Text_write(&tBegin, "#include \"quick.h\"\n\n");
	Text_write(&tMain, "\nint main(){\n");

	for(;;){//cand for n-are conditie data se ia ca si cum ar fii bucla infinita, un while(true)
		if(defVar()){}//testam daca apelul lui defvar da true, nu mai avem ceva de facut
						//daca da fals, apelam deffunc, daca da true nu mai da nimic de facut, daca da 
						//fals atunci mai dam inca odata pe un block, daca nici asta nu e true, iesim din 
						//bucla noastra infinita
						//la sfarsitul programului mai avem de consumat atomul finish, care se consuma 
						//cu functia speciala consume(FINISH), ii spunem ce sa consume, daca reusim sa consu
						//mam acest finish, da true functia si ii da mai departe, daca nu e true ci fals
						//daca avem de ex alt atom, atunci iteratorul ramane pe pozitia unde era
						//deci in cazul asta daca am consumat finish
		else if(defFunc()){}
		else if(block()){}
		else break;
		}

	if(consume(FINISH)){
		delDomain(); // deletes the global domain

		Text_write(&tMain, "return 0;\n}\n");
		FILE *fis = fopen("1.c", "w");
		if (!fis) {
			printf("cannot write to file 1.c\n");
			exit(EXIT_FAILURE);
		}
		fwrite(tBegin.buf, sizeof(char), tBegin.n, fis);
		fwrite(tFunctions.buf, sizeof(char), tFunctions.n, fis);
		fwrite(tMain.buf, sizeof(char), tMain.n, fis);
		fclose(fis);

		return true;//am consumat toti atomii din regula, a.i. regula noastra returneaz atrue
	}
	else { tkerr("syntax error"); }//daca da fals, inseamna ca pe pozitia aia e alt atom, adica ceva cu 
									//regulile astea nu e ok, si nu s-a mai ajuns la sfarsitul programului
									//aici avem tkerr, ora viitoare vb de erori
	return false;
	}

void parse(){
	iTk=0;//iterator in sirul de atomi, in array(vector), va fi un cursor(iterator) in tokens
	program();//prima regula din regulile sintactice, avem si in laborator explicatiile astea, si la curs se fac
				//tot legat de program();, ea contine un intreg program quick, care are o alternativa
				//de forma din lab, care poate avea o definitie de var, o functie, sau un sir de instructiuni
				//analiza sintactica se face prin implementarea unei functii separate pt fiecare regula, 
				//ele returnand bool, si sa nu aiba argumente
	}

bool defVar() { //asta are iteratorul resetat
	int start = iTk;
	if (consume(VAR)) {
		//printf("\nAm fost in var in defVar\n");
		if (consume(ID)) {

			const char *name = consumed->text;//domeniu
			Symbol *s = searchInCurrentDomain(name);
			if (s)tkerr("symbol redefinition: %s", name);
			s = addSymbol(name, KIND_VAR);
			s->local = crtFn != NULL;

			//printf("\nAm fost in id in defVar\n");
			if (consume(COLON)) {
				//printf("\nAm fost in colon in defVar\n");
				if (baseType()) {
					s->type = ret.type;//domeniu
					//printf("\nAm fost in baseType in defVar\n");
					if (consume(SEMICOLON)) { 
						//printf("\nAm fost in semicolon in defVar\n");

						Text_write(crtVar, "%s %s;\n", cType(ret.type), name);

						return true;
					}
					else { tkerr("lipseste punctul si virgula de la finalul declararii variabilei"); }
				}
				else { tkerr("eroare de sintaxa la alegerea tipului de baza al variabilei"); }
			}
			else { tkerr("lipseste punctul si virgula de dupa variabila"); }
		}
		else { tkerr("lipseste id-ul variabilei"); }
	}
	iTk = start;
	return false;
}

//la baseType nu trebe coduri de eroare, exista alternative si nu se imbrica nimic	
bool baseType() {//aici nu trebe iteratorul resetat ca avem doar alternativa, adica oricum doar un atom se consuma intr-un caz
	int start = iTk;
	if (consume(TYPE_INT)) {
		//printf("\nAm fost in int in baseType si iTk e %d cu consumed %s\n", iTk, labels[consumed->code]);
		ret.type = TYPE_INT;
		return true;
	}
	if (consume(TYPE_REAL)) {
		//printf("\nAm fost in real;\n");
		ret.type = TYPE_REAL;

		return true;
	}
	if (consume(TYPE_STR)) {
		//printf("\nAm fost in str;\n");
		ret.type = TYPE_STR;
		return true;
	}

	iTk = start;//aici s-ar putea sa nu fie bine
	return false;
}

bool defFunc() {//asta are iteratorul resetat
	int start = iTk;
	if (consume(FUNCTION)) {
		if (consume(ID)) {

			const char *name = consumed->text;//domeniu
			Symbol *s = searchInCurrentDomain(name);
			if (s)tkerr("symbol redefinition: %s", name);
			crtFn = addSymbol(name, KIND_FN);
			crtFn->args = NULL;
			addDomain();

			crtCode = &tFunctions;//gen
			crtVar = &tFunctions;
			Text_clear(&tFnHeader);
			Text_write(&tFnHeader, "%s(", name);

			if (consume(LPAR)) {
				if(funcParams()){}//cum parametrii pot lipsi, nu poate fi eroare lipsa lor
				if (consume(RPAR)) {
					if (consume(COLON)) {
						if (baseType()) {

							crtFn->type = ret.type;//domeniu

							Text_write(&tFunctions, "\n%s %s){\n", cType(ret.type), tFnHeader.buf);//gen

							while (defVar()) {}//nu vine pusa tkerr ca nu e nevoie neaparat de definire de variabila
							if (block()) {
								if (consume(END)) { 

									delDomain();//domeniu
									crtFn = NULL;

									Text_write(&tFunctions, "}\n");//gen
									crtCode = &tMain;
									crtVar = &tBegin;

									return true; }
								else { tkerr("lipseste end de la finalul functiei"); }
							}
							else { tkerr("lipseste blocul de instructiuni, e nevoie de minim o ';' in functie"); }
						}
						else { tkerr("eroare de sintaxa la alegerea tipului de baza al returnului functiei"); }
					}
					else { tkerr("lipseste : dupa parantezele de parametrii din cadrul definirii functiei"); }
				}
				else { tkerr("eroare de sintaxa in interiorul parantezelor sau lipseste ) in cadrul definirii functiei"); }
			}
			else { tkerr("lipseste ( in cadrul definirii functiei"); }
		}
		else { tkerr("id-ul lipseste pentru functie"); }
	}
	iTk = start;
	return false;
}

bool block() {//aici nu trebe sa avem resetare, ca e de ajuns sa se consume o intructiune si e ok
	int start = iTk;
	if (instr()) {
		for (;;) {//aici nu se pune mesaj de erorare caci se poate merge mai departe si fara adica e practic alternativa
			if (instr()) {}
			else break;
		}
		return true;
	}
	iTk = start;
	return false;
}

bool funcParams() {//acelasi caz ca la block
	if (funcParam()) {
		for (;;) { //optional, nu e nevoie de parametrii, functia poate fi fara
			if (consume(COMMA)) {

				Text_write(&tFnHeader, ",");//gen

				if(funcParam()){}
				
			}
			else { //tkerr("lipseste ',' dintre parametrii"); 
			break; }
		}
		return true;
	}
	return false;
}

bool funcParam() {//asta are iteratorul resetat
	int start = iTk;
	if (consume(ID)) {

		const char *name = consumed->text;//domeniu
		Symbol *s = searchInCurrentDomain(name);
		if (s)tkerr("symbol redefinition: %s", name);
		s = addSymbol(name, KIND_ARG);
		Symbol *sFnParam = addFnArg(crtFn, name);

		if (consume(COLON)) {
			if (baseType()) { 

				s->type = ret.type;//domeniu
				sFnParam->type = ret.type;

				Text_write(&tFnHeader, "%s %s", cType(ret.type), name);//gen

				return true; }
			else { tkerr("eroare de sintaxa la tipul de baza al parametrului"); }
		}
		else { tkerr("dupa id-ul parametrului lipseste :"); }
	}
	iTk = start;
	return false;
}

bool instr() {//asta are iteratorul resetat
	int start = iTk;//reset
	if (consume(SEMICOLON)) { 
		Text_write(crtCode, ";\n");//gen
		return true; }//vine intai doar ; testata, apoi daca e instructiune mai complexa se accepta la urmatorul pas
	if (expr()) {
		if (consume(SEMICOLON)) { 
			Text_write(crtCode, ";\n");//gen
			return true; }
		else { tkerr("lipseste ; de dupa instructiune"); }
	}//nu vine pusa eroare ca sunt alternative si nu e imbricata
	
	//else { tkerr("lipseste ; dupa instructiune"); }

	if (consume(IF)) {
		if (consume(LPAR)) {

			Text_write(crtCode, "if(");//gen

			if (expr()) {
				if (ret.type == TYPE_STR)tkerr("the if condition must have type int or real");//tipuri

				if (consume(RPAR)) {

					Text_write(crtCode, "){\n");//gen

					if (block()) {

						Text_write(crtCode, "}\n");//gen

						if (consume(ELSE)) {//else nu e obligatoriu, e alternativa la el sa nu fie, se merge mai departe

							Text_write(crtCode, "else{\n");//gen

							if (block()){
							
								Text_write(crtCode, "}\n");//gen
							}
							else { tkerr("lipseste blocul de intructiuni din else-ul if-ului, minim sa fie o ';'"); }
						}
						if (consume(END)) { return true; }
						else { tkerr("lipseste END dupa blocul de instructiuni din if");  }
					}else{tkerr("lipseste blocul de instructiuni, e nevoie de minim o ';' in if"); }
				}else { tkerr("lipseste ) dupa expresia ifului sau expresia dintre () e ilegala"); }
			}else { tkerr("lipseste expresia dintre parentezele ifului"); }
			
		}else { tkerr("lispeste ( dupa if"); }
		
	}
	
	if (consume(RETURN)) {

		Text_write(crtCode, "return ");//gen

		if (expr()) {

			if (!crtFn)tkerr("return can be used only in a function");//tipuri
			if (ret.type != crtFn->type)tkerr("the return type must be the same as the function return type");

			if (consume(SEMICOLON)) {
				
				Text_write(crtCode, ";\n");//gen
				return true; }
			else { tkerr("lipseste ';' dupa expresia intoarsa de return"); }
		}
		else { tkerr("lipseste expresia returnului"); }
	}
	
	if (consume(WHILE)) {

		Text_write(crtCode, "while(");//gen

		if (consume(LPAR)) {
			if (expr()) {
				if (ret.type == TYPE_STR)tkerr("the while condition must have type int or real");//tipuri

				if (consume(RPAR)) {

					Text_write(crtCode, "){\n");//gen

					if (block()) {
						if (consume(END)) { 
							
							Text_write(crtCode, "}\n");//gen
							return true; }
						else { tkerr("lipseste END dupa blocul de instructiuni din while"); }
					}//else { tkerr("lipseste blocul de instructiuni, e nevoie de minim o ';' in while"); }
				}else { tkerr("lipseste ) dupa expresia while-ului sau expresia dintre () e ilegala"); }
			}else {tkerr("lipseste expresia dintre parentezele while-ului");}
		}else { tkerr("lispeste ( dupa while"); }
	}
	iTk = start;//reset done;mai sus s-a facut resetare dupa fiecare secventa, inafar de prima alternativa care nu era secventa
	return false;
}

bool expr() {//nu e nevoie de iterator resetat
	int start = iTk;
	if (exprLogic()) {
		return true;
	}
	iTk = start;
	return false;
}

bool exprLogic() {//aici nu se pune nici un reset, ca se poate consuma numa exprAssign si merge
	int start = iTk;
	if (exprAssign()) {
		for (;;) {
			if (consume(AND)) {//nu vine pus nimic aici, e alternativa 

				Ret leftType = ret;//tipuri
				if (leftType.type == TYPE_STR)tkerr("the left operand of && or || cannot be of type str");

				Text_write(crtCode, "&&");//gen

				if (exprAssign()) {
				
					if (ret.type == TYPE_STR)tkerr("the right operand of && or || cannot be of type str");//tipuri
					setRet(TYPE_INT, false);
				}
			} 
			if (consume(OR)) {//nu vine nimic pus aici, e alternativa

				Ret leftType = ret;//tipuri
				if (leftType.type == TYPE_STR)tkerr("the left operand of && or || cannot be of type str");//tipuri

				Text_write(crtCode, "||");//gen

				if (exprAssign()) {
				
					if (ret.type == TYPE_STR)tkerr("the right operand of && or || cannot be of type str");//tipuri
					setRet(TYPE_INT, false);
				}
			}
			else break;
		}
		return true;//se ajunge aici indiferent de cate ori se intampla ce e mai sus, ca mai sus e optional(*)
	}
	iTk = start;
	return false;
}

//bool exprAssign() {//asta are iteratorul resetat, AICI EXEMPLU LUATE SI LA ALTE OPTIONALITATI
//	int start = iTk;
//	if (consume(ID)) {
//		
//
//		if (consume(ASSIGN)) {}
//		else { iTk = start; 
//		//tkerr("lipseste '=' dupa id cand se face atribuirea"); 
//		}
//	}
//	//iTk = start;
//	if (exprComp()) { return true; }
//	return false;
//}

bool exprAssign() {//asta are iteratorul resetat, aici exemplu luate si la alte optionalitati
	int start = iTk;
	if (consume(ID)) {

		const char *name = consumed->text;//tipuri

		if (consume(ASSIGN)) {

			Text_write(crtCode, "%s=", name);//gen

			if (exprComp()) {

				Symbol *s = searchSymbol(name);//tipuri
				if (!s)tkerr("undefined symbol: %s", name);
				if (s->kind == KIND_FN)tkerr("a function (%s) cannot be used as a destination for assignment ", name);
				if (s->type != ret.type)tkerr("the source and destination for assignment must have the same type");
				ret.lval = false;

				return true;
			}
			else { tkerr("lipseste expresia atribuirii"); }
		}
	}
	iTk = start;//pus aici caci dupa ce iese din secventa, sa nu ramana incrementat iTk cu ceea ce nu s-a indeplinit
	if (exprComp()) { return true; }
	//iTk = start;//aici poate mai vine pus unu, cine stie
	return false;
}

bool exprComp() {//aici posibil sa nu fie nevoie de reset, dar de REVAZUT
	int start = iTk;
	if (exprAdd()) {
		if (consume(LESS)) {

			Ret leftType = ret;//tipuri

			Text_write(crtCode, "<");//gen

			if (exprAdd()) {
				if (leftType.type != ret.type)tkerr("different types for the operands of < or ==");//tipuri
				setRet(TYPE_INT, false); // the result of comparation is int 0 or 1
			}
		}
		if (consume(EQUAL)) {
			Ret leftType = ret;

			Text_write(crtCode, "==");//gen

			if (exprAdd()) {
				if (leftType.type != ret.type)tkerr("different types for the operands of < or ==");
				setRet(TYPE_INT, false); // the result of comparation is int 0 or 1
			}
		}
		return true;
	}
	iTk = start;
	return false;
}

bool exprAdd() {//aici la fel ca la exprComp
	int start = iTk;
	if (exprMul()) {
		for (;;) {
			if (consume(ADD)) {
				Ret leftType = ret;//tipuri
				if (leftType.type == TYPE_STR)tkerr("the operands of + or - cannot be of type str");

				Text_write(crtCode, "+");//gen

				if (exprMul()) {
					if (leftType.type != ret.type)tkerr("different types for the operands of + or -");//tipuri
					ret.lval = false;
				}
			}
			if (consume(SUB)) {

				Ret leftType = ret;//tipuri
				if (leftType.type == TYPE_STR)tkerr("the operands of + or - cannot be of type str");

				Text_write(crtCode, "-");//gen

				if (exprMul()) {
					if (leftType.type != ret.type)tkerr("different types for the operands of + or -");//tipuri
					ret.lval = false;
				}
			}
			else break;
		}
		return true;//se ajunge aici indiferent de ce se intampla mai sus, caci mai sus e optional (*)
	}
	iTk = start;
	return false;
}

bool exprMul() {//la fel ca la exprComp
	int start = iTk;
	if (exprPrefix()) {
		for (;;) {
			if (consume(MUL)) {
				Ret leftType = ret;

				if (leftType.type == TYPE_STR)tkerr("the operands of * or / cannot be of type str");

				Text_write(crtCode, "*");//gen

				if(exprPrefix()){
					if (leftType.type != ret.type)tkerr("different types for the operands of * or /");
					ret.lval = false;
				}
			}
			if (consume(DIV)) {
				Ret leftType = ret;

				if (leftType.type == TYPE_STR)tkerr("the operands of * or / cannot be of type str");

				Text_write(crtCode, "/");//gen

				if (exprPrefix()) {
					if (leftType.type != ret.type)tkerr("different types for the operands of * or /");
					ret.lval = false;
				}
			}
			else break;
		}
		return true;
	}
	iTk = start;
	return false;
}

//bool exprPrefix() {//aici mu trebe reset
//	int start = iTk;
//	if (consume(SUB)) {}
//	if (consume(NOT)) {}
//	if (factor()) { return true; }
//	iTk = start;
//	return false;
//}

bool exprPrefix() {//aici mu trebe reset
	int start = iTk;//nu stiu daca mai trebe sa modific itk si starturi, se poate
	if (consume(SUB)) {

		Text_write(crtCode, "-");//gen

		if (factor()) { 
			if (ret.type == TYPE_STR)tkerr("the expression of unary - must be of type int or real");
			ret.lval = false;
			return true; }
	}
	iTk = start;//asta se poate dovedi problematica
	if (consume(NOT)) {

		Text_write(crtCode, "!");//gen

		if (factor()) {
			if (ret.type == TYPE_STR)tkerr("the expression of ! must be of type int or real");
			setRet(TYPE_INT, false);
			return true;
		}
	}
	iTk = start;//asta se poate dovedi problematica
	if (factor()) { return true; }
	iTk = start;
	return false;
}

bool factor() {
	int start = iTk;
	if (consume(INT)) {
		setRet(TYPE_INT, false);

		Text_write(crtCode, "%d", consumed->i);//gen

		return true;
	}
	if (consume(REAL)) {
		setRet(TYPE_REAL, false);

		Text_write(crtCode, "%g", consumed->r);//gen

		return true;
	}
	if (consume(STR)) {
		setRet(TYPE_STR, false);

		Text_write(crtCode, "\%s\"", consumed->text);//gen

		return true;
	}
	if (consume(LPAR)) {//aici pus reset
		
		Text_write(crtCode, "(");//gen

		if (expr()) {
			if (consume(RPAR)) {
				
				Text_write(crtCode, ")");//gen
				return true; }
			else { tkerr("lipseste ) a expresiei sau e eroare de sintaxa in cadrul parantezelor"); }
		}
		
	}
	if (consume(ID)) {//aici iarasi nu imi e clar
		Symbol *s = searchSymbol(consumed->text);
		if (!s)tkerr("undefined symbol: %s", consumed->text);

		Text_write(crtCode, "%s", s->name);//gen

		if (consume(LPAR)) {//asta toata poate lipsi, poate fi doar id-ul
			if (s->kind != KIND_FN)tkerr("%s cannot be called, because it is not a function", s->name);
			Symbol *argDef = s->args;

			Text_write(crtCode, "(");//gen

			if (expr()) {
				if (!argDef)tkerr("the function %s is called with too many arguments", s->name);
				if (argDef->type != ret.type)tkerr("the argument type at function %s call is different from the one given at its definition",s->name);
					argDef = argDef->next;

				for (;;) {
					if (consume(COMMA)) {

						Text_write(crtCode, ",");//gen

						if(expr()){
							if (!argDef)tkerr("the function %s is called with too many arguments", s->name);
							if (argDef->type != ret.type)tkerr("the argument type at function %s call is differentfrom the one given at its definition",s->name);
								argDef = argDef->next;
						}
					}
					else { //tkerr("lipseste ',' de la factori"); 
					break; }
				}
			}//asta e optionalu ?
			if (consume(RPAR)) { 
				if (argDef)tkerr("the function %s is called with too few arguments", s->name);
				setRet(s->type, false);

				Text_write(crtCode, ")");//gen

				return true; }//poate lipsi, adica nu e nevoie sa
			
		}
		//else { tkerr("lipseste ( de dupa id-ul apelului"); }
		if (1) {//epsilon
			if (s->kind == KIND_FN)tkerr("the function %s can only be called", s->name);
			setRet(s->type, true);
			return true;
		}
		//return true;
	}

	iTk = start;
	return false;
}