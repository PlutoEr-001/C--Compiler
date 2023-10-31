//
// Created by pluto on 22-9-24.
//
#include <stdio.h>
#include "ast.h"
#include "sdt.h"
#include "Mips.h"
FILE *out_file;
FILE *Mips_out_file;
int no_error=1;
extern int scope_level;
struct table_item* Symbol_Hashtable[Table_size]={};
struct table_item* SymbolTable_stack[MAX_NEST_NUM]={};
struct InterCode *InterCode_list= nullptr,*list_tail= nullptr;
struct Mips *mips_list=nullptr,*mips_tail=nullptr;
void yyrestart ( FILE *input_file  );
int main(int argc, char* argv[]) {
    char *filename = NULL;
    filename = argv[1];
    //char* out_file_name=argv[2];
    char* Mips_file_name=argv[2];
    FILE* fp = fopen(filename, "r");
    //out_file= fopen(out_file_name,"w");
    Mips_out_file=fopen(Mips_file_name,"w");
    yyrestart(fp);
    yyparse();
    /*if (no_error) {
        print_ast(ast_root,0);
    }*/
    //printf("%s\n",*test_content);
    //printf("\n");
    Initialize();
    translate(ast_root);
    struct table_item* head=SymbolTable_stack[scope_level]->stack_next;
    while(head){
        if(head->type->Kind==FUNCTION){
            if(head->type->content.func.State==DECLARED){
                print_error(17,head->type->content.func.ast_node,head->name);
            }
        }
        head=head->stack_next;
    }
    //write_Ir_to_file(InterCode_list);
    translate_Ir2Mips();
    return 0;
}
