//
// Created by plutoer on 22-10-19.
//
#include "symbol_table.h"
extern struct table_item* Symbol_Hashtable[Table_size];
extern struct table_item* SymbolTable_stack[MAX_NEST_NUM];
unsigned int hash_pjw(char* name){
    unsigned int val = 0, i;
    for (; *name; ++name) {
        val = (val << 2) + *name;
        if ((i = val & ~Table_size)) val = (val ^ (i >> 12)) &Table_size;
    }
    return val;
}
void Initialize(){
    //add read and write
    for(int i=0;i<Table_size;i++){
        struct table_item* temp=(struct table_item*) malloc(sizeof(struct table_item));
        temp->hash_next=temp->hash_prev=nullptr;
        Symbol_Hashtable[i]=temp;
    }
    for(int i=0;i<MAX_NEST_NUM;i++){
        struct table_item* temp=(struct table_item*) malloc(sizeof(struct table_item));
        temp->stack_next=temp->stack_prev=nullptr;
        SymbolTable_stack[i]=temp;
    }
    struct Type_* int_type=(struct Type_*) malloc(sizeof(struct Type_));
    int_type->Kind=BASIC;
    int_type->content.basic=0;
    struct Type_* func_type=(struct Type_*) malloc(sizeof(struct Type_));
    struct table_item* new_func=(struct table_item*) malloc(sizeof(struct table_item));
    new_func->name=strdup("read");
    new_func->type=func_type;
    func_type->Kind=FUNCTION;
    func_type->content.func.State=DEFINED;
    func_type->content.func.param_list=nullptr;
    func_type->content.func.return_type=int_type;
    add_symbol(new_func);
    struct Param* int_param=(struct Param*) malloc(sizeof(struct Param));
    int_param->param_type=int_type;
    int_param->next=nullptr;
    func_type=(struct Type_*) malloc(sizeof(struct Type_));
    new_func=(struct table_item*) malloc(sizeof(struct table_item));
    new_func->name=strdup("write");
    new_func->type=func_type;
    func_type->Kind=FUNCTION;
    func_type->content.func.State=DEFINED;
    func_type->content.func.param_list=int_param;
    func_type->content.func.return_type=int_type;
    add_symbol(new_func);
}
void head_insert_HashTable(int hash_val,struct table_item* item){
    struct table_item* head=Symbol_Hashtable[hash_val];
    struct table_item* temp=head->hash_next;
    head->hash_next=item;
    item->hash_next=temp;
    if(temp!=nullptr){
        temp->hash_prev=item;
    }
    item->hash_prev=head;
}

bool add_symbol(struct table_item* item){
    if(item->type->Kind==STRUCTURE){
        char *struct_name=item->name;
        if(find_symbol(struct_name)!=nullptr){
            return false;
        }
        else{
            head_insert_HashTable(hash_pjw(item->name),item);
            add_to_stack(item);
            return true;
        }
    }
    if(find_in_scope(item->name,item->scope)!=nullptr){
        return false;
    }
    else{
        struct table_item* stru= find_symbol(item->name);
        if(stru&&stru->type->Kind==STRUCTURE){
            return false;
        }else{
            //printf("%s added\n",item->name);
            head_insert_HashTable(hash_pjw(item->name),item);
            add_to_stack(item);
            return true;
        }
    }
}
/*!
 * used when define a varible
 * @param name
 * @param scope
 * @return
 */
struct table_item* find_in_scope(char* name,int scope){
    struct table_item* p = Symbol_Hashtable[hash_pjw(name)]->hash_next;
    while (p) {
        if (strcmp(p->name, name) == 0&&p->scope==scope){
            return p;
        }
        p = p->hash_next;
    }
    return nullptr;
}
/*!
 * head_insert makes the newest defnition
 * @param name
 * @return
 */
struct table_item* find_symbol(char* name){
    //printf("finding: %s \n",name);
    struct table_item* head=Symbol_Hashtable[hash_pjw(name)]->hash_next;
    //printf("in find_symbol: %s\n",name);
    while(head){
        //printf("in table: %s\n",head->name);
        if(strcmp(head->name,name)==0){
            //printf("in table:%s\n",head->name);
            return head;
        }
        head=head->hash_next;
    }
    return nullptr;
}
void add_to_stack(struct table_item *item){
    int scope=item->scope;
    struct table_item *head = SymbolTable_stack[scope];
    //insert to head
    item->stack_next=head->stack_next;
    item->stack_prev=head;
    head->stack_next=item;
}
void symbol_stack_pop(int scope_level){
    //printf("in pop!!\n");
    struct table_item* head = SymbolTable_stack[scope_level];
    //printf("begin delete\n");
    while (head->stack_next!= nullptr) {
        //delete from stack
        struct table_item *p =head->stack_next;
        //printf("%s is being deleted\n",p->name);
        head->stack_next=p->stack_next;
        //delete from hash_table
        struct table_item *m=p->hash_prev;
        struct table_item *n=p->hash_next;
        //printf("%s is being deleted\n",p->name);
        m->hash_next=n;
        if(n!=nullptr){
            n->hash_prev=m;
        }
        //printf("%s is being deleted\n",p->name);
        free(p);
        //printf("%s is being deleted\n",p->name);
    }
}
void delete_symbol(char* name){
    struct table_item *p = Symbol_Hashtable[hash_pjw(name)]->hash_next;
    while (p!= nullptr&&strcmp(p->name,name)!= 0) {
        p = p->hash_next;
    }
    if(p!= nullptr){
        struct table_item* m=p->hash_prev;
        struct table_item* n=p->hash_next;
        struct table_item* x=p->stack_prev;
        struct table_item* y=p->stack_next;
        m->hash_next=n;
        n->hash_prev=m;
        x->stack_next=y;
        y->stack_prev=x;
        free(p);
    }
}
