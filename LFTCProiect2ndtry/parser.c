#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "lexer.h"

int iTk;	// the iterator in tokens
Token *consumed;	// the last consumed token

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

bool consume(int code){
	if(tokens[iTk].code==code){
		consumed=&tokens[iTk++];//se tine minte adresa atomului curent, se trece apoi la urmatorul token, astfel
									//consumand atomul curent
		return true;
		}
	return false;
	}

// program ::= ( defVar | defFunc | block )* FINISH
bool program(){//implementare regula program
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
		return true;//am consumat toti atomii din regula, a.i. regula noastra returneaz atrue
		}else tkerr("syntax error");//daca da fals, inseamna ca pe pozitia aia e alt atom, adica ceva cu 
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
