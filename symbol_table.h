//
// Created by plutoer on 22-10-19.
//
#ifndef LAB_SYMBOL_TABLE_H
#define LAB_SYMBOL_TABLE_H
#include "error.h"
#include "IR.h"
#include <string.h>
#define Table_size 0x3fff
#define MAX_NEST_NUM 100
struct Type_;
struct Param;
struct FieldList_;
struct table_item;
typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct Param* Param_list;
struct Param{
    Type param_type;
    char* name;
    struct Param* next;
    struct Operand_* op;
};
struct Type_{
    enum { BASIC, ARRAY, STRUCTURE, FUNCTION } Kind;
    union{
         // 基本类型
         int basic;//0:int 1 float
         //数组类型信息包括元素类型与数组大小构成
         struct{Type elem;int size;} array;
         //结构体类型信息是一个链表
         FieldList structure;
         //function
         struct{
             enum {DECLARED,DEFINED} State;
             struct Node* ast_node;
             Type return_type;
             Param_list param_list;
         }func;
    }content;
};
struct FieldList_{
 char* name; // 域的名字
 Type type;// 域的类型
 FieldList tail; // 下一个域
};
struct table_item{
    char* name;//name of varible
    Type type;
    int scope;
    struct table_item *hash_next,*hash_prev;
    struct table_item *stack_next,*stack_prev;
    struct Operand_ *op;
};
unsigned int hash_pjw(char* name); //map to hash_val
void Initialize();//Initialize the elements of hashtable and stack as nullptr
struct table_item* find_in_scope(char* name,int scope);//find the same name as "name" with the scope_level :scope
bool add_symbol(struct table_item* item);//add the symbol to hashtable and stack
struct table_item* find_symbol(char* name);//serach from head (zui nei ceng de scope) to find a varible called name
void symbol_stack_pop(int scope_level);//pop symbols in a compst when finished
void head_insert_HashTable(int hash_val,struct table_item* item);//head insert to hashtable
void add_to_stack(struct table_item *item);//head insert to stack
void delete_symbol(char* name);
#endif //LAB_SYMBOL_TABLE_H
