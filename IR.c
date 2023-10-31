//
// Created by plutoer on 22-11-3.
//
#include "IR.h"
#include <string.h>
extern struct InterCode *InterCode_list;
extern struct InterCode *list_tail;
const char* translate_Operand(struct Operand_* operand){
    char res[64];
    char *ref_def;
    switch (operand->Ref_Def) {
        case Reference:
            ref_def="&";
            break;
        case Deference:
            ref_def="*";
            break;
        case BaseCond:
            ref_def="";
            break;
        default:
            break;
    }
    switch(operand->kind){
        case VARIABLE:
            //a varible named T_1 may can not be told from temp_var
            sprintf(res,"%sV_%d",ref_def,operand->no);
            break;
        case OP_FUNCTION:
            sprintf(res,"%s",operand->name);
            return strdup(operand->name);
            break;
        case OP_LABEL:
            sprintf(res,"Label_%d",operand->no);
            break;
        case TEMP_VAR:
            sprintf(res,"%sT_%d",ref_def,operand->no);
            break;
        case CONSTANT:
            sprintf(res,"#%d",operand->int_val);
            break;
        default:
            break;
    }
    return strdup(res);
}
const char* relop_symbol[] = {"==", "!=", ">", "<", ">=", "<="};
void print_IR(struct InterCode* Ir){
    switch(Ir->kind){
        case LABEL_:
            fprintf(out_file,"LABEL %s :", translate_Operand(Ir->unary_op.left));
            break;
        case FUNCTION_:
            fprintf(out_file,"FUNCTION %s :", translate_Operand(Ir->unary_op.left));
            break;
        case ASSIGN_:
            fprintf(out_file,"%s := %s",translate_Operand(Ir->unary_op.left), translate_Operand(Ir->unary_op.right));
            break;
        case ADD_:
            fprintf(out_file, "%s := %s + %s", translate_Operand(Ir->binary_op.res), translate_Operand(Ir->binary_op.left), translate_Operand(Ir->binary_op.right));
            break;
        case SUB_:
            fprintf(out_file, "%s := %s - %s", translate_Operand(Ir->binary_op.res), translate_Operand(Ir->binary_op.left), translate_Operand(Ir->binary_op.right));
            break;
        case MUL_:
            fprintf(out_file, "%s := %s * %s", translate_Operand(Ir->binary_op.res), translate_Operand(Ir->binary_op.left), translate_Operand(Ir->binary_op.right));
            break;
        case DIV_:
            fprintf(out_file, "%s := %s / %s", translate_Operand(Ir->binary_op.res), translate_Operand(Ir->binary_op.left), translate_Operand(Ir->binary_op.right));
            break;
        case GOTO_:
            fprintf(out_file, "GOTO %s", translate_Operand(Ir->unary_op.left));
            break;
        case IF_GOTO_:
            fprintf(out_file, "IF %s %s %s GOTO %s", translate_Operand(Ir->binary_op.left),relop_symbol[Ir->Relop],translate_Operand(Ir->binary_op.right), translate_Operand(Ir->binary_op.res));
            break;
        case RETURN_:
            fprintf(out_file, "RETURN %s", translate_Operand(Ir->unary_op.left));
            break;
        case DEC_:
            fprintf(out_file, "DEC %s %d", translate_Operand(Ir->unary_op.left), Ir->dec_size);
            break;
        case ARG_:
            fprintf(out_file, "ARG %s", translate_Operand(Ir->unary_op.left));
            break;
        case CALL_:
            fprintf(out_file, "%s := CALL %s", translate_Operand(Ir->unary_op.left), translate_Operand(Ir->unary_op.right));
            break;
        case PARAM_:
            fprintf(out_file, "PARAM %s", translate_Operand(Ir->unary_op.left));
            break;
        case READ_:
            fprintf(out_file, "READ %s", translate_Operand(Ir->unary_op.left));
            break;
        case WRITE_:
            fprintf(out_file, "WRITE %s", translate_Operand(Ir->unary_op.left));
            break;
        default:
            break;
    }
    fprintf(out_file,"\n");
}
void write_Ir_to_file(struct InterCode* InterCode_list){
    struct InterCode*temp=InterCode_list;
    while(temp){
        print_IR(temp);
        temp=temp->next;
    }
}
void insert_IR(struct InterCode *temp){
    if (list_tail==nullptr){
        InterCode_list=temp;
        list_tail=temp;
        temp->prev=temp->next=nullptr;
    } else {
        temp->prev=list_tail;
        list_tail->next =temp;
        list_tail=temp;
    }
}
