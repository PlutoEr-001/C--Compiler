//
// Created by plutoer on 22-10-26.
//
#include "error.h"
const char *error_format[] = {
        "Undefined variable \"%s\".",
        "Undefined function \"%s\".",
        "Redefined variable \"%s\".",
        "Redefined function \"%s\".",
        "Type mismatched for assignment.",
        "The left-hand side of an assignment must be a variable.",
        "Type mismatched for operands.",
        "Type mismatched for return.",
        "Function \"%s(%s)\" is not applicable for arguments \"(%s)\".",
        "Not an array.",
        "\"%s\" is not a function.",
        "Array index is not an integer.",
        "Illegal use of \".\".",
        "Non-existent field \"%s\".",
        "Redefined field Or Initialized in structure.",
        "Duplicated name \"%s\".",
        "Undefined structure \"%s\".",
        "Undefined function \"%s\".",
        "Inconsistent declaration of function \"%s\"."
};
void print_error(int error_type, struct Node* node, ...) {
    no_error=0;
    va_list ap;
    va_start(ap, node);
    printf("Error type %d at Line %d: ", error_type+1, node->lineno);
    vprintf(error_format[error_type], ap);
    printf("\n");
}