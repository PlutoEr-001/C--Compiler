%locations
%define parse.error verbose
%{
#include "ast.h"
#include "lex.yy.c"
void yyerror (const char *);
enum NT_TOKEN{Program,ExtDefList,ExtDef,ExtDecList,Specifier,
	StructSpecifier,OptTag,Tag,VarDec,FunDec,VarList,ParamDec,
	CompSt,StmtList,Stmt,DefList,Def,DecList,Dec,Exp,Args,
};
%}
%union{
	struct Node *node;
}
%right <node> ASSIGNOP

%left <node> OR

%left <node> AND

%left <node> RELOP

%left <node> MINUS PLUS

%left <node> STAR DIV

%right <node> NOT

%left <node> LP RP LB RB DOT

%nonassoc LOWER_THAN_ELSE
%token <node> INT FLOAT TYPE ID STRUCT RETURN IF WHILE ELSE
%type <node> Program ExtDefList ExtDef ExtDecList Specifier StructSpecifier OptTag Tag VarDec FunDec VarList ParamDec CompSt StmtList Stmt DefList Def DecList Dec Exp Args
%nonassoc <node> SEMI COMMA
%nonassoc  STRUCT RETURN IF ELSE WHILE TYPE
%left  <node> LC RC
%%
Program: ExtDefList {
    union ast_value value;
    value.str_value=strdup(nt_token_name[Program]);
    $$ = make_ast_node(OTHER_,value);
    ast_root = $$;
    add_children($$, 1, $1);
    }
    ;
ExtDefList: ExtDef ExtDefList {
    union ast_value value;
    value.str_value=strdup(nt_token_name[ExtDefList]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 2, $1, $2);
    }
    |  { $$ = NULL; }
    ;
ExtDef: Specifier ExtDecList SEMI {
    union ast_value value;
    value.str_value=strdup(nt_token_name[ExtDef]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 3, $1, $2, $3);
    }
    | Specifier SEMI {
    union ast_value value;
    value.str_value=strdup(nt_token_name[ExtDef]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 2, $1, $2);
    }
    | Specifier FunDec CompSt {
    union ast_value value;
    value.str_value=strdup(nt_token_name[ExtDef]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 3, $1, $2, $3);
    }
    | Specifier FunDec SEMI {
    union ast_value value;
    value.str_value=strdup(nt_token_name[ExtDef]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 3, $1, $2, $3);
    }
    | error SEMI { yyerrok; }
    ;
ExtDecList: VarDec {
    union ast_value value;
    value.str_value=strdup(nt_token_name[ExtDecList]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 1, $1);
    }
    | VarDec COMMA ExtDecList {
    union ast_value value;
    value.str_value=strdup(nt_token_name[ExtDecList]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 3, $1, $2, $3);
    }
    ;
Specifier: TYPE {
    union ast_value value;
    value.str_value=strdup(nt_token_name[Specifier]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 1, $1);
    //printf("888888888888%s",$1->value.str_value);
    }
    | StructSpecifier {
    union ast_value value;
    value.str_value=strdup(nt_token_name[Specifier]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 1, $1);
    }
    ;
StructSpecifier: STRUCT OptTag LC DefList RC {
    union ast_value value;
    value.str_value=strdup(nt_token_name[StructSpecifier]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 5, $1, $2, $3, $4, $5);
    }
    | STRUCT Tag {
    union ast_value value;
    value.str_value=strdup(nt_token_name[StructSpecifier]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 2, $1, $2);
    }
    ;
OptTag: ID {
    union ast_value value;
    value.str_value=strdup(nt_token_name[OptTag]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 1, $1);
    }
    |  { $$ = NULL; }
    ;
Tag: ID {
    union ast_value value;
    value.str_value=strdup(nt_token_name[Tag]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 1, $1);
    }
    ;
VarDec: ID {
    union ast_value value;
    value.str_value=strdup(nt_token_name[VarDec]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 1, $1);
    }
    | VarDec LB INT RB {
    union ast_value value;
    value.str_value=strdup(nt_token_name[VarDec]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 4, $1, $2, $3, $4);
    }
    ;
FunDec: ID LP VarList RP {
    union ast_value value;
    value.str_value=strdup(nt_token_name[FunDec]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 4, $1, $2, $3, $4);
    }
    | ID LP RP {
    union ast_value value;
    value.str_value=strdup(nt_token_name[FunDec]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 3, $1, $2, $3);
    }
    ;
VarList: ParamDec COMMA VarList {
    union ast_value value;
    value.str_value=strdup(nt_token_name[VarList]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 3, $1, $2, $3);
    }
    | ParamDec {
    union ast_value value;
    value.str_value=strdup(nt_token_name[VarList]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 1, $1);
    }
    ;
ParamDec: Specifier VarDec {
    union ast_value value;
    value.str_value=strdup(nt_token_name[ParamDec]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 2, $1, $2);
    }
    | Specifier {
    union ast_value value;
    value.str_value=strdup(nt_token_name[ParamDec]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 2, $1);
    }
    ;

/* Statements */
CompSt: LC DefList StmtList RC {
    union ast_value value;
    value.str_value=strdup(nt_token_name[CompSt]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 4, $1, $2, $3, $4);
    }
    | error RC { yyerrok; }
    ;
StmtList: Stmt StmtList {
    union ast_value value;
    value.str_value=strdup(nt_token_name[StmtList]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 2, $1, $2);
    }
    |  { $$ = NULL; }
    ;
Stmt: Exp SEMI {
    union ast_value value;
    value.str_value=strdup(nt_token_name[Stmt]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 2, $1, $2);
    }
    | CompSt {
    union ast_value value;
    value.str_value=strdup(nt_token_name[Stmt]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 1, $1);
    }
    | RETURN Exp SEMI {
    union ast_value value;
    value.str_value=strdup(nt_token_name[Stmt]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 3, $1, $2, $3);
    }
    |RETURN error SEMI{yyerrok;}
    | IF LP Exp RP Stmt {
    union ast_value value;
    value.str_value=strdup(nt_token_name[Stmt]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 5, $1, $2, $3, $4, $5);
    }
    | IF LP Exp RP Stmt ELSE Stmt %prec LOWER_THAN_ELSE {
    union ast_value value;
    value.str_value=strdup(nt_token_name[Stmt]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 7, $1, $2, $3, $4, $5, $6, $7);
    }
    | WHILE LP Exp RP Stmt {
    union ast_value value;
    value.str_value=strdup(nt_token_name[Stmt]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 5, $1, $2, $3, $4, $5);
    }
    | error SEMI { yyerrok; }
    ;
DefList: Def DefList {
    union ast_value value;
    value.str_value=strdup(nt_token_name[DefList]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 2, $1, $2);
    }
    | { $$ = NULL; }
    ;
Def: Specifier DecList SEMI {
    union ast_value value;
    value.str_value=strdup(nt_token_name[Def]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 3, $1, $2, $3);
    }
    | error SEMI { yyerrok; }
    ;
DecList: Dec {
    union ast_value value;
    value.str_value=strdup(nt_token_name[DecList]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 1, $1);
    }
    | Dec COMMA DecList {
    union ast_value value;
    value.str_value=strdup(nt_token_name[DecList]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 3, $1, $2, $3);
    }
    ;
Dec: VarDec {
    union ast_value value;
    value.str_value=strdup(nt_token_name[Dec]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 1, $1);
    }
    | VarDec ASSIGNOP Exp {
    union ast_value value;
    value.str_value=strdup(nt_token_name[Dec]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 3, $1, $2, $3);
    }
    ;
Exp: Exp ASSIGNOP Exp {//a= =3)
    union ast_value value;
    value.str_value=strdup(nt_token_name[Exp]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 3, $1, $2, $3);
    }
    | Exp AND Exp {
    union ast_value value;
    value.str_value=strdup(nt_token_name[Exp]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 3, $1, $2, $3);
    }
    | Exp OR Exp {
    union ast_value value;
    value.str_value=strdup(nt_token_name[Exp]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 3, $1, $2, $3);
    }
    | Exp RELOP Exp {
    union ast_value value;
    value.str_value=strdup(nt_token_name[Exp]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 3, $1, $2, $3);
    }
    | Exp PLUS Exp {
    union ast_value value;
    value.str_value=strdup(nt_token_name[Exp]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 3, $1, $2, $3);
    }
    | Exp MINUS Exp {
    union ast_value value;
    value.str_value=strdup(nt_token_name[Exp]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 3, $1, $2, $3);
    }
    | Exp STAR Exp {
    union ast_value value;
    value.str_value=strdup(nt_token_name[Exp]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 3, $1, $2, $3);
    }
    | Exp DIV Exp {
    union ast_value value;
    value.str_value=strdup(nt_token_name[Exp]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 3, $1, $2, $3);
    }
    | LP Exp RP {
    union ast_value value;
    value.str_value=strdup(nt_token_name[Exp]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 3, $1, $2, $3);
    }
    | MINUS Exp {
    union ast_value value;
    value.str_value=strdup(nt_token_name[Exp]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 2, $1, $2);
    }
    | NOT Exp {
    union ast_value value;
    value.str_value=strdup(nt_token_name[Exp]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 2, $1, $2);
    }
    | ID LP Args RP {
    union ast_value value;
    value.str_value=strdup(nt_token_name[Exp]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 4, $1, $2, $3, $4);
    }
    | ID LP RP {
    union ast_value value;
    value.str_value=strdup(nt_token_name[Exp]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 3, $1, $2, $3);
    }
    | Exp LB Exp RB {
    union ast_value value;
    value.str_value=strdup(nt_token_name[Exp]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 4, $1, $2, $3, $4);
    }
    | Exp DOT ID {
    union ast_value value;
    value.str_value=strdup(nt_token_name[Exp]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 3, $1, $2, $3);
    }
    | ID {
    union ast_value value;
    value.str_value=strdup(nt_token_name[Exp]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 1, $1);
    }
    | INT {
    union ast_value value;
    value.str_value=strdup(nt_token_name[Exp]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 1, $1);
    }
    | FLOAT {
    union ast_value value;
    value.str_value=strdup(nt_token_name[Exp]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 1, $1);
    }
    | Exp ASSIGNOP error  INT{ yyerrok; }//exp= =3
    //| error  RP{ yyerrok; }
    ;
Args: Exp COMMA Args {
    union ast_value value;
    value.str_value=strdup(nt_token_name[Args]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 3, $1, $2, $3);
    }
    | Exp {
    union ast_value value;
    value.str_value=strdup(nt_token_name[Args]);
    $$ = make_ast_node(OTHER_,value);
    add_children($$, 1, $1);
    }
    ;
%%
void yyerror(const char* s) {
    no_error = 0;
    printf("Error type B at Line %d: %s\n", yylloc.first_line, s);
}