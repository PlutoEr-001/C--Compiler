//
// Created by plutoer on 22-11-29.
//

#include "Mips.h"
#include <stdlib.h>
#include <string.h>
extern FILE *Mips_out_file;
struct Stack* StAck;
char* header=".data\n \
_prompt: .asciiz \"Enter an integer:\"\n\
_ret: .asciiz \"\\n\"\n\
.globl main\n\
.text\n\
read:\n\
li $v0, 4\n\
la $a0, _prompt\n\
syscall\n\
li $v0, 5\n\
syscall\n\
jr $ra\n\
\n\
write:\n\
li $v0, 1\n\
syscall\n\
li $v0, 4\n\
la $a0, _ret\n\
syscall\n\
move $v0, $0\n\
jr $ra\n\
\n";
extern struct Mips *mips_list;
extern struct Mips *mips_tail;
extern struct InterCode *InterCode_list;
extern struct InterCode *list_tail;
void write_Mips_to_file(struct Mips *mips_list){
    while(mips_list){
        print_Mips(mips_list);
        mips_list=mips_list->next;
    }
}
char* regs_name[32]={
        "$zero",
        "$at",
        "$v0", "$v1",
        "$a0", "$a1", "$a2", "$a3",
        "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
        "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
        "$t8", "$t9",
        "$k0", "$k1",
        "$gp",
        "$sp",
        "$fp",
        "$ra"
};
char* translate_Mips_Operand(struct Mips_operand* operand){
    char res[64];
    switch (operand->operand_Kind) {
        case Mips_Op_Imm:
            sprintf(res,"%d",operand->immediate);
            break;
        case Mips_Op_Addr:
            sprintf(res,"%d(%s)",operand->addr.offset,regs_name[operand->addr.reg]);
            break;
        case Mips_Op_Label:
            sprintf(res,"%s",operand->label_name);
            break;
        case Mips_Op_Reg:
            sprintf(res,"%s",regs_name[operand->reg]);
            break;
    }
    return strdup(res);
}
void Insert_Mips(struct Mips* mips){
    if (mips_tail==nullptr){
        mips_list=mips;
        mips_tail=mips;
        mips->prev=mips->next=nullptr;
    } else {
        mips->prev=mips_tail;
        mips_tail->next =mips;
        mips_tail=mips;
    }
}
void print_Mips(struct Mips* mips){
    switch(mips->mips_Kind){
        case MIPS_LABEL:
            fprintf(Mips_out_file,"%s:", mips->label_name);
            break;
        case MIPS_ADD:
            fprintf(Mips_out_file,"add %s %s %s", translate_Mips_Operand(mips->res), translate_Mips_Operand(mips->left),
                    translate_Mips_Operand(mips->right));
            break;
        case LI:
            fprintf(Mips_out_file,"li %s %s",translate_Mips_Operand(mips->left), translate_Mips_Operand(mips->right));
            break;
        case LA:
            fprintf(Mips_out_file,"la %s %s", translate_Mips_Operand(mips->left), translate_Mips_Operand(mips->right));
            break;
        case ADDI:
            fprintf(Mips_out_file, "addi %s %s %s", translate_Mips_Operand(mips->res), translate_Mips_Operand(mips->left), translate_Mips_Operand(mips->right));
            break;
        case MIPS_SUB:
            fprintf(Mips_out_file, "sub %s %s %s", translate_Mips_Operand(mips->res), translate_Mips_Operand(mips->left), translate_Mips_Operand(mips->right));
            break;
        case MIPS_MUL:
            fprintf(Mips_out_file, "mul %s %s %s", translate_Mips_Operand(mips->res), translate_Mips_Operand(mips->left), translate_Mips_Operand(mips->right));
            break;
        case MIPS_DIV:
            fprintf(Mips_out_file, "div %s %s %s", translate_Mips_Operand(mips->res), translate_Mips_Operand(mips->left), translate_Mips_Operand(mips->right));
            break;
        case MOVE:
            fprintf(Mips_out_file, "move %s %s", translate_Mips_Operand(mips->left), translate_Mips_Operand(mips->right));
            break;
        case LW:
            fprintf(Mips_out_file, "lw %s %s", translate_Mips_Operand(mips->left),translate_Mips_Operand(mips->right));
            break;
        case SW:
            fprintf(Mips_out_file, "sw %s %s", translate_Mips_Operand(mips->left),translate_Mips_Operand(mips->right));
            break;
        case J:
            fprintf(Mips_out_file, "j %s", translate_Mips_Operand(mips->left));
            break;
        case JAL:
            fprintf(Mips_out_file, "jal %s", translate_Mips_Operand(mips->left));
            break;
        case JR:
            fprintf(Mips_out_file, "jr %s", translate_Mips_Operand(mips->left));
            break;
        case BGE:
            fprintf(Mips_out_file, "bge %s %s %s", translate_Mips_Operand(mips->res),translate_Mips_Operand(mips->left),
                    translate_Mips_Operand(mips->right));
            break;
        case BGT:
            fprintf(Mips_out_file, "bgt %s %s %s", translate_Mips_Operand(mips->res),translate_Mips_Operand(mips->left),
                    translate_Mips_Operand(mips->right));
            break;
        case BLE:
            fprintf(Mips_out_file, "ble %s %s %s", translate_Mips_Operand(mips->res),translate_Mips_Operand(mips->left),
                    translate_Mips_Operand(mips->right));
            break;
        case BLT:
            fprintf(Mips_out_file, "blt %s %s %s", translate_Mips_Operand(mips->res),translate_Mips_Operand(mips->left),
                    translate_Mips_Operand(mips->right));
            break;
        case BEQ:
            fprintf(Mips_out_file, "beq %s %s %s", translate_Mips_Operand(mips->res),translate_Mips_Operand(mips->left),
                    translate_Mips_Operand(mips->right));
            break;
        case BNE:
            fprintf(Mips_out_file, "bne %s %s %s", translate_Mips_Operand(mips->res),translate_Mips_Operand(mips->left),
                    translate_Mips_Operand(mips->right));
            break;
        default:
            break;
    }
    fprintf(Mips_out_file,"\n");
}
struct Mips* new_Mips(enum Mips_kind kin,struct Mips_operand* re,struct Mips_operand* lef,struct Mips_operand* rig){
    struct Mips* temp=(struct Mips*)malloc(sizeof(struct Mips));
    temp->mips_Kind=kin;
    temp->left=lef;
    temp->right=rig;
    temp->res=re;
    return temp;
}
struct Mips_operand* new_Mips_reg(int i){
    struct Mips_operand* temp=(struct Mips_operand*)malloc(sizeof(struct Mips_operand));
    temp->operand_Kind=Mips_Op_Reg;
    temp->reg=i;
    return temp;
}
struct Mips_operand* new_Mips_label(char* label_name){
    struct Mips_operand* temp=(struct Mips_operand*)malloc(sizeof(struct Mips_operand));
    temp->operand_Kind=Mips_Op_Label;
    temp->label_name= strdup(label_name);
    return temp;
}
struct Mips_operand* new_Mips_addr(enum Mips_regs regi,int offset){
    struct Mips_operand* temp=(struct Mips_operand*)malloc(sizeof(struct Mips_operand));
    temp->operand_Kind=Mips_Op_Addr;
    temp->addr.reg=regi;
    temp->addr.offset=offset;
    return temp;
}
struct Mips_operand* new_Mips_Imm(int val){
    struct Mips_operand* temp=(struct Mips_operand*)malloc(sizeof(struct Mips_operand));
    temp->operand_Kind=Mips_Op_Imm;
    temp->immediate=val;
    return temp;
}
void translate_Ir2Mips(){
    fputs(header, Mips_out_file);
    for (int i = 0; i < 32; i++) {
        regs[i] = new_Mips_reg(i);
    }
    gen_function_block(InterCode_list, list_tail);
    write_Mips_to_file(mips_list);
}
void push_var(enum Ir_Operand_kind kind, int var_no){
    struct stack_varible* temp=(struct stack_varible*) malloc(sizeof(struct stack_varible));
    temp->offset_2_fp=StAck->varible_offset;
    StAck->varible_offset-=4;
    temp->varible_kind=kind;
    temp->var_no=var_no;
    temp->next=StAck->stack_varible_list;
    StAck->stack_varible_list=temp;
}
void handle_function_block(struct InterCode* block_start,struct InterCode* block_end){
    struct Mips* m0= new_Mips(MIPS_LABEL,nullptr,nullptr,nullptr);
    m0->label_name=strdup(block_start->unary_op.left->name);
    Insert_Mips(m0);
    StAck=(struct Stack*) malloc(sizeof(struct Stack));
    StAck->varible_offset=-4;
    StAck->arg_offset=8;
    StAck->arg_num=0;
    StAck->stack_varible_list=nullptr;
    // Generate prologue:
    //push ebp
    // move ebp esp
    // addi $sp, $sp, -frame_size
    struct Mips *m1;
    struct Mips_operand* op;
    op= new_Mips_Imm(-4);
    m1= new_Mips(ADDI,regs[$sp],regs[$sp],op);
    Insert_Mips(m1);
    op= new_Mips_addr($sp,0);
    m1= new_Mips(SW,nullptr,regs[$fp],op);
    Insert_Mips(m1);
    m1= new_Mips(MOVE,nullptr,regs[$fp],regs[$sp]);
    Insert_Mips(m1);
    /*op= new_Mips_Imm(-4);
    m1= new_Mips(ADDI,regs[$sp],regs[$sp],op);
    Insert_Mips(m1);
    op= new_Mips_addr($sp,0);
    m1= new_Mips(SW,nullptr,regs[$ra],op);
    Insert_Mips(m1);
    m1= new_Mips(MOVE,nullptr,regs[$fp],regs[$sp]);
    Insert_Mips(m1);*/
    //to be determined
    op= new_Mips_Imm(-1);
    m1= new_Mips(ADDI,regs[$sp],regs[$sp],op);
    Insert_Mips(m1);
    int *frame_size = &(op->immediate); // wait to be filled
    // put arguments in the frame variables table
    struct InterCode *temp=block_start->next;
    while (temp->kind==PARAM_) {
        insert_args_2_stack(temp->unary_op.left->kind,temp->unary_op.left->no);
        temp=temp->next;
    }
    //StAck->arg_offset=8;//for di gui call
    while (temp!= block_end->next) {
        translate_Ir(temp);
        temp=temp->next;
    }
    *frame_size =StAck->varible_offset;
    StAck=nullptr;
}
void load_varible_2_Reg(struct Operand_* ir_operand,enum Mips_regs reg_no){
    /*     VARIABLE,
            CONSTANT,
            TEMP_VAR,
            OP_LABEL,
            OP_FUNCTION
    */
    struct Mips* m0;
    struct Mips_operand* op;
    switch(ir_operand->kind){
        case VARIABLE:
        case TEMP_VAR:
            if(find_varible_in_stack(ir_operand->kind,ir_operand->no)==0){
                push_var(ir_operand->kind,ir_operand->no);
            }
            int offset= get_varible_offset_to_fp(ir_operand->kind,ir_operand->no);
            //&a
            if(ir_operand->Ref_Def==Reference){
                op= new_Mips_addr($fp,offset);
                m0= new_Mips(LA,nullptr,regs[reg_no],op);
                Insert_Mips(m0);
                break;
            }
            //*a
            else if(ir_operand->Ref_Def==Deference){
                op= new_Mips_addr($fp,offset);
                m0= new_Mips(LW,nullptr,regs[reg_no],op);
                Insert_Mips(m0);
                op= new_Mips_addr(reg_no,0);
                m0= new_Mips(LW,nullptr,regs[reg_no],op);
                Insert_Mips(m0);
                break;
            }
            else{
                op= new_Mips_addr($fp,offset);
                m0= new_Mips(LW,nullptr,regs[reg_no],op);
                Insert_Mips(m0);
                break;
            }
        case CONSTANT:
            op= new_Mips_Imm(ir_operand->int_val);
            m0= new_Mips(LI,nullptr,regs[reg_no],op);
            Insert_Mips(m0);
            break;
        default:
            break;
    }
}
int find_varible_in_stack(enum Ir_Operand_kind var_kind,int var_no){
    struct stack_varible* temp=StAck->stack_varible_list;
    while(temp){
        if(temp->varible_kind==var_kind&&temp->var_no==var_no){
            return 1;
        }
        temp=temp->next;
    }
    return 0;
}
void translate_Ir(struct InterCode *ir){
    struct Mips* m0;
    struct Mips_operand* op;
    char buf[64];
    enum Mips_kind relop;
    enum Mips_kind cal;
    struct stack_varible* temp;
    int arg_num_temp;
    switch (ir->kind) {
        case DEC_:
            temp=(struct stack_varible*) malloc(sizeof(struct stack_varible));
            StAck->varible_offset-=ir->dec_size;
            temp->offset_2_fp=StAck->varible_offset+4;
            temp->varible_kind=ir->unary_op.left->kind;
            temp->var_no=ir->unary_op.left->no;
            temp->next=StAck->stack_varible_list;
            StAck->stack_varible_list=temp;
            break;
        case LABEL_:
            sprintf(buf, "label%d", ir->unary_op.left->no);
            m0= new_Mips(MIPS_LABEL,nullptr,nullptr,nullptr);
            m0->label_name= strdup(buf);
            Insert_Mips(m0);
            break;
        case CALL_:
            op= new_Mips_Imm(-4);
            m0= new_Mips(ADDI,regs[$sp],regs[$sp],op);
            Insert_Mips(m0);
            op= new_Mips_addr($sp,0);
            m0= new_Mips(SW,nullptr,regs[$ra],op);
            Insert_Mips(m0);
            op= new_Mips_label(ir->unary_op.right->name);
            m0= new_Mips(JAL,nullptr,op,nullptr);
            Insert_Mips(m0);
            //pop ra
            op= new_Mips_addr($sp,0);
            m0= new_Mips(LW,nullptr,regs[$ra],op);
            Insert_Mips(m0);
            op= new_Mips_Imm(4);
            m0= new_Mips(ADDI,regs[$sp],regs[$sp],op);
            Insert_Mips(m0);
            m0= new_Mips(MOVE,nullptr,regs[$t1],regs[$v0]);//need to store in memory
            Insert_Mips(m0);
            op= new_Mips_addr($fp, get_varible_offset_to_fp(ir->unary_op.left->kind,ir->unary_op.left->no));
            m0= new_Mips(SW,nullptr,regs[$t1],op);
            Insert_Mips(m0);
            break;
        case ASSIGN_:
            if (ir->unary_op.left->Ref_Def==Deference){
                int offset = get_varible_offset_to_fp(ir->unary_op.left->kind, ir->unary_op.left->no);
                op= new_Mips_addr($fp,offset);
                m0= new_Mips(LW,nullptr,regs[$t1],op);
                Insert_Mips(m0);
                load_varible_2_Reg(ir->unary_op.right,$t2);
                op=new_Mips_addr($t1,0);
                m0= new_Mips(SW,nullptr,regs[$t2],op);
                Insert_Mips(m0);
                return;
            }else{
                load_varible_2_Reg(ir->unary_op.right,$t2);
                m0= new_Mips(MOVE,nullptr,regs[$t1],regs[$t2]);
                Insert_Mips(m0);
                op= new_Mips_addr($fp, get_varible_offset_to_fp(ir->unary_op.left->kind,ir->unary_op.left->no));
                m0= new_Mips(SW,nullptr,regs[$t1],op);
                Insert_Mips(m0);
                break;
            }
        case ADD_:
        case SUB_:
        case MUL_:
        case DIV_:
            switch(ir->kind){
                case ADD_: cal=MIPS_ADD; break;
                case SUB_: cal=MIPS_SUB; break;
                case MUL_: cal=MIPS_MUL; break;
                case DIV_: cal=MIPS_DIV; break;
                default: break;
            }
            load_varible_2_Reg(ir->binary_op.left,$t2);
            load_varible_2_Reg(ir->binary_op.right,$t3);
            m0= new_Mips(cal,regs[$t1],regs[$t2],regs[$t3]);
            Insert_Mips(m0);
            op= new_Mips_addr($fp, get_varible_offset_to_fp(ir->binary_op.res->kind,ir->binary_op.res->no));
            m0= new_Mips(SW,nullptr,regs[$t1],op);
            Insert_Mips(m0);
            break;
        case GOTO_:
            sprintf(buf, "label%d", ir->unary_op.left->no);
            op= new_Mips_label(strdup(buf));
            m0= new_Mips(J,nullptr,op,nullptr);
            Insert_Mips(m0);
            break;
        case IF_GOTO_:
            load_varible_2_Reg(ir->binary_op.left,$t2);
            load_varible_2_Reg(ir->binary_op.right,$t3);
            switch (ir->Relop) {
                case RELOP_EQ:  relop=BEQ; break;
                case RELOP_NEQ: relop=BNE; break;
                case RELOP_G:   relop = BGT; break;
                case RELOP_L:   relop = BLT; break;
                case RELOP_GE:  relop = BGE; break;
                case RELOP_LE:  relop = BLE; break;
            }
            sprintf(buf, "label%d", ir->binary_op.res->no);
            op= new_Mips_label(strdup(buf));
            m0= new_Mips(relop,regs[$t2],regs[$t3],op);
            Insert_Mips(m0);
            break;
        case RETURN_:
            load_varible_2_Reg(ir->unary_op.left,$t1);
            m0= new_Mips(MOVE,nullptr,regs[$v0],regs[$t1]);
            Insert_Mips(m0);
            // Generate epilogue:
            // move $sp, $fp
            // lw $fp, 0($sp)
            // jr $ra
            m0= new_Mips(MOVE,nullptr,regs[$sp],regs[$fp]);
            Insert_Mips(m0);
            op= new_Mips_addr($sp,0);
            m0= new_Mips(LW,nullptr,regs[$fp],op);
            Insert_Mips(m0);
            op= new_Mips_Imm(4);
            m0= new_Mips(ADDI,regs[$sp],regs[$sp],op);
            Insert_Mips(m0);
            op= new_Mips_Imm(-4);
            m0= new_Mips(JR,nullptr,regs[$ra],nullptr);
            Insert_Mips(m0);
            break;
        case ARG_:
            load_varible_2_Reg(ir->unary_op.left,$t1);
            op= new_Mips_Imm(-4);
            m0=new_Mips(ADDI,regs[$sp],regs[$sp],op);
            Insert_Mips(m0);
            op= new_Mips_addr($sp,0);
            m0= new_Mips(SW,nullptr,regs[$t1],op);
            Insert_Mips(m0);
            StAck->arg_num++;
            break;
            //read t2
        case READ_:
            op= new_Mips_addr($sp,-4);
            m0= new_Mips(SW,nullptr,regs[$ra],op);
            Insert_Mips(m0);
            op= new_Mips_label("read");
            m0= new_Mips(JAL,nullptr,op,nullptr);
            Insert_Mips(m0);
            op= new_Mips_addr($sp,-4);
            m0= new_Mips(LW,nullptr,regs[$ra],op);
            Insert_Mips(m0);
            //store res
            m0= new_Mips(MOVE,nullptr,regs[$t1],regs[$v0]);
            Insert_Mips(m0);
            //printf("%d %d\n",ir->unary_op.left->kind,ir->unary_op.left->no);
            op= new_Mips_addr($fp,get_varible_offset_to_fp(ir->unary_op.left->kind,ir->unary_op.left->no));
            m0= new_Mips(SW,nullptr,regs[$t1],op);
            Insert_Mips(m0);
            break;
        case WRITE_:
            load_varible_2_Reg(ir->unary_op.left,$t1);
            op= new_Mips_addr($sp,-4);
            m0= new_Mips(SW,nullptr,regs[$ra],op);
            Insert_Mips(m0);
            m0= new_Mips(MOVE,nullptr,regs[$a0],regs[$t1]);
            Insert_Mips(m0);
            op= new_Mips_label("write");
            m0= new_Mips(JAL,nullptr,op,nullptr);
            Insert_Mips(m0);
            op= new_Mips_addr($sp,-4);
            m0= new_Mips(LW,nullptr,regs[$ra],op);
            Insert_Mips(m0);
            break;
        default:
            break;
    }
}
void gen_function_block(struct InterCode *head, struct InterCode *tail) {
    struct InterCode *block_start=nullptr,*block_end=nullptr;
    bool head_founded=false;
    while (head != tail) {
        if(head->kind==FUNCTION_){
            if(!head_founded){
                block_start=head;
                head_founded=true;
            } else{
                head=head->prev;
                block_end=head;
                head_founded=false;
                handle_function_block(block_start,block_end);
            }
        }
        head=head->next;
    }
    block_end=tail;
    handle_function_block(block_start,block_end);
}
int get_varible_offset_to_fp(enum Ir_Operand_kind varible_kind,int varible_no){
    struct stack_varible* temp=StAck->stack_varible_list;
    while(temp){
        if(temp->varible_kind==varible_kind&&temp->var_no==varible_no){
            return temp->offset_2_fp;
        }
        temp=temp->next;
    }
    push_var(varible_kind,varible_no);
    return get_varible_offset_to_fp(varible_kind,varible_no);
}
void insert_args_2_stack(enum Ir_Operand_kind kind, int var_no){
    struct stack_varible* temp=(struct stack_varible*) malloc(sizeof(struct stack_varible));
    temp->varible_kind=kind;
    temp->var_no=var_no;
    temp->offset_2_fp=StAck->arg_offset;
    temp->next=StAck->stack_varible_list;
    StAck->stack_varible_list=temp;
    StAck->arg_offset+=4;
}

