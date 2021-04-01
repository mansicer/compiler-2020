#ifndef __SYMTABLE_H__
#define __SYMTABLE_H__

#include "config.h"


typedef struct Type_ * Type;
typedef struct FieldList_ * FieldList;

struct Type_
{
    enum { BASIC, ARRAY, STRUCTURE } kind;
    union
    {
        BASIC_TYPE basic;
        struct { Type elem; int size; } array;
        struct { char name[MAX_NAME_LENGTH]; FieldList field; } structure;
    };
};

struct FieldList_
{
    char name[MAX_NAME_LENGTH];
    Type type;
    FieldList tail;
};

typedef struct Symbol_ * Symbol;
struct Symbol_
{
    SYMBOL_TYPE symtype;
    char name[MAX_NAME_LENGTH];
    union {
        // variable
        struct {
            Type type;
        } var;
        // function
        struct {
            Type return_type;
            FieldList param;
        } func;
        // struct
        struct {
            Type type;
        } structure;
    } ;
    int defined;
};

typedef struct SymList_ * SymList;
struct SymList_
{
    struct Symbol_ sym;
    SymList next;
};

SymList symtable[MAX_SYMTABLE_ENTRY];

extern int hidden_struct_num;

int insert_variable(char *name, Type type, int defined);
int insert_struct(char *name, Type type, int defined);
int insert_func(char *name, Type return_type, FieldList param, int defined);
Symbol search_variable(char *name);
Symbol search_struct(char *name);
Symbol search_func(char *name);
int type_equal(Type t1, Type t2);
int fieldlist_equal(FieldList f1, FieldList t2);
int func_args_equal(FieldList param, FieldList args);
Type copy_type(Type src);
FieldList copy_fieldlist(FieldList src);
Type find_fieldlist(FieldList f, char *name);
void print_symtable();
void print_type(Type t);
void print_type_help(Type t, int tab);
void print_fieldlist(FieldList f, int tab);

#endif