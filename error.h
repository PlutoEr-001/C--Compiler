//
// Created by plutoer on 22-10-20.
//

#ifndef LAB_ERROR_H
#define LAB_ERROR_H
#include "ast.h"
extern int no_error;
void print_error(int error_type, struct Node* node, ...);
#endif //LAB_ERROR_H
