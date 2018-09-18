/* 
 * renpsm.y:
 *
 * This file contains the BISON grammar for RENPSM models P-Lingua
 * 
 * More information can be found in:
 * 
 * I. Perez-Hurtado, G. Zang, M.J. Perez-Jimenez, D. Orellana
 * Simulation of Rapidly-Exploring Random Trees in Membrane Computing 
 * with P-Lingua and Automatic Programing
 * International Journal of Computers, Communications and Control, in press.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Copyright (C) 2018  Ignacio Perez-Hurtado (perezh@us.es)
 *                     Research Group On Natural Computing
 *                     http://www.gcn.us.es
 *
 * You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 */

%{
#include <stdio.h>
#include <stdlib.h>
#include "renpsm_parser.h"
#include "gen_c.h"

#define YYERROR_VERBOSE

DEFINITIONS* code = NULL;

extern int yylex();
extern int yyparse();
extern FILE* yyin;
void yyerror(const char* s);
extern FILE* yyin;
extern int yylineno;
extern char* yytext;

%}


%token MODEL INTEGER REAL ID LT GT DEF LPAR RPAR LBRACE 
       RBRACE COMMA SEMICOLON ASIG LSQ RSQ CALL MU QUOTE
       ADD SUB MUL DIV MOD MS LARROW COLON IN LE GE 
       EQ NEQ ARROBA RARROW OBJECT FUNCTION INIT_VARIABLE QUESTION
       PRODUCTION_RULE CREATION_RULE EVOLUTION_RULE 
       SET_ITERATOR RANGE_ITERATOR

%left NOT
%left AND OR        
%left EQ NEQ LE GE LT GT       
%left SUB ADD 
%left MUL DIV MOD 

%type <intValue> INTEGER
%type <doubleValue> REAL
%type <stringValue> ID
%type <definitionValue> definition instructions
%type <definitionsValue> definitions code
%type <paramsValue> params paramlist;
%type <instructionValue> instruction asig call mu ms production_rule creation_rule evolution_rule
%type <exprValue> expr limit label object multiobject;
%type <iteratorsValue> iterators itlist;
%type <iteratorValue> iterator;
%type <argumentsValue> args argslist multiset;
%type <membraneValue> membrane membranelist membranes

%union
{
    int intValue;
    double doubleValue;
    char* stringValue;
    struct Code* codeValue;
    struct Definition* definitionValue;
    struct Definitions* definitionsValue;
    struct Params* paramsValue;
    struct Instruction* instructionValue;
    struct Expr* exprValue;
    struct Iterators* iteratorsValue;
    struct Iterator* iteratorValue;
    struct Arguments* argumentsValue;
    struct Membrane* membraneValue;
}

%start code

%%
code : MODEL LT ID GT definitions 
		{code = createCode($3,$5);}

definitions : definitions definition {$$ = addDefinition($1,$2);}
			| definition {$$ = createDefinitions($1);}
			;

definition : DEF ID LPAR params RPAR LBRACE instructions RBRACE 
			{$$ = createDefinition($2,$4,$7);}

 
params : {$$ = createEmptyParams();}
       | paramlist {$$ = $1;}
       ;

paramlist : paramlist COMMA ID {$$ = addParam($1,$3);}
	      | ID {$$ = createParams($1);}
	      ;
		  
instructions : instructions instruction {$$ = addInstruction($1,$2);}
			 | instruction {$$ = createInstructions($1);}
			 ;

instruction : asig iterators {$$ = addIterators($1,$2);} 
			| call iterators {$$ = addIterators($1,$2);} 
			| mu iterators {$$ = addIterators($1,$2);} 
			| ms iterators {$$ = addIterators($1,$2);} 
			| production_rule iterators {$$ = addIterators($1,$2);} 
			| creation_rule iterators  {$$ = addIterators($1,$2);} 
			| evolution_rule iterators{$$ = addIterators($1,$2);} 
			;

iterators : SEMICOLON {$$ = createEmptyIterators();}
		  | COLON itlist SEMICOLON {$$ = $2;}
		  ;

itlist : itlist COMMA iterator {$$ = addIterator($1,$3);}
	   | iterator {$$ = createIterators($1);}
	   ;
	   
iterator : ID IN label { $$ = createSetIterator($1,$3);}
		 | limit LE ID LE limit {$$ = createRangeIterator($3,$1,$5);}
		 ; 

limit: ID {$$ = createId($1);}
	 | INTEGER {$$ = createInteger($1);}
	 | REAL {$$ = createReal($1);}
	 | SUB expr {$$ = createExpr(SUB,$2,NULL);}
	 | limit ADD limit {$$ = createExpr(ADD,$1,$3);}
	 | limit SUB limit {$$ = createExpr(SUB,$1,$3);}
	 | limit MUL limit {$$ = createExpr(MUL,$1,$3);} 
	 | limit DIV limit {$$ = createExpr(DIV,$1,$3);}
	 | limit MOD limit {$$ = createExpr(MOD,$1,$3);}
	 | LPAR limit RPAR {$$ = $2;}
	 ;
	 
		  
asig : ARROBA object ASIG expr {$$ = createAsigInst($2,$4,1);}
	 | object ASIG expr {$$ = createAsigInst($1,$3,0);}
	 ;
	 
call : CALL ID LPAR args RPAR {$$ = createCall($2,$4);}

args: {$$ = createEmptyArguments();}
     | argslist {$$ = $1;} 
     | expr COLON itlist {$$ = createIteratorArguments($1,$3);}
     ;

argslist : argslist COMMA expr {$$ = addArgument($1,$3);}
		 | expr {$$ = createArguments($1);}
		 ;

ms : MS LPAR label RPAR ASIG multiset {$$ = createMultisetInst($3,$6);}
   ;

object : ID {$$ = createObject($1,NULL);}
	   | ID LBRACE argslist RBRACE {$$ = createObject($1,$3);}
	   ;
	   
multiset : multiset COMMA multiobject {$$ = addArgument($1,$3);}
		 | multiobject {$$ = createArguments($1);}
		 ;

multiobject : object {$$ = $1;}
			| object MUL ID {$$ = addMultiplicity($1,createId($3));}
			| object MUL INTEGER {$$ = addMultiplicity($1,createInteger($3));}
			| object MUL LPAR expr RPAR {$$ = addMultiplicity($1,$4);}
			;

mu: MU ASIG membrane {$$ = createMu($3);}

membrane: LSQ membranes RSQ QUOTE label {$$ = createMembrane($2,$5);}


membranes : {$$ = NULL;} 
          | membranelist {$$ = $1;}
          ;

membranelist : membranelist membrane {$$ = addInnerMembrane($1,$2);}
			 | membrane {$$ = createInnerMembrane($1);}
			 ;


label : ID {$$ = createId($1);} 
	  | INTEGER {$$ = createInteger($1);}
	  ; 



production_rule : object LARROW expr {$$ = createProductionRule($1,$3,NULL,NULL);}
				| object LARROW expr COMMA object {$$ = createProductionRule($1,$3,$5,NULL);}
				| object LARROW expr QUESTION object {$$ = createProductionRule($1,$3,NULL,$5);}
				| object LARROW expr COMMA object QUESTION object {$$ = createProductionRule($1,$3,$5,$7);}
			    ;

creation_rule : LSQ LSQ RSQ QUOTE expr RSQ QUOTE expr {$$ = createCreationRule($5, $8,NULL,NULL);}
			  | LSQ LSQ RSQ QUOTE expr RSQ QUOTE expr COMMA object {$$ = createCreationRule($5, $8,$10,NULL);}
			  | LSQ LSQ RSQ QUOTE expr RSQ QUOTE expr QUESTION object {$$ = createCreationRule($5, $8,NULL,$10);}
			  | LSQ LSQ RSQ QUOTE expr RSQ QUOTE expr COMMA object QUESTION object {$$ = createCreationRule($5, $8,$10,$12);}
			  ;
			  

evolution_rule : LSQ object RARROW object RSQ QUOTE expr {$$ = createEvolutionRule($2, $4,$7,NULL,NULL);}
			   | LSQ object RARROW object RSQ QUOTE expr COMMA object {$$ = createEvolutionRule($2, $4,$7,$9,NULL);}
			   | LSQ object RARROW object RSQ QUOTE expr QUESTION object {$$ = createEvolutionRule($2, $4,$7,NULL,$9);}
			   | LSQ object RARROW object RSQ QUOTE expr COMMA object QUESTION object {$$ = createEvolutionRule($2, $4,$7,$9,$11);}
			   ;

expr : object {$$ = $1;}
	 | INTEGER {$$ = createInteger($1);}
	 | REAL {$$ = createReal($1);}
	 | SUB expr {$$ = createExpr(SUB,NULL,$2);}
	 | NOT expr {$$ = createExpr(NOT,NULL,$2);}
	 | ID LPAR args RPAR {$$ = createFunction($1,$3);}
	 | expr ADD expr {$$ = createExpr(ADD,$1,$3);}
	 | expr SUB expr {$$ = createExpr(SUB,$1,$3);}
	 | expr MUL expr {$$ = createExpr(MUL,$1,$3);} 
	 | expr DIV expr {$$ = createExpr(DIV,$1,$3);}
	 | expr MOD expr {$$ = createExpr(MOD,$1,$3);}
	 | expr LT expr {$$ = createExpr(LT,$1,$3);}
	 | expr GT expr {$$ = createExpr(GT,$1,$3);}
	 | expr EQ expr {$$ = createExpr(EQ,$1,$3);}
	 | expr NEQ expr {$$ = createExpr(NEQ,$1,$3);}
	 | expr LE expr {$$ = createExpr(LE,$1,$3);}
	 | expr GE expr {$$ = createExpr(GE,$1,$3);}
	 | expr AND expr {$$ = createExpr(AND,$1,$3);}
	 | expr OR expr {$$ = createExpr(OR,$1,$3);}
	 | LPAR expr RPAR {$$ = $2;}
	 ;
	 


%%

int main() {
	yyin = stdin;
	do { 
		yyparse();
	} while(!feof(yyin));
	DEFINITIONS* code1 = rollOut(code);
	code = code1;
	printTree(stdout,code);
	FILE *fp = fopen("simulator.c","w");
	generate_c_simulator(fp,code);
	fclose(fp);
	return 0;
}

void yyerror(const char* s) {
	fprintf (stderr, "Line %d: %s\n", yylineno, s);
	exit(1);
}
