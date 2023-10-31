//
// Created by pluto on 22-9-24.
//
#include "ast.h"
extern int yylineno;
extern int yyleng;
int get_child_num(struct Node *root){
    int num = 0;
    struct Node* p=root->child;
    while(p){
        num++;
        p=p->peer;
    }
    return num;
}
struct Node *get_nth_child(struct Node *root, int n){
    struct Node* p=root->child;
    if(p==nullptr) return nullptr;
    while(n){
        p=p->peer;
        if(p==nullptr) return nullptr;
        n--;
    }
    return p;
}
void add_children(Node *parent, int num, ...) {
    va_list ap;
    va_start(ap,num);
    struct Node *prev;
    for(int i=0;i<num;i++){
        struct Node *child = va_arg(ap, struct Node*);
        if(child!=NULL){
            if (i==0) {
                parent->child = child;
                parent->lineno = child->lineno;
                parent->column = child->column;
                parent->length = child->length;
            } else {
                parent->length += child->length;
                prev->peer = child;
            }
            child->parent = parent;
            prev = child;
        }
    }
}
Node* make_ast_node(enum LEX_TOKEN token_, union ast_value value){
    Node* node = (Node*) malloc(sizeof(struct Node));
    node->token_ = token_;
    node->value = value;
    node->lineno = yylineno;
    node->column = yylloc.first_column;
    node->length = yyleng;
    /*if(node->token_==TYPE_){
        printf("%s",value.str_value);
        printf("ID  IS  %s\n",node->value.str_value);
        printf("**********\n");
    }*/
    return node;
}
void print_ast(struct Node *root, int space) {
    if (root == NULL) return;
    int n;
    for (n=0; n<space; n++) {
        printf("  ");
    }
    switch (root->token_) {
        case INT_:
            printf("INT");
            printf(": %d", root->value.int_value);
            break;
        case FLOAT_:
            printf("FLOAT");
            printf(": %f", root->value.float_value);
            break;
        case ID_:
            printf("ID");
            printf(": %s", root->value.str_value);
            break;
        case TYPE_:
            printf("TYPE");
            printf(": %s", root->value.str_value);
            break;
        case RELOP_:
            printf("RELOP");
            break;
        default:
            printf("%s", root->value.str_value);
            break;
    }
    if (root->child) {
        printf(" (%d)\n", root->lineno);
        struct Node *p = root->child;
        while (p != NULL) {
            print_ast(p, space+1);
            p = p->peer;
        }
    } else {
        printf("\n");
    }
}