//
// Created by plutoer on 22-11-3.
//

#ifndef LAB_IR_H
#define LAB_IR_H
#include <stdio.h>
#include <stdbool.h>
#define nullptr ((void *)0)
extern FILE* out_file;
enum Ir_Operand_kind{
    VARIABLE,
    CONSTANT,
    TEMP_VAR,
    OP_LABEL,
    OP_FUNCTION
};
struct Operand_{
    enum Ir_Operand_kind kind;
    union{
        int int_val;//val
        int no;
        char* name;
    };
    enum Reference_{
        Reference,
        Deference,
        BaseCond,
    }Ref_Def;
    bool is_addr;
};

struct InterCode {
    enum{
        LABEL_,
        FUNCTION_,
        ASSIGN_,
        ADD_,
        SUB_,
        MUL_,
        DIV_,
        GOTO_,
        IF_GOTO_,
        RETURN_,
        DEC_,
        ARG_,
        CALL_,
        PARAM_,
        READ_,
        WRITE_,
    }kind;
    union {
        struct {
            struct Operand_ *res;
            struct Operand_ *right,*left;
        }binary_op;
        struct {
            struct Operand_ *left;
            struct Operand_ *right;
        }unary_op;//maybe a :+= CALL FUNCTION a&&FUNCTION
    };
    enum Relop_type{
        RELOP_EQ,
        RELOP_NEQ,
        RELOP_G,
        RELOP_L,
        RELOP_GE,
        RELOP_LE,
    }Relop;
    int dec_size;
    struct InterCode *prev, *next;
};
void insert_IR(struct InterCode *Ir);
struct InterCode* Optimize_Ir(struct InterCode *code);
void write_Ir_to_file(struct InterCode* InterCode_list);
void print_IR(struct InterCode* Ir);
const char* translate_Operand(struct Operand_* operand);
#endif //LAB_IR_H
