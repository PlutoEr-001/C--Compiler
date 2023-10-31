//
// Created by plutoer on 22-11-29.
//

#ifndef LAB_MIPS_H
#define LAB_MIPS_H
#include "IR.h"
enum Mips_regs{
    $zero,//常数0
    $at,// (Assembler Temporary) 汇编器保留
    $v0,
    $v1,// (Values) 表达式求值或函数结果
    $a0,
    $a1,
    $a2,
    $a3,// (Arguments) 函数的首四个参数（跨函数不保留） yi chu dao stack
    $t0,
    $t1,
    $t2,
    $t3,
    $t4,
    $t5,
    $t6,
    $t7,// (Temporaries) 函数调用者负责保存（跨函数不保留）
    $s0,
    $s1,
    $s2,
    $s3,
    $s4,
    $s5,
    $s6,
    $s7,// (Saved Values) 函数负责保存和恢复（跨函数不保留)
    $t8,
    $t9,//(Temporaries) 函数调用者负责保存（跨函数不保留)
    $k0,
    $k1,//中断处理保留
    $gp,// $gp (Global Pointer) 指向静态数据段64K内存空间的中部。
    $sp,// $sp (Stack Pointer) 栈顶指针
    $fp,// $fp
    $ra,//$ra return address
};
//32 regs


struct Mips_operand{
    enum Operand_kind{
       Mips_Op_Label,
       Mips_Op_Imm,
       Mips_Op_Addr,//only in load or store  load dst src ; st addr reg
       Mips_Op_Reg,
    }operand_Kind;
    union{
        int immediate;
        char* label_name;
        //addr
        struct{
            enum Mips_regs reg;
            int offset;//  4($a1)
        }addr;
        enum Mips_regs reg;
    };
};

struct Mips_operand* regs[32];
struct Mips{
    enum Mips_kind{
        MIPS_LABEL,
        LI,
        LA,
        MOVE,
        BEQ,
        BNE,
        BGT,
        BGE,
        BLT,
        BLE,
        MIPS_ADD,
        ADDI,
        MIPS_SUB,
        MIPS_MUL,
        MIPS_DIV,
        JAL,//bao cun fan hui di zhi bing tiao zhuan
        J,// label or address
        JR,// register  jr rs pc<-rs
        SW,//sw rs rt:memory[rs]=rt;
        LW,
    }mips_Kind;
    struct {
        struct Mips_operand *res,*left,*right;
    };
    char* label_name;
    struct Mips *prev,*next;
};
struct stack_varible{
    enum Ir_Operand_kind varible_kind;
    int var_no;
    int offset_2_fp;
    struct stack_varible *next;
};
struct Stack{
    struct stack_varible* stack_varible_list;
    int varible_offset;//initialed be -8
    int arg_offset;//needed to visit arg
    int arg_num;
};
void Insert_Mips(struct Mips* mips);
struct Mips_operand* new_Mips_reg(int i);
struct Mips_operand* new_Mips_label(char* label_name);
struct Mips_operand* new_Mips_addr(enum Mips_regs regi,int offset);
struct Mips_operand* new_Mips_Imm(int val);
void print_Mips(struct Mips* mips);
void write_Mips_to_file(struct Mips *mips_list);
char* translate_Mips_Operand(struct Mips_operand* operand);
void gen_function_block(struct InterCode *head, struct InterCode *tail);
void handle_function_block(struct InterCode* block_start,struct InterCode* block_end);
int get_varible_offset_to_fp(enum Ir_Operand_kind varible_kind,int varible_no);
void translate_Ir(struct InterCode *ir);
void push_var(enum Ir_Operand_kind kind, int var_no);
void load_varible_2_Reg(struct Operand_* ir_operand,enum Mips_regs reg_no);
void insert_args_2_stack(enum Ir_Operand_kind kind, int var_no);
void translate_Ir2Mips();
int find_varible_in_stack(enum Ir_Operand_kind var_kind,int var_no);
#endif //LAB_MIPS_H
