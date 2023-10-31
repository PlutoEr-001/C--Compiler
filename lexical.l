%{
#include "ast.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "sytanx.tab.h"
void make_node();
void lex_error(char* s);
int yycolumn = 1;

const char* nt_token_name[] = {
        "Program", "ExtDefList", "ExtDef", "ExtDecList", "Specifier",
        "StructSpecifier", "OptTag", "Tag", "VarDec", "FunDec",
        "VarList", "ParamDec", "CompSt", "StmtList", "Stmt",
        "DefList", "Def", "DecList", "Dec", "Exp", "Args"
};
const char* t_token_name[] = {
        "INT","FLOAT","ID","SEMI","COMMA","ASSIGNOP", "RELOP", "PLUS", "MINUS","STAR",
        "DIV", "AND","OR", "DOT","NOT", "TYPE","LP", "RP", "LB", "RB","LC", "RC",
        "STRUCT","RETURN", "IF", "ELSE", "WHILE",
};
enum TERM_TOKEN{
        _INT,_FLOAT,_ID,_SEMI,_COMMA,_ASSIGNOP, _RELOP, _PLUS, _MINUS,_STAR,
        _DIV, _AND,_OR, _DOT,_NOT, _TYPE,_LP, _RP, _LB, _RB,_LC, _RC,
        _STRUCT,_RETURN, _IF, _ELSE, _WHILE,
        };
#define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = yycolumn; \
    yylloc.last_column = yycolumn + yyleng - 1; \
    yycolumn += yyleng;
%}

Digit   [0-9]
Letter   [a-zA-Z_]
Hex   [a-fA-F0-9]
Exp   [Ee][+-]?{Digit}+

%%

[+]?({Digit}*\.{Digit}+|{Digit}+\.)({Exp})?  {make_node(_FLOAT);return FLOAT;}

0[0-7]+ { make_node(_INT);return INT;}
0[xX]{Hex}+ { make_node(_INT);return INT;}
0|([1-9]{Digit}*) { make_node(_INT);return INT;}   //09
("int")|("float") {make_node(_TYPE);return TYPE;}
"struct" { make_node(_STRUCT);return STRUCT; }
"return" { make_node(_RETURN); return RETURN;}
"if" { make_node(_IF); return IF;}
"else" { make_node(_ELSE); return ELSE;}
"while" { make_node(_WHILE);return WHILE; }
{Letter}({Letter}|{Digit})* {make_node(_ID);return ID;}
";" { make_node(_SEMI);return SEMI; }
"," {  make_node(_COMMA);return COMMA;}
"=" { make_node(_ASSIGNOP);return ASSIGNOP; }
(">=")|("<=")|("==")|("!=")|("<")|(">") { make_node(_RELOP); return RELOP;}
"+" { make_node(_PLUS); return PLUS;}
"-" { make_node(_MINUS); return MINUS;}
"*" { make_node(_STAR); return STAR;}
"/" { make_node(_DIV); return DIV;}
"&&" { make_node(_AND); return AND;}
"||" { make_node(_OR); return OR;}
"." { make_node(_DOT); return DOT;}
"!" { make_node(_NOT); return NOT;}
"(" { make_node(_LP); return LP;}
")" { make_node(_RP); return RP;}
"[" { make_node(_LB); return LB;}
"]" { make_node(_RB); return RB;}
"{" { make_node(_LC); return LC;}
"}" { make_node(_RC); return RC;}
\n {yylineno++;yycolumn = 1;}
"//" {
    char c;
    while (c != '\n') c = input();
    unput(c);
}
"/*" {
    char m,n;
    m=input();
    do {
        n=m;
        m=input();
        if(n=='\n') yylineno++;
    } while ((n != '*' ) || (m != '/' ));
}
[\t ] {}  //space
. {
    printf("%s",yytext);
    lex_error(yytext);
}
%%
extern int no_error;
void make_node(enum TERM_TOKEN token_){
    //char* temp=strdup(yytext);
    //printf("%s",temp);
    union ast_value value;
    switch(token_){
        case _INT:
            value.int_value = strtol(yytext, NULL,0);
            yylval.node = make_ast_node(INT_,value);
            break;
        case _FLOAT:
            value.float_value = atof(yytext);
            yylval.node = make_ast_node(FLOAT_,value);
            break;
        case _ID:
            value.str_value=strdup(yytext);
            yylval.node = make_ast_node(ID_,value);
            break;
        case _TYPE:
            value.str_value=strdup(yytext);
            yylval.node = make_ast_node(TYPE_,value);
            break;
        case _RELOP:
            value.str_value=strdup(yytext);
            yylval.node=make_ast_node(RELOP_,value);
            break;
        default:
            value.str_value=strdup(t_token_name[token_]);
            yylval.node = make_ast_node(OTHER_,value);
            break;
    }
}
void lex_error(char* s) {
    no_error = 0;
    printf("Error type A at Line %d: %s\n", yylloc.first_line, s);
}
