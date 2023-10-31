//
// Created by pluto on 22-9-24.
//

#ifndef LAB_1_AST_H
#define LAB_1_AST_H
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include "sytanx.tab.h"
#define nullptr ((void *)0)
enum LEX_TOKEN {
    INT_,FLOAT_,ID_,TYPE_,RELOP_,OTHER_,
};
union ast_value {
    int int_value;
    float float_value;
    char* str_value;
};
typedef struct Node {
    enum LEX_TOKEN token_;
    union ast_value value;
    int lineno;
    int column;
    int length;
    struct Node *child;
    struct Node *peer;
    struct Node *parent;
}Node;
Node* ast_root;
void add_children(struct Node *parent, int num, ...);
Node* make_ast_node(enum LEX_TOKEN token_, union ast_value value);
void print_ast(struct Node *root, int space);
int get_child_num(struct Node *root);
struct Node* get_nth_child(struct Node *root, int n);
#endif //LAB_1_AST_H
