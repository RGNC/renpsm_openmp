/* 
 * renpsm.l:
 *
 * This file contains the FLEX lexical parser for RENPSM models P-Lingua
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

%option noyywrap
%{
	#include <stdio.h>
	#define YY_DECL int yylex()
	#include "y.tab.h" 
%}
%option yylineno
%x IN_COMMENT1 IN_COMMENT2


blank			([ \t\n])+
letter			[a-zA-Z]
digit			[0-9]
identifier		{letter}(({letter}|"_")|{digit})*
integer			{digit}({digit})*
real			{integer}"."{integer}


%%
<INITIAL>{
"/*"           			BEGIN(IN_COMMENT1);
"//"					BEGIN(IN_COMMENT2);
{blank}					{/* omit */}
"<"						{return (LT);}
">"						{return (GT);}
"("						{return (LPAR);}
")"						{return (RPAR);}
"{"						{return (LBRACE);}
"}"						{return (RBRACE);}
"["						{return (LSQ);}
"]"						{return (RSQ);}
","						{return (COMMA);}
";"						{return (SEMICOLON);}
"="						{return (ASIG);}
"+"						{return (ADD);}
"-"						{return (SUB);}
"*"						{return (MUL);}
"/"						{return (DIV);}
"%"						{return (MOD);}
"in"					{return (IN);}
"def"					{return (DEF);}
"call"					{return (CALL);}
"@model"				{return (MODEL);}
"@mu"					{return (MU);}
"@ms"					{return (MS);}
{integer}				{yylval.intValue = atoi(yytext); return (INTEGER);}
{real}					{yylval.doubleValue = atof(yytext); return (REAL);}
{identifier}			{yylval.stringValue = strdup(yytext); return (ID);}
"'"						{return (QUOTE);}
"<-"					{return (LARROW);}
"->"					{return (RARROW);}
":"						{return (COLON);}
"<="					{return (LE);}
">="					{return (GE);}
"=="					{return (EQ);}
"!="					{return (NEQ);}
"!"						{return (NOT);}
"&&"					{return (AND);}
"||"					{return (OR);}
"@"						{return (ARROBA);}
"?"						{return (QUESTION);}
.						ECHO;
}

<IN_COMMENT1>{
"*/"      BEGIN(INITIAL);
[^*]+   
"*"       
}

<IN_COMMENT2>{
"\n" BEGIN(INITIAL);
[^*\n]+
"*"
}

%% 
