//
// Created by plutoer on 22-10-21.
//
#include "sdt.h"
int temp_var_no = 1;
int variable_no = 1;
int label_no    = 1;
int scope_level=0;//scope_now
Type ret_type;//function cna not involve function
extern struct table_item* Symbol_Hashtable[Table_size];
extern struct table_item* SymbolTable_stack[MAX_NEST_NUM];
void translate(struct Node* root) {
    char* name=strdup(root->value.str_value);
    //printf("%s aaaaaaaaa\n",name);
    if(strcmp(name,"ExtDef")==0){
        //printf("in ext_def\n");
        translate_ext_def(root);
        //printf("return from ext_def\n");
        return;
    }
    else if(strcmp(name,"Exp")==0){
        //printf("in exp\n");
        translate_exp(root,nullptr);
        return;
    }
    else if(strcmp(name,"CompSt")==0){
        //printf("in compst\n");
        translate_compst(root);
        return;
    }
    else if(strcmp(name,"Stmt")==0){
        //printf("in stmt\n");
        translate_stmt(root);
        //printf("back!!!!\n");
        return;
    }
    else{
        struct Node* child=root->child;
        while(child!=nullptr){
            translate(child);
            //printf("%s has finished!!!\n",child->value.str_value);
            child=child->peer;
        }
        //printf("%s\n",root->value.str_value);
        return;
    }
}
/*!
 * StructSpecifier → STRUCT OptTag LC DefList RC
    | STRUCT Tag
    OptTag → ID
    |empty
    Tag → ID
 * @param root
 * @return
 */
FieldList translate_struct_specifier(struct Node* root){
    FieldList struct_field;
    struct Node* tag= get_nth_child(root,1);//LC or ID
    /*!
     * structSpecifier → STRUCT OptTag LC DefList RC
     */
    Type temp=ret_type;
    ret_type=nullptr;
    int child_num= get_child_num(root);
    if(child_num==5){
        char* struct_name= strdup(tag->child->value.str_value);
        //printf("in STRUCT:%s\n",struct_name);
        scope_level++;//can not be redefined in struct
        struct Node* def_list= get_nth_child(root,3);
        struct_field=translate_def_list(def_list);
        /*printf("%s Field:\n",struct_name);
        FieldList temp=struct_field;
        while(temp!=nullptr){
            printf("%s, %d\n",temp->name,temp->type->Kind);
            temp=temp->tail;
        }*/
        symbol_stack_pop(scope_level);
        scope_level--;
        struct table_item* new_struct= (struct table_item*)malloc(sizeof(struct table_item));
        Type structure=(struct Type_*)malloc(sizeof(struct Type_));
        structure->Kind=STRUCTURE;
        structure->content.structure=struct_field;
        new_struct->name=struct_name;
        new_struct->scope=scope_level;
        new_struct->type=structure;
        if (!add_symbol(new_struct)) {
            print_error(15, root->child, new_struct->name);
        }
        //printf("%s successfully added\n",new_struct->name);
    }
    else if(child_num==4){
        char* no_1_name= strdup(tag->value.str_value);
        if(strcmp(no_1_name,"LC")==0){
            //struct lc def_list rc
            scope_level++;//can not be redefined in struct
            struct Node* def_list= get_nth_child(root,2);
            struct_field=translate_def_list(def_list);
            symbol_stack_pop(scope_level);
            scope_level--;
        }
        else{
            //struct opt_tag lc rc
            scope_level++;//can not be redefined in struct
            struct Node* def_list= get_nth_child(root,3);
            struct_field=translate_def_list(def_list);
            symbol_stack_pop(scope_level);
            scope_level--;
            char* struct_name= strdup(tag->child->value.str_value);
            struct table_item* new_struct=(struct table_item*)malloc(sizeof(struct table_item));
            Type structure=(struct Type_*)malloc(sizeof(struct Type_));
            structure->Kind=STRUCTURE;
            structure->content.structure=struct_field;
            new_struct->name=struct_name;
            new_struct->scope=scope_level;
            new_struct->type=structure;
            if (!add_symbol(new_struct)) {
                print_error(15, root->child, new_struct->name);
            }
        }
    }
    else if(child_num==3){
        //struct lc rc
        scope_level++;//can not be redefined in struct
        struct Node* def_list= get_nth_child(root,3);
        struct_field=translate_def_list(def_list);
        symbol_stack_pop(scope_level);
        scope_level--;
    }
    else{
        //struct tag
        char* struct_name= strdup(tag->child->value.str_value);
        //printf("struct :%s is being found\n",struct_name);
        struct table_item* result= find_symbol(struct_name);
        if(result!=nullptr){
            struct_field=result->type->content.structure;
        }
        else{
            print_error(16,tag,struct_name);
            return nullptr;
        }
    }
    ret_type=temp;
    return struct_field;
}
/*!
 * Specifier → TYPE
    | StructSpecifier
 * @param root
 */
Type translate_specifier(struct Node* root){
    Type spec_type=(struct Type_*)malloc(sizeof(struct Type_));
    struct Node* child=root->child;
    if(child->token_==TYPE_){
        spec_type->Kind=BASIC;
        if(strcmp("int",child->value.str_value)==0){
            spec_type->content.basic=0;
        }
        else spec_type->content.basic=1;
    }
    else{
        spec_type->Kind=STRUCTURE;
        spec_type->content.structure=translate_struct_specifier(root->child);
        if (spec_type->content.structure==nullptr){
            return nullptr;
        }
    }
    return spec_type;
}
/*!
 * VarDec → ID
    | VarDec LB INT RB
    a   m[2][3]
 * @param root
 * @param name
 * @param vt
 * @return  var or array
 */
FieldList translate_var_dec(struct Node* root,Type var_type) {
    //ID
    FieldList var_dec=(struct FieldList_*)malloc(sizeof(struct FieldList_));
    if(get_child_num(root)==1){
        var_dec->name= strdup(root->child->value.str_value);
        var_dec->type=var_type;
        return var_dec;
    }
    //m [2][3]
    //insert_head 3->2
    Type array_head=(struct Type_*)malloc(sizeof(struct Type_));
    array_head->Kind=ARRAY;
    array_head->content.array.size= get_nth_child(root,2)->value.int_value;
    array_head->content.array.elem=var_type;
    root=root->child;
    while (get_child_num(root)!=1){
        Type array_type=(struct Type_*)malloc(sizeof(struct Type_));
        array_type->Kind=ARRAY;
        array_type->content.array.size= get_nth_child(root,2)->value.int_value;
        array_type->content.array.elem=array_head;
        array_head=array_type;
        root = root->child;
    }
    var_dec->name= strdup(root->child->value.str_value);
    var_dec->type=array_head;
    return var_dec;// tail is useless
}
/*!
 * Dec → VarDec
    | VarDec ASSIGNOP Exp
 */
FieldList translate_dec(struct Node* root, Type var_type){
    //only var_dec
    FieldList var_dec=translate_var_dec(root->child,var_type);
    ////printf("return from var_dec,%d\n",var_dec->type->content.basic);
    struct table_item* new_item=(struct table_item*) malloc(sizeof(struct table_item));
    new_item->name=var_dec->name;
    new_item->type=var_dec->type;
    new_item->scope=scope_level;
    new_item->op=new_variable();
    //printf("%s\n",new_item->name);
    if (!add_symbol(new_item)){//only same scope
        if (ret_type == nullptr) { // now in struct
            print_error(14, root->child, new_item->name);
        } else { // now in function
            print_error(2, root->child, new_item->name);
        }
        return nullptr;
    }
    ////printf("here\n");
    //func can not qian tao define
    if (ret_type == nullptr) { // Initialized in struct
        if (get_child_num(root)!=1) {
            print_error(14, root);
        }
    }
    else {
        if(get_child_num(root)!=1){
            struct Node* exp= get_nth_child(root,2);
            Type Right_type = translate_exp(exp,new_item->op);
            //mismatched for assignment
            if (!type_equal(Right_type,var_type)){
                print_error(4, root);
            }
        }else{
            //printf("var name: %s  %d   %d\n",new_item->name,new_item->type->Kind,temp->int_val);
            struct InterCode* ir_0=new_InterCode();
            ir_0->kind=DEC_;
            ir_0->unary_op.left=new_item->op;
            ir_0->dec_size= get_memory_size(new_item->type);
            insert_IR(ir_0);
        }
    }
    return var_dec;
}
/*!
 * DecList → Dec
    | Dec COMMA DecList
    int   a,b;
 * @param root
 * @param var_type
 * @return
 */
FieldList translate_dec_list(struct Node *root, Type var_type){
    FieldList var_list=translate_dec(root->child, var_type);
    ////printf("return from dec\n");
    if (get_child_num(root)!=1){//dec comma dec_list
        //printf("not one dec lineno:%d\n",root->lineno);
        struct Node *dec_list= get_nth_child(root,2);
        if (var_list != nullptr){
            //printf("first not empty!  %s\n",var_list->name);
            var_list->tail = translate_dec_list(dec_list,var_type);
            //printf("second:%s\n",var_list->tail->name);
        } else {
            var_list = translate_dec_list(dec_list,var_type);
        }
    }
    return var_list;
}
/*!
 * Def → Specifier DecList SEMI
 */
FieldList translate_def(struct Node* root){
    Type spec_type=translate_specifier(root->child);
    ////printf("return from spec %d\n",spec_type->content.basic);
    if(spec_type==nullptr) return nullptr;
    struct Node* dec_list= get_nth_child(root,1);
    FieldList var_list=translate_dec_list(dec_list,spec_type);
    //printf("return from dec_list\n");
    return var_list;// has been inserted
}
/*!
 * DefList → Def DefList
    | empty
 * @param root
 * @return
 */
FieldList translate_def_list(struct Node* root){
    if(root==nullptr){
        return nullptr;
    }
    FieldList struct_field=translate_def(root->child);
    FieldList Tail=struct_field;
    if(struct_field!=nullptr){
        while(Tail->tail!=nullptr){
            Tail=Tail->tail;
        }
    }
    //printf("return from def\n");
    struct Node* def_list= get_nth_child(root,1);
    if(struct_field!=nullptr){
        Tail->tail= translate_def_list(def_list);
    }
    else{
        struct_field= translate_def_list(def_list);
    }
    return struct_field;
}
/*!
 *ExtDecList → VarDec
    | VarDec COMMA ExtDecList
 */
void translate_ext_dec_list(struct Node *root, Type var_type){
    FieldList var_list=translate_var_dec(root->child,var_type);
    struct table_item* new_item=(struct table_item*)malloc(sizeof(struct table_item));
    new_item->type=var_list->type;
    new_item->name= strdup(var_list->name);
    new_item->scope=scope_level;
    new_item->op=new_variable();//array?
    if (!add_symbol(new_item)) {
        print_error(2, root->child, var_list->name);
    }
    if (get_child_num(root) != 1) {
        struct Node* ext_dec_list= get_nth_child(root,2);
        translate_ext_dec_list(ext_dec_list,var_type);
    }
}
/*!
 * ExtDefList → ExtDef ExtDefList
    ExtDef → Specifier ExtDecList SEMI //global var
    | Specifier SEMI //int ;
    | Specifier FunDec CompSt//func
    | Specifier FunDec SEMI //func declare
 * @param root
 */
void translate_ext_def(struct Node *root) {
    Type var_type = translate_specifier(get_nth_child(root,0));
    //printf("return from specifier,%d\n",var_type->Kind);
    struct Node* no_2_child= get_nth_child(root,1);
    char *branch= strdup(no_2_child->value.str_value);
    if(strcmp(branch,"ExtDecList")==0){
        translate_ext_dec_list(no_2_child, var_type);
        return;
    }
    else if(strcmp(branch,"FunDec")==0){
        ret_type = var_type;
        char* semi=strdup(get_nth_child(root,2)->value.str_value);
        if(strcmp(semi,"SEMI")==0){
            //func declare
            scope_level++;
            translate_fun_dec(get_nth_child(root,1),var_type,true);
            symbol_stack_pop(scope_level);
            scope_level--;
            ret_type=nullptr;
            return;
        }else{
            scope_level++;
            translate_fun_dec(get_nth_child(root,1),var_type,false);
            //printf("return from fundec\n");
            translate_compst(get_nth_child(root,2));
            //printf("return from compst\n");
            symbol_stack_pop(scope_level);
            //printf("pop finished!!\n");
            scope_level--;
            ret_type = nullptr;
            return;
        }
    }
}
/*!
 * Stmt → Exp SEMI 2
    | CompSt 1
    | RETURN Exp SEMI 3
    | IF LP Exp RP Stmt 5
    | IF LP Exp RP Stmt ELSE Stmt 7
    | WHILE LP Exp RP Stmt 5
 * @param root
 */
void translate_stmt(struct Node *root) {
    Type var_type=nullptr;
    struct Operand_ *t1,*t2,*label1,*label2,*label3;
    struct InterCode *ir_0;
    ////printf("in translate_stmt\n");
    int child_num= get_child_num(root);
    //printf("%d   %d\n",child_num,root->lineno);
    switch(child_num){
        case 1://compst
            scope_level++;
            translate_compst(root->child);
            symbol_stack_pop(scope_level);
            scope_level--;
            return;
        case 2://EXP SEMI
            //printf("Exp SEMI\n");
            translate_exp(root->child,nullptr);
            return;
        case 3://RETURN Exp SEMI 3
            t1=new_temp_var();
            var_type= translate_exp(get_nth_child(root,1),t1);
            if(var_type!=nullptr){
                if(!type_equal(var_type,ret_type)){
                    print_error(7,root->child);
                    return;
                }
            }
            ir_0=new_InterCode();
            ir_0->kind=RETURN_;
            ir_0->unary_op.left=t1;
            insert_IR(ir_0);
            return;
        case 7://IF LP Exp RP Stmt ELSE Stmt 7
            //printf("if\n");
            //printf("here\n");
            label1=new_label();
            label2=new_label();
            label3=new_label();
            var_type= translate_cond(get_nth_child(root,2),label1,label2);
            //printf("return from if cond\n");
            if(var_type!=nullptr){
                if(!(var_type->Kind==BASIC&&var_type->content.basic==0)){
                    print_error(6,root);
                }
            }
            ir_0=new_InterCode();
            ir_0->kind=LABEL_;
            ir_0->unary_op.left=label1;
            insert_IR(ir_0);
            translate_stmt(get_nth_child(root,4));
            ir_0=new_InterCode();
            ir_0->kind=GOTO_;
            ir_0->unary_op.left=label3;
            insert_IR(ir_0);
            ir_0=new_InterCode();
            ir_0->kind=LABEL_;
            ir_0->unary_op.left=label2;
            insert_IR(ir_0);
            //printf("retrun from if stmt\n");
            translate_stmt(get_nth_child(root,6));
            //printf("return from else stmt\n");
            ir_0=new_InterCode();
            ir_0->kind=LABEL_;
            ir_0->unary_op.left=label3;
            insert_IR(ir_0);
            return;
        default://IF or WHILE
            break;
    }
    char* name=strdup(root->child->value.str_value);
    if(strcmp(name,"IF")==0){
        //| IF LP Exp RP Stmt 5
        label1=new_label();
        label2=new_label();
        var_type= translate_cond(get_nth_child(root,2),label1,label2);
        if(var_type!=nullptr){
             if(!(var_type->Kind==BASIC&&var_type->content.basic==0)){
                 print_error(6,root);
             }
        }
        ir_0=new_InterCode();
        ir_0->kind=LABEL_;
        ir_0->unary_op.left=label1;
        insert_IR(ir_0);
        translate_stmt(get_nth_child(root,4));
        ir_0=new_InterCode();
        ir_0->kind=LABEL_;
        ir_0->unary_op.left=label2;
        insert_IR(ir_0);
        return;
    }
    //| WHILE LP Exp RP Stmt 5
    else{
        label1=new_label();
        label2=new_label();
        label3=new_label();
        ir_0=new_InterCode();
        ir_0->kind=LABEL_;
        ir_0->unary_op.left=label1;
        insert_IR(ir_0);
        var_type= translate_cond(get_nth_child(root,2),label2,label3);
        if(var_type!=nullptr){
            if(!(var_type->Kind==BASIC&&var_type->content.basic==0)){
                print_error(6,root);
            }
        }
        ir_0=new_InterCode();
        ir_0->kind=LABEL_;
        ir_0->unary_op.left=label2;
        insert_IR(ir_0);
        translate_stmt(get_nth_child(root,4));
        ir_0=new_InterCode();
        ir_0->kind=GOTO_;
        ir_0->unary_op.left=label1;
        insert_IR(ir_0);
        ir_0=new_InterCode();
        ir_0->kind=LABEL_;
        ir_0->unary_op.left=label3;
        insert_IR(ir_0);
        return;
    }
}
/*!
 * CompSt → LC DefList StmtList RC
 * @param root
 */
void translate_compst(struct Node *root) {
    struct Node* def_list= get_nth_child(root,1);
    char* name= strdup(def_list->value.str_value);
    //printf("%s  \n",name);
    if(strcmp(name,"DefList")==0){
        translate_def_list(def_list);
        //printf("return from deflist\n");
        struct Node* stmt_list= get_nth_child(root,2);
        name= strdup(stmt_list->value.str_value);
        if(strcmp(name,"StmtList")==0){
            //printf("going to stmtlist\n");
            translate(stmt_list);
            //printf("back from stmtlist!!!!!\n");
            return;
        }
        else{
            return;
        }
    }
    else if(strcmp(name,"StmtList")==0){
        //printf("translate stmt list\n");
        translate(def_list);
        return;
    }
}
bool type_equal(Type a, Type b){
    //printf("%d %d\n",a->Kind,b->Kind);
    if (a == nullptr || b == nullptr) return true;
    if(a->Kind!=b->Kind) return false;
    if (a == b) return true;
    switch (a->Kind){
        case BASIC:
            return a->content.basic==b->content.basic;
        case ARRAY:
            /*!
             * int a[2][3]
             *kind:array content:size:2 elem->{kind: array content:{size:3,elem->{kind:basic,content:0}}}
             */
            return type_equal(a->content.array.elem,b->content.array.elem);
        case STRUCTURE:
            if(a->content.structure==b->content.structure){
                return true;
            }
            else{
                FieldList a_field=a->content.structure;
                FieldList b_field=b->content.structure;
                while(a_field&&b_field){
                    if(!type_equal(a_field->type,b_field->type)){
                        return false;
                    }
                    a_field=a_field->tail;
                    b_field=b_field->tail;
                }
                if(a_field||b_field) return false;
                return true;
            }
        case FUNCTION:
            return false;
    }
    return false;
}
/*!
 * type name next
 * @param a
 * @param b
 * @return
 */
bool param_list_equal(Param_list a, Param_list b){
    //printf("param equal\n");
    while (a && b) {
        if (!type_equal(a->param_type,b->param_type)) return false;
        a = a->next;
        b = b->next;
    }
    //printf("okkkkkkk\n");
    if(a||b) return false;
    else return true;
}
/*!
 * ParamDec → Specifier VarDec
 */
Param_list translate_param_dec(struct Node* root){
    Param_list param=(struct Param*)malloc(sizeof(struct Param));
    Type var_type= translate_specifier(get_nth_child(root,0));
    FieldList param_res=translate_var_dec(get_nth_child(root,1),var_type);
    param->name=param_res->name;
    param->param_type=param_res->type;
    return param;
}
/*!
 * VarList → ParamDec COMMA VarList
    | ParamDec
 */
Param_list translate_var_list(struct Node* root){
    int child_num= get_child_num(root);
    if(child_num==1){
        return translate_param_dec(root->child);
    }
    else{
        Param_list params= translate_param_dec(root->child);
        params->next= translate_var_list(get_nth_child(root,2));
        return params;
    }
}
/*!
 * FunDec → ID LP VarList RP
    | ID LP RP
 * @param root
 * @param ret_type
 */
void translate_fun_dec(struct Node *root, Type ret_type,bool is_declare) {
    //printf("in func_dec\n");
    struct table_item* func=(struct table_item*)malloc(sizeof(struct table_item));
    Type func_type= (struct Type_*)malloc(sizeof(struct Type_));
    func->name= strdup(root->child->value.str_value);
    struct Operand_* temp= new_function(func->name);
    struct InterCode* ir_0=new_InterCode();
    ir_0->kind=FUNCTION_;
    ir_0->unary_op.left=temp;
    insert_IR(ir_0);
    //printf("in fun_dec: %s\n",func->name);
    func_type->Kind=FUNCTION;
    func_type->content.func.return_type=ret_type;
    func_type->content.func.param_list=nullptr;
    func_type->content.func.ast_node=root;
    struct Node* var_list= get_nth_child(root,2);
    //printf("%s %d\n",func->name,root->lineno);
    if (strcmp(var_list->value.str_value,"VarList")==0){
        //printf("here!!\n");
        func_type->content.func.param_list= translate_var_list(var_list);
        // in declare maybe redefinition
        Param_list head=func_type->content.func.param_list;
        while(head!=nullptr){
            struct table_item* new_item=(struct table_item*) malloc(sizeof(struct table_item));
            new_item->op=new_variable();
            new_item->name=head->name;
            new_item->type=head->param_type;
            switch (new_item->type->Kind) {
                case STRUCTURE:
                case ARRAY:
                    new_item->op->is_addr=true;
                    break;
                default:
                    break;
            }
            new_item->scope=scope_level;
            //printf("param: %s  %d\n",new_item->name,head->param_type->Kind);
            if(!add_symbol(new_item)){
                //printf("wrong!!!");
                print_error(2,root,new_item->name);
            }
            ir_0=new_InterCode();
            ir_0->kind=PARAM_;
            ir_0->unary_op.left=new_item->op;
            insert_IR(ir_0);
            head=head->next;
        }
        //printf("return from var_list\n");
    }
    func->type=func_type;
    func->scope=scope_level-1;
    func->op= new_function(func->name);
    struct table_item* func_res= find_symbol(func->name);
    //printf("find %d\n",func_res->type->Kind);
    if(func_res==nullptr||func_res->type->Kind!=FUNCTION){
        if(is_declare){
            func_type->content.func.State=DECLARED;
            add_symbol(func);
        }else{
            func_type->content.func.State=DEFINED;
            add_symbol(func);
        }
    }
    else{
        bool consistent=type_equal(func->type->content.func.return_type,func_res->type->content.func.return_type)&&
                                param_list_equal(func->type->content.func.param_list,func_res->type->content.func.param_list);
        if(func_res->type->content.func.State==DECLARED){
            if(!consistent){
                print_error(18,root,func->name);
                return;
            }else{
                if(!is_declare){
                    func_res->type->content.func.State=DEFINED;
                }
            }
        }else{
            //res:define
            if(!consistent){
                if(is_declare){
                    print_error(18,root,func->name);
                    return;
                }else{
                    print_error(3,root,func_res->name);
                    return;
                }
            }
            else{
                if(!is_declare){
                    print_error(3,root,func->name);
                }
            }
        }
        return;
    }
}
/*!
 * Args → Exp COMMA Args   // shi can lie biao
         | Exp
 */
Param_list create_param_list(struct Node* root){
    //printf("translate_ARGS\n");
    struct Operand_*t1;
    struct Node* Args=root;
    Param_list head=(struct Param*)malloc(sizeof(struct Param)),tail=head;
    struct Node* exp=Args->child;
    t1=new_temp_var();
    head->param_type=translate_exp(exp,t1);
    head->next=nullptr;
    head->op=t1;
    Args= get_nth_child(Args,2);
    //func_params->param_type=translate_exp(exp);
    while(Args!=nullptr){
        //printf("111\n");
        Param_list temp=(struct Param*)malloc(sizeof(struct Param));
        t1=new_temp_var();
        exp=Args->child;
        temp->param_type=translate_exp(exp,t1);
        temp->next=nullptr;
        temp->op=t1;
        tail->next=temp;
        tail=temp;
        Args=get_nth_child(Args,2);
    }
    return head;
}
/*!
 * Exp → Exp ASSIGNOP Exp
    | Exp AND Exp
    | Exp OR Exp
    | Exp RELOP Exp
    | Exp PLUS Exp
    | Exp MINUS Exp
    | Exp STAR Exp
    | Exp DIV Exp
    | LP Exp RP
    | MINUS Exp
    | NOT Exp
    | ID LP Args RP
    | ID LP RP
    | Exp LB Exp RB
    | Exp DOT ID
    | ID
    | INT
    | FLOAT
Args → Exp COMMA Args   // shi can lie biao
    | Exp
 * @param root
 * @return
 */
Type translate_exp(struct Node* root,struct Operand_* op){
    //printf("%s\n",root->parent->value.str_value);
    struct Operand_ *t1,*t2,*label1,*label2;
    struct InterCode* ir_0;
    char* name;
    struct table_item* symbol;
    struct Type_ *left_type=(struct Type_*)malloc(sizeof(struct Type_)),*right_type=(struct Type_*)malloc(sizeof(struct Type_));
    struct Node* no_1_child= get_nth_child(root,0);
    int child_num= get_child_num(root);
    if(child_num==0) return nullptr;
    //printf("child_num:%d\n",child_num);
    if(child_num==1){
        //printf("yes\n");
        switch (no_1_child->token_) {
            case INT_:
                if(op){
                    t1=new_constant();
                    t1->int_val=root->child->value.int_value;
                    ir_0=new_InterCode();
                    ir_0->kind=ASSIGN_;
                    ir_0->unary_op.left=op;
                    ir_0->unary_op.right=t1;
                    insert_IR(ir_0);
                }
                //printf("int val:%d\n",t1->int_val);
                left_type->Kind=BASIC;
                left_type->content.basic=0;
                return left_type;
                break;
            case FLOAT_:
                left_type->Kind=BASIC;
                left_type->content.basic=1;
                return left_type;
                break;
            case ID_:
                //printf("exp_id\n");
                name= strdup(no_1_child->value.str_value);
                //printf("is finding %s\n",name);
                symbol= find_symbol(name);
                if(symbol==nullptr){
                    //printf("here!!!\n");
                    print_error(0,root,name);
                    return nullptr;
                }
                if(op){
                    if(symbol->type->Kind==STRUCTURE||symbol->type->Kind==ARRAY){
                        if(!symbol->op->is_addr){
                            t1=new_temp_var();
                            ir_0=new_InterCode();
                            ir_0->kind=ASSIGN_;
                            ir_0->unary_op.left=op;
                            t2=new_variable();
                            *t2=*(symbol->op);
                            t2->Ref_Def=Reference;
                            ir_0->unary_op.right=t2;
                            insert_IR(ir_0);
                        }
                        else{
                            ir_0=new_InterCode();
                            ir_0->kind=ASSIGN_;
                            ir_0->unary_op.left=op;
                            ir_0->unary_op.right=symbol->op;
                            insert_IR(ir_0);
                        }
                    }else{
                        ir_0=new_InterCode();
                        ir_0->kind=ASSIGN_;
                        ir_0->unary_op.left=op;
                        ir_0->unary_op.right=symbol->op;
                        insert_IR(ir_0);
                    }
                }
                left_type=symbol->type;
                return left_type;
                break;
            default:
                //other
                break;
        }
    }
    else{
        char* no_1_name=strdup(root->child->value.str_value);
        //    | LP Exp RP
        if(strcmp(no_1_name,"LP")==0){
            return translate_exp(get_nth_child(root,1),op);
        }
        //    | MINUS Exp
        else if(strcmp(no_1_name,"MINUS")==0){
            t1=new_temp_var();
            Type cal_type= translate_exp(get_nth_child(root,1),t1);
            if(cal_type->Kind!=BASIC){
                print_error(6, root);
                return nullptr;
            }
            if(op){
                ir_0=new_InterCode();
                ir_0->kind=SUB_;
                struct Operand_* temp=new_constant();
                ir_0->binary_op.left=temp;
                ir_0->unary_op.right=t1;
                ir_0->binary_op.res=op;
                insert_IR(ir_0);
            }
            return cal_type;
        }
        //   | NOT Exp
        else if(strcmp(no_1_name,"NOT")==0){
            //printf("NOT\n");
            if(op){
                ir_0=new_InterCode();
                ir_0->kind=ASSIGN_;
                ir_0->unary_op.left=op;
                ir_0->unary_op.right=new_constant();
                insert_IR(ir_0);
            }
            label1=new_label();
            label2=new_label();
            Type not_type= translate_cond(root,label1,label2);
            ir_0=new_InterCode();
            ir_0->kind=LABEL_;
            ir_0->unary_op.left=label1;
            insert_IR(ir_0);
            if(op){
                ir_0=new_InterCode();
                ir_0->kind=ASSIGN_;
                ir_0->unary_op.left=op;
                struct Operand_* temp=new_constant();
                temp->int_val=1;
                ir_0->unary_op.right=temp;
                insert_IR(ir_0);
            }
            ir_0=new_InterCode();
            ir_0->kind=LABEL_;
            ir_0->unary_op.left=label2;
            insert_IR(ir_0);
            return not_type;
        }
        //      Exp ASSIGNOP Exp .
        //    | Exp AND Exp .
        //    | Exp OR Exp .
        //    | Exp RELOP Exp.
        //    | Exp PLUS Exp.
        //    | Exp MINUS Exp
        //    | Exp STAR Exp
        //    | Exp DIV Exp
        //    | Exp LB Exp RB
        //    | Exp DOT ID
        else if(strcmp(no_1_name,"Exp")==0){
            //printf("Ex\n");
            char* no_2_name= get_nth_child(root,1)->value.str_value;
            bool is_cal=!(strcmp(no_2_name,"PLUS"))||!(strcmp(no_2_name,"MINUS"))||!(strcmp(no_2_name,"STAR"))||!(strcmp(no_2_name,"DIV"));
            bool is_logic=!(strcmp(no_2_name,"AND"))||!(strcmp(no_2_name,"OR"));
            bool is_relop=!(strcmp(no_2_name,">="))||!(strcmp(no_2_name,"<="))||!(strcmp(no_2_name,"=="))||!(strcmp(no_2_name,">"))||!(strcmp(no_2_name,"<"))||!(strcmp(no_2_name,"!="));
            //EXP ASSIGN EXP
            if(strcmp(no_2_name,"ASSIGNOP")==0){
                /*!
                 *  | Exp DOT ID
                    | ID
                    | Exp LB Exp RB
                 */
                int exp_left_num_child= get_child_num(root->child);
                bool left_cond=((exp_left_num_child==1)&&(root->child->child->token_==ID_))||((exp_left_num_child==3)&&(strcmp(get_nth_child(root->child,1)->value.str_value,"DOT")==0))||((exp_left_num_child==4)&&(strcmp(get_nth_child(root->child,1)->value.str_value,"LB")==0));
                if(!left_cond){
                    print_error(5,root);
                    return nullptr;
                }
                t1=new_temp_var();
                t2=new_temp_var();
                left_type= translate_exp_to_addr(get_nth_child(root,0),t1);
                right_type= translate_exp(get_nth_child(root,2),t2);
                if(left_type==nullptr||right_type==nullptr){
                    return nullptr;
                }
                if(!type_equal(left_type,right_type)){
                    //printf("here\n");
                    print_error(4,root);
                    return nullptr;
                }
                struct Operand_*t3=new_temp_var();
                *t3=*t1;
                t3->Ref_Def=Deference;
                ir_0=new_InterCode();
                ir_0->kind=ASSIGN_;
                ir_0->unary_op.left=t3;
                ir_0->unary_op.right=t2;
                //print_IR(ir_0);
                //printf("       inserted at %d in test.txt at %d in sdt.c\n",root->lineno,__LINE__);
                insert_IR(ir_0);
                if(op){
                    ir_0=new_InterCode();
                    ir_0->kind=ASSIGN_;
                    ir_0->unary_op.left=op;
                    ir_0->unary_op.right=t1;
                }
                return left_type;
            }
            // + - * /
            else if(is_cal){
                t1=new_temp_var();
                t2=new_temp_var();
                //printf("is_cal\n");
                left_type = translate_exp(get_nth_child(root,0),t1);
                //printf("%d\n",left_type->Kind);
                right_type= translate_exp(get_nth_child(root,2),t2);
                if (left_type == nullptr || right_type == nullptr) return nullptr;
                //int+int float+float
                if(!(left_type->Kind==right_type->Kind&&left_type->Kind==BASIC)){
                    print_error(6, root);
                    return nullptr;
                }
                else{
                    if(left_type->content.basic!=right_type->content.basic){
                        print_error(6,root);
                        return nullptr;
                    }
                    if(op){
                        ir_0=new_InterCode();
                        if(strcmp(no_2_name,"PLUS")==0) ir_0->kind=ADD_;
                        if(strcmp(no_2_name,"MINUS")==0) ir_0->kind=SUB_;
                        if(strcmp(no_2_name,"STAR")==0) ir_0->kind=MUL_;
                        if(strcmp(no_2_name,"DIV")==0) ir_0->kind=DIV_;
                        ir_0->binary_op.left=t1;
                        ir_0->binary_op.right=t2;
                        ir_0->binary_op.res=op;
                        insert_IR(ir_0);
                    }
                    return left_type;
                }
            }
            //AND OR
            else if(is_logic){
                if(op){
                    ir_0=new_InterCode();
                    ir_0->kind=ASSIGN_;
                    ir_0->unary_op.left=op;
                    ir_0->unary_op.right=new_constant();
                    insert_IR(ir_0);
                }
                label1=new_label();
                label2=new_label();
                Type logic_type= translate_cond(root,label1,label2);
                ir_0=new_InterCode();
                ir_0->kind=LABEL_;
                ir_0->unary_op.left=label1;
                insert_IR(ir_0);
                if(op){
                    ir_0=new_InterCode();
                    ir_0->kind=ASSIGN_;
                    ir_0->unary_op.left=op;
                    struct Operand_* temp=new_constant();
                    temp->int_val=1;
                    ir_0->unary_op.right=temp;
                    insert_IR(ir_0);
                }
                ir_0=new_InterCode();
                ir_0->kind=LABEL_;
                ir_0->unary_op.left=label2;
                insert_IR(ir_0);
                return logic_type;
            }
            //RELOP
            else if(is_relop){
                if(op){
                    ir_0=new_InterCode();
                    ir_0->kind=ASSIGN_;
                    ir_0->unary_op.left=op;
                    ir_0->unary_op.right=new_constant();
                    insert_IR(ir_0);
                }
                label1=new_label();
                label2=new_label();
                Type relop_type= translate_cond(root,label1,label2);
                ir_0=new_InterCode();
                ir_0->kind=LABEL_;
                ir_0->unary_op.left=label1;
                insert_IR(ir_0);
                if(op){
                    ir_0=new_InterCode();
                    ir_0->kind=ASSIGN_;
                    ir_0->unary_op.left=op;
                    struct Operand_* temp=new_constant();
                    temp->int_val=1;
                    ir_0->unary_op.right=temp;
                    insert_IR(ir_0);
                }
                ir_0=new_InterCode();
                ir_0->kind=LABEL_;
                ir_0->unary_op.left=label2;
                insert_IR(ir_0);
                return relop_type;
            }
            //   | Exp LB Exp RB a [2] id zai di yi ge Exp de zui di ceng
            //  a [2.5]
            else if(strcmp(no_2_name,"LB")==0){
                t1=new_temp_var();
                left_type= translate_exp_to_addr(root,t1);
                if(left_type==nullptr) return nullptr;
                if(left_type->Kind==STRUCTURE||left_type->Kind==ARRAY){
                    if(op){
                        ir_0=new_InterCode();
                        ir_0->kind=ASSIGN_;
                        ir_0->unary_op.left=op;
                        ir_0->unary_op.right=t1;
                        insert_IR(ir_0);
                    }
                }else{
                    t2=new_temp_var();
                    *t2=*t1;
                    t2->Ref_Def=Deference;
                    if(op){
                        ir_0=new_InterCode();
                        ir_0->kind=ASSIGN_;
                        ir_0->unary_op.left=op;
                        ir_0->unary_op.right=t2;
                        insert_IR(ir_0);
                    }
                }
                return left_type;
                /*
                Type dimension_type= translate_exp(get_nth_child(root,2));
                Type array_type=translate_exp(get_nth_child(root,0));
                int dimension=0;
                if(dimension_type&&dimension_type->Kind!=BASIC){
                    print_error(11,root);
                    return nullptr;
                }
                else if(dimension_type&&dimension_type->content.basic==1){
                    print_error(11,root);
                    return nullptr;
                }
                if(array_type==nullptr){
                    return nullptr;
                }
                else if(array_type&&array_type->Kind!=ARRAY){
                    print_error(9,root);
                    return nullptr;
                }
                else{
                    return array_type->content.array.elem;
                }*/
            }
            //   | Exp DOT ID
            else{
                t1=new_temp_var();
                left_type = translate_exp_to_addr(root, t1);
                if (left_type==nullptr) return nullptr;
                if(left_type->Kind==STRUCTURE||left_type->Kind==ARRAY){
                    if(op){
                        ir_0=new_InterCode();
                        ir_0->kind=ASSIGN_;
                        ir_0->unary_op.left=op;
                        ir_0->unary_op.right=t1;
                        insert_IR(ir_0);
                    }
                }else{
                    t2=new_temp_var();
                    *t2=*t1;
                    t2->Ref_Def=Deference;
                    if(op){
                        ir_0=new_InterCode();
                        ir_0->kind=ASSIGN_;
                        ir_0->unary_op.left=op;
                        ir_0->unary_op.right=t2;
                        insert_IR(ir_0);
                    }
                }
                return left_type;
                /*
                char* field_name= strdup(get_nth_child(root,2)->value.str_value);//!!!!mess.float[2].a
                //printf("Field_name:%s\n",field_name);
                Type StRuct_type= translate_exp(get_nth_child(root,0));

                //printf("Exp dot ID %s %s %s\n", get_nth_child(root,0)->value.str_value, get_nth_child(root,1)->value.str_value,
                //get_nth_child(root,1)->value.str_value);
                if(StRuct_type->Kind!=STRUCTURE){
                    print_error(12,root);
                    return nullptr;
                }
                else{
                    FieldList head=StRuct_type->content.structure;
                    //printf("begin\n");
                    while(head){
                        //printf("filed:%s\n",head->name);
                        if(strcmp(field_name,head->name)==0){
                            //printf("%s: kind:%d",field_name,head->type->content.basic);
                            return head->type;
                        }
                        head=head->tail;
                    }
                    //printf("end\n");
                    if(head==nullptr){
                        print_error(13,root,field_name);
                        return nullptr;
                    }
                }*/
            }

        }
        //    | ID LP Args RP
        //    | ID LP RP
        else{
            if(op==nullptr) op=new_temp_var();
            //printf("func call\n");
            char* func_name= strdup(no_1_name);
            //printf("here!!\n");
            //printf("%s\n",func_name);
            struct table_item* func_res= find_symbol(func_name);
            //printf("find func:%s\n",func_res->name);
            if(func_res==nullptr){
                //function not exist
                ////printf("");
                print_error(1,root,func_name);
                return nullptr;
            }
            else{
                if(func_res->type->Kind!=FUNCTION){
                    print_error(10,root,func_name);
                    return nullptr;
                }
                Type ret_type=func_res->type->content.func.return_type;
                Param_list func_params=func_res->type->content.func.param_list;
                /*!
                 * Args → Exp COMMA Args   // shi can lie biao
                        | Exp
                 */
                if(get_child_num(root)==3) {
                    //no param
                    if (func_params != nullptr) {
                        print_error(8, root, func_name);
                        return nullptr;
                    } else {
                        if (strcmp(func_res->name, "read") == 0) {
                            ir_0 = new_InterCode();
                            ir_0->kind = READ_;
                            ir_0->unary_op.left = op;
                            insert_IR(ir_0);
                        } else {
                            ir_0 = new_InterCode();
                            ir_0->kind = CALL_;
                            ir_0->unary_op.left = op;
                            t1 = new_function(func_res->name);
                            ir_0->unary_op.right = t1;
                            insert_IR(ir_0);
                        }
                        return ret_type;
                    }
                }
                // exp lp args rp
                else{
                    //translate to create param_list
                    //printf("CALL FUNCTION:%s\n",func_name);
                    Param_list call_list=create_param_list(get_nth_child(root,2));
                    //printf("%s   %d\n",func_name,call_list->param_type->Kind);
                    //printf("create paramlist!\n");
                    if(!param_list_equal(call_list,func_params)){
                        //printf("not equal\n");
                        char* types[]={"array","structure","int","float"};
                        char* request_param;
                        char* my_param;
                        if(func_params!=nullptr){
                            int array_index=func_params->param_type->Kind==BASIC?func_params->param_type->content.basic+2:func_params->param_type->Kind-1;
                            request_param= strdup(types[array_index]);
                            //printf("request: %d\n",func_params->param_type->Kind);
                            func_params=func_params->next;
                            while(func_params){
                                int array_index=func_params->param_type->Kind==BASIC?func_params->param_type->content.basic+2:func_params->param_type->Kind-1;
                                strcat(request_param,",");
                                strcat(request_param,types[array_index]);
                                //printf("request: %d\n",func_params->param_type->Kind);
                                func_params=func_params->next;
                            }
                        }
                        else{
                            request_param=strdup(" ");
                        }
                        if(call_list!=nullptr){
                            int array_index=call_list->param_type->Kind==BASIC?call_list->param_type->content.basic+2:call_list->param_type->Kind-1;
                            my_param= strdup(types[array_index]);
                            //printf("NOw: %d\n",call_list->param_type->Kind);
                            call_list=call_list->next;
                            while(call_list){
                                //printf("%s\n",my_param);
                                int array_index=call_list->param_type->Kind==BASIC?call_list->param_type->content.basic+2:call_list->param_type->Kind-1;
                                strcat(my_param,",");
                                strcat(my_param,types[array_index]);
                                //printf("NOw: %d\n",call_list->param_type->Kind);
                                call_list=call_list->next;
                            }
                        }
                        else{
                            my_param=strdup(" ");
                        }
                        print_error(8,root,func_name,request_param,my_param);
                        return nullptr;
                    }
                    else{
                        if(strcmp(func_res->name,"write")==0){
                            ir_0=new_InterCode();
                            ir_0->kind=WRITE_;
                            ir_0->unary_op.left=call_list->op;
                            insert_IR(ir_0);
                            ir_0=new_InterCode();
                            ir_0->kind=ASSIGN_;
                            ir_0->unary_op.left=op;
                            ir_0->unary_op.right=new_constant();
                            insert_IR(ir_0);
                        }else{
                            insert_ARGS(call_list);
                            ir_0=new_InterCode();
                            ir_0->kind=CALL_;
                            ir_0->unary_op.left=op;
                            ir_0->unary_op.right=new_function(func_res->name);
                            insert_IR(ir_0);
                        }
                        return ret_type;
                    }
                }
            }
        }
    }
    return nullptr;
}
void insert_ARGS(Param_list paramList){
    if(paramList->next!=nullptr){
        insert_ARGS(paramList->next);
    }
    struct InterCode* ir_0=new_InterCode();
    /*
    if(paramList->param_type->Kind==STRUCTURE||paramList->param_type->Kind==ARRAY){
        ir_0->kind=ARG_;
        struct Operand_* temp=new_variable();
        *temp=*(paramList->op);
        temp->Ref_Def=Reference;
        ir_0->unary_op.left=temp;
        insert_IR(ir_0);
    }*/
    ir_0->kind=ARG_;
    ir_0->unary_op.left=paramList->op;
    insert_IR(ir_0);
    return;
}
struct Operand_ *new_temp_var() {
    struct Operand_* temp=(struct  Operand_*) malloc(sizeof(struct Operand_));
    temp->Ref_Def=BaseCond;
    temp->kind=TEMP_VAR;
    temp->no=temp_var_no;
    temp->is_addr=false;
    temp_var_no++;
    return temp;
}
struct Operand_ *new_variable() {
    struct Operand_* temp=(struct  Operand_*) malloc(sizeof(struct Operand_));
    temp->Ref_Def=BaseCond;
    temp->kind=VARIABLE;
    temp->no=variable_no;
    temp->is_addr=false;
    variable_no++;
    return temp;
}
struct Operand_ *new_label() {
    struct Operand_* temp=(struct  Operand_*) malloc(sizeof(struct Operand_));
    temp->Ref_Def=BaseCond;
    temp->kind=OP_LABEL;
    temp->no=label_no;
    temp->is_addr=false;
    label_no++;
    return temp;
}
struct Operand_ *new_constant(){
    struct Operand_* temp=(struct  Operand_*) malloc(sizeof(struct Operand_));
    temp->Ref_Def=BaseCond;
    temp->kind=CONSTANT;
    temp->int_val=0;
    temp->is_addr=false;
    return temp;
}
struct Operand_ *new_function(char* func_name){
    struct Operand_* temp=(struct  Operand_*) malloc(sizeof(struct Operand_));
    temp->Ref_Def=BaseCond;
    temp->kind=OP_FUNCTION;
    temp->name= strdup(func_name);
    temp->is_addr=false;
    return temp;
}
struct InterCode* new_InterCode(){
    struct InterCode* temp=(struct  InterCode*) malloc(sizeof(struct InterCode));
    return temp;
}
int get_memory_size(struct Type_ *var){
    int size=0;
    FieldList struct_field;
    switch (var->Kind){
        case BASIC:
            size=4;
            break;
        case ARRAY:
            /*!
             * int a[2][3]
             *kind:array content:size:2 elem->{kind: array content:{size:3,elem->{kind:basic,content:0}}}
             */
            size=var->content.array.size;
            var=var->content.array.elem;
            size*= get_memory_size(var);
            break;
        case STRUCTURE:
            struct_field=var->content.structure;
            if(struct_field==nullptr) break;
            size= get_memory_size(struct_field->type);
            struct_field=struct_field->tail;
            while(struct_field){
                size+= get_memory_size(struct_field->type);
                struct_field=struct_field->tail;
            }
            break;
        default:
            break;
    }
    return size;
}
int cal_field_offset(struct Type_ * struct_type, char* name){
    FieldList struct_field=struct_type->content.structure;
    int offset=0;
    while (struct_field){
        if(strcmp(struct_field->name,name)==0) break;
        else{
            offset+= get_memory_size(struct_field->type);
            struct_field=struct_field->tail;
        }
    }
    return offset;
}
enum Relop_type get_relop(struct Node *root){
    if (strcmp(root->value.str_value, "==") == 0) return RELOP_EQ;
    if (strcmp(root->value.str_value, "!=") == 0) return RELOP_NEQ;
    if (strcmp(root->value.str_value, ">" ) == 0) return RELOP_G;
    if (strcmp(root->value.str_value, "<" ) == 0) return RELOP_L;
    if (strcmp(root->value.str_value, ">=") == 0) return RELOP_GE;
    if (strcmp(root->value.str_value, "<=") == 0) return RELOP_LE;
}
struct Type_* translate_cond(struct  Node* root,struct Operand_ *label_true, struct Operand_ *label_false) {
    struct Operand_ *t1=new_temp_var(), *t2=new_temp_var(), *label1=new_label();
    struct Type_ *left, *right;
    int child_num= get_child_num(root);
    //EXP->INT:1
    if(child_num==3){
        char *logic= strdup(root->child->peer->value.str_value);
        if(strcmp(get_nth_child(root,0)->value.str_value,"LP")==0){
            return translate_cond(get_nth_child(root,1),label_true,label_false);
        }
        else if(get_nth_child(root,1)->token_==RELOP_){
            //printf("in exp relop exp    (%d,%d)\n",root->lineno,root->column);
            left= translate_exp(get_nth_child(root,0),t1);
            right= translate_exp(get_nth_child(root,2),t2);
            //if(right==nullptr) printf("right null\n");
            if (left == nullptr || right == nullptr) return nullptr;
            struct InterCode* ir_0=new_InterCode();
            ir_0->kind=IF_GOTO_;
            ir_0->binary_op.left=t1;
            ir_0->binary_op.right=t2;
            ir_0->binary_op.res=label_true;
            ir_0->Relop= get_relop(get_nth_child(root,1));
            insert_IR(ir_0);
            struct InterCode* ir_1=new_InterCode();
            ir_1->kind=GOTO_;
            ir_1->unary_op.left=label_false;
            insert_IR(ir_1);
            //printf("in exp relop exp    %d\n",root->lineno);
            if(!(left->Kind==right->Kind&&left->Kind==BASIC)){
                print_error(6, root);
                return nullptr;
            }else{
                if(left->content.basic!=right->content.basic){
                    print_error(6,root);
                    return nullptr;
                }
                //printf("no_error\n");
                struct Type_* int_type=(struct Type_*)malloc(sizeof(struct Type_));
                int_type->Kind=BASIC;
                int_type->content.basic=0;
                return int_type;
            }
        }
        else if(strcmp(logic,"AND")==0){
            struct InterCode* ir_0=new_InterCode();
            //printf("in exp AND exp    (%d,%d)\n",root->lineno,root->column);
            left= translate_cond(get_nth_child(root,0),label1,label_false);
            //printf("return from and left %d\n",left->Kind);
            ir_0->kind=LABEL_;
            ir_0->unary_op.left=label1;
            insert_IR(ir_0);
            right= translate_cond(get_nth_child(root,2),label_true,label_false);
            if (left == nullptr || right == nullptr) return nullptr;
            if(!(left->Kind==right->Kind&&left->Kind==BASIC)){
                print_error(6,root);
                return nullptr;
            }
            else{
                if(left->content.basic!=0){
                    print_error(6,root);
                    return nullptr;
                }
            }
            return left;
        }
        else if(strcmp(logic,"OR")==0){
            struct InterCode* ir_0=new_InterCode();
            left= translate_cond(get_nth_child(root,0),label_true,label1);
            ir_0->kind=LABEL_;
            ir_0->unary_op.left=label1;
            insert_IR(ir_0);
            right= translate_cond(get_nth_child(root,2),label_true,label_false);
            if (left == nullptr || right == nullptr) return nullptr;
            if(!(left->Kind==right->Kind&&left->Kind==BASIC)){
                print_error(6,root);
                return nullptr;
            }
            else{
                if(left->content.basic!=0){
                    print_error(6,root);
                    return nullptr;
                }
            }
            return left;
        }
    }
    else if(child_num==2){
        //Not exp
        Type not_type= translate_cond(get_nth_child(root,1),label_false,label_true);
        if(not_type->Kind!=BASIC){
            print_error(6,root);
            return nullptr;
        }
        else{
            if(not_type->content.basic!=0){
                print_error(6,root);
                return nullptr;
            }
        }
        return not_type;
    }
    else{
        left=translate_exp(root,t1);
        t2->kind=CONSTANT;
        t2->int_val=0;
        struct InterCode* ir_0=new_InterCode(),*ir_1=new_InterCode();
        ir_0->kind=IF_GOTO_;
        ir_0->binary_op.left=t1;
        ir_0->binary_op.right=t2;
        ir_0->binary_op.res=label_true;
        ir_0->Relop=RELOP_NEQ;
        insert_IR(ir_0);
        ir_1->kind=GOTO_;
        ir_1->unary_op.left=label_false;
        insert_IR(ir_1);
        return left;
    }
}
/*
 * exp dot id
 * exp lb exp rb
 */
struct Type_ *translate_exp_to_addr(struct Node *root, struct Operand_ *op) {
    char* name;
    struct table_item *symbol;
    struct Operand_ *t1, *t2, *t3, *t4;
    struct Type_ *vt1, *vt2;
    struct InterCode* ir_0;
    int child_num= get_child_num(root);
    switch (child_num) {
        case 1:
            //ID
            name = strdup(root->child->value.str_value);
            symbol = find_symbol(name);
            if (symbol == nullptr) {
                print_error(0, root, name);
                return nullptr;
            }
            t1 = symbol->op;
            t2=new_temp_var();
            *t2=*t1;
            if(!t1->is_addr){
                t2->Ref_Def=Reference;
                t2->is_addr=true;
            }
            if(op){
                ir_0=new_InterCode();
                ir_0->kind=ASSIGN_;
                ir_0->unary_op.left=op;
                ir_0->unary_op.right=t2;
                insert_IR(ir_0);
            }
            return symbol->type;
        case 3:
            //lp exp rp
            if(strcmp(root->child->value.str_value,"LP")==0){
                return translate_exp_to_addr(get_nth_child(root,1),op);
            }
            //EXP DOT ID
            else if(strcmp(root->child->peer->value.str_value,"DOT")==0){
                t1=new_temp_var();
                Type StRuct_type= translate_exp_to_addr(get_nth_child(root,0), t1);
                char* field_name = strdup(get_nth_child(root,2)->value.str_value);
                if (StRuct_type==nullptr) return nullptr;
                if (StRuct_type->Kind != STRUCTURE) {
                    print_error(12, root);
                    return nullptr;
                }
                else{
                    FieldList head=StRuct_type->content.structure;
                    struct Type_* field_type;
                    while(head){
                        //printf("filed:%s\n",head->name);
                        if(strcmp(field_name,head->name)==0){
                            field_type=head->type;
                            break;
                        }
                        head=head->tail;
                    }
                    if(head==nullptr){
                        print_error(13,root,field_name);
                        return nullptr;
                    }
                    else{
                        int field_offset= cal_field_offset(StRuct_type,field_name);
                        t2=new_constant();
                        t2->int_val=field_offset;
                        if(op){
                            ir_0=new_InterCode();
                            ir_0->kind=ADD_;
                            ir_0->binary_op.left=t1;
                            ir_0->binary_op.right=t2;
                            ir_0->binary_op.res=op;
                            insert_IR(ir_0);
                        }
                        return field_type;
                    }
                }
            }
        case 4:
            //array a[3][2]
            t1=new_temp_var();
            t2=new_temp_var();
            vt1 = translate_exp_to_addr(get_nth_child(root,0), t1);
            if (vt1&&vt1->Kind!= ARRAY) {
                print_error(9, root);
                return nullptr;
            }
            vt2 = translate_exp(get_nth_child(root,2), t2 );
            if ((vt2&&vt2->Kind != BASIC) || (vt2&&vt2->content.basic != 0)) {
                print_error(11, root);
                return nullptr;
            }
            struct Type_ *elem_type;
            elem_type=vt1->content.array.elem;
            if (elem_type->Kind!= ARRAY){
                // last dimension
                if (t2->kind==CONSTANT) {
                    //cal addr
                    t3=new_constant();
                    int elem_size= get_memory_size(elem_type);
                    t3->int_val=elem_size*t2->int_val;
                    if(op){
                        ir_0=new_InterCode();
                        ir_0->kind=ADD_;
                        ir_0->binary_op.left=t1;
                        ir_0->binary_op.right=t3;
                        ir_0->binary_op.res=op;
                        insert_IR(ir_0);
                    }
                } else {
                    //a[b];
                    t4=new_constant();
                    t4->int_val= get_memory_size(elem_type);
                    ir_0=new_InterCode();
                    ir_0->kind=MUL_;
                    t3=new_temp_var();
                    ir_0->binary_op.left=t2;
                    ir_0->binary_op.right=t4;
                    ir_0->binary_op.res=t3;
                    insert_IR(ir_0);
                    //printf("MUL here at %d %d\n",__LINE__,t2->kind);
                    if(op){
                        ir_0=new_InterCode();
                        ir_0->kind=ADD_;
                        ir_0->binary_op.left=t1;
                        ir_0->binary_op.right=t3;
                        ir_0->binary_op.res=op;
                        insert_IR(ir_0);
                    }
                }
            } else {
                int size= get_memory_size(elem_type);
                t3=new_constant();
                t4=new_temp_var();
                t3->int_val=size;
                ir_0=new_InterCode();
                ir_0->kind=MUL_;
                //printf("MUL here %d\n",__LINE__);
                ir_0->binary_op.left=t2;
                ir_0->binary_op.right=t3;
                ir_0->binary_op.res=t4;
                insert_IR(ir_0);
                if(op){
                    ir_0=new_InterCode();
                    ir_0->kind=ADD_;
                    ir_0->binary_op.left=t1;
                    ir_0->binary_op.right=t4;
                    ir_0->binary_op.res=op;
                    insert_IR(ir_0);
                }
            }
            return elem_type;
        default:
            break;
    }
    return nullptr;
}