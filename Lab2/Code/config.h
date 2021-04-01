#ifndef __CONFIG_H__
#define __CONFIG_H__

#define MAX_CHILDREN_NUM 10
#define MAX_NAME_LENGTH 128
#define MAX_INFO_LENGTH 128
#define MAX_MSG_LENGTH 256
#define MAX_SYMTABLE_ENTRY 0x3fff

#define ERROR1 1
#define ERROR2 2
#define ERROR3 3
#define ERROR4 4 
#define ERROR5 5
#define ERROR6 6
#define ERROR7 7
#define ERROR8 8
#define ERROR9 9
#define ERROR10 10
#define ERROR11 11
#define ERROR12 12
#define ERROR13 13
#define ERROR14 14
#define ERROR15 15
#define ERROR16 16
#define ERROR17 17

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdarg.h>
#include <string.h>

typedef enum 
{
    B_INT = 0, B_FLOAT = 1
} BASIC_TYPE;

typedef enum 
{
    SYM_VARIABLE = 0, SYM_FUNCTION = 1, SYM_STRUCT
} SYMBOL_TYPE;

typedef enum {
    T_INT, T_FLOAT, T_ID, T_SEMI, T_COMMA, T_ASSIGNOP, T_RELOP, T_PLUS, T_MINUS, T_STAR, T_DIV, T_AND, T_OR, T_NOT, T_DOT,
    T_TYPE, T_LP, T_RP, T_LB, T_RB, T_LC, T_RC, T_STRUCT, T_RETURN, T_IF, T_ELSE, T_WHILE,
    PROGRAM, EXTDEFLIST, EXTDEF, EXTDECLIST, SPECIFIER, STRUCTSPECIFIER, OPTTAG, TAG,
    VARDEC, FUNDEC, VARLIST, PARAMDEC, COMPST, STMTLIST, STMT, DEFLIST, DEF, DECLIST, DEC, EXP, ARGS
} TOKEN_NAME;


#endif 