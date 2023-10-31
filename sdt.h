//
// Created by plutoer on 22-10-21.
//

#ifndef LAB_SDT_H
#define LAB_SDT_H
#include "symbol_table.h"
#include "IR.h"
#include <malloc.h>
void translate(struct Node* root);
FieldList translate_struct_specifier(struct Node* root);
Type translate_specifier(struct Node* root);
FieldList translate_var_dec(struct Node* root,Type var_type);
FieldList translate_dec(struct Node* root, Type var_type);
FieldList translate_dec_list(struct Node *root, Type var_type);
FieldList translate_def(struct Node* root);
FieldList translate_def_list(struct Node* root);
void translate_ext_dec_list(struct Node *root, Type var_type);
void translate_ext_def(struct Node *root);
void translate_stmt(struct Node *root);
void translate_compst(struct Node *root);
bool type_equal(Type a, Type b);
bool param_list_equal(Param_list a, Param_list b);
Param_list translate_param_dec(struct Node* root);
Param_list translate_var_list(struct Node* root);
void translate_fun_dec(struct Node *root, Type ret_type,bool is_declare);
Param_list create_param_list(struct Node* root);
Type translate_exp(struct Node* root,struct Operand_* op);
int get_memory_size(struct Type_ *var);
int cal_field_offset(struct Type_ * struct_type, char* name);
struct Operand_ *new_temp_var();
struct Operand_ *new_variable();
struct Operand_ *new_label();
struct Operand_ *new_constant();
struct Operand_ *new_function(char* func_name);
struct InterCode* new_InterCode();
struct Type_* translate_cond(struct  Node* root,struct Operand_ *label_true, struct Operand_ *label_false);
struct Type_ *translate_exp_to_addr(struct Node *root, struct Operand_ *op);
void insert_ARGS(Param_list paramList);
#endif //LAB_SDT_H
