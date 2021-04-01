#ifndef __IMMEDIATE_H__
#define __IMMEDIATE_H__

#include "config.h"

typedef struct {
    enum { O_TEMP, O_VARIABLE, O_CONSTANT } type;
    enum { O_VAL, O_LEFTVAL, O_RIGHTVAL } kind;
    union {
        int temp_no;
        int var_no;
        int const_value;
    } ;
} Operand;

typedef enum {
    I_LABEL, I_FUNC, I_ASSIGN, I_ADD, I_SUB, I_MULT, I_DIV,
    I_GOTO, I_COND, I_RETURN, I_DEC, I_ARG, I_CALL, I_PARAM, I_READ, I_WRITE,
    I_END_OF_FUNC
} InterCodeType;

typedef struct {
    InterCodeType type;
    union {
        int label_no;
        char func_name[MAX_NAME_LENGTH];
        struct {
            Operand left;
            Operand right;
        } assign;
        struct {
            Operand left;
            Operand right;
            Operand result;
        } add;
        struct {
            Operand left;
            Operand right;
            Operand result;
        } sub;
        struct {
            Operand left;
            Operand right;
            Operand result;
        } mult;
        struct {
            Operand left;
            Operand right;
            Operand result;
        } div;
        int goto_no;
        struct {
            Operand left;
            Operand right;
            enum { RE_EQ, RE_NE, RE_L, RE_G, RE_LE, RE_GE } relop;
            int goto_no;
        } cond;
        Operand ret;
        struct {
            Operand decvar;
            int size;
        } dec;
        Operand arg;
        struct {
            Operand ret;
            char func_name[MAX_NAME_LENGTH];
        } call;
        Operand param;
        Operand read;
        Operand write;
    };
} InterCode;

typedef struct InterCodeNode_ * InterCodeNode;
struct InterCodeNode_ {
    InterCode code;
    struct InterCodeNode_ *next;
    struct InterCodeNode_ *prev;
};

#include "treenode.h"

extern InterCodeNode icodelist;
extern int inter_no_error_flag;

extern int temporary_no;
extern int variable_no;

void insert_intercode(InterCode *p);
void optimize_intercode();
void print_intercode(FILE *fp);

void prepare_ir();

void translate_ExtDefList(TreeNode *p);
void translate_ExtDef(TreeNode *p);
int translate_FunDec(TreeNode *p);
void translate_CompSt(TreeNode *p);
void translate_DefList(TreeNode *p);
void translate_Def(TreeNode *p);
void translate_Specifier(TreeNode *p);
void translate_DecList(TreeNode *p);
void translate_Dec(TreeNode *p);
int translate_VarDec(TreeNode *p);
void translate_StmtList(TreeNode *p);
void translate_Stmt(TreeNode *p);
void translate_Exp(TreeNode *p, int temp_no);
int translate_array(TreeNode *p, int temp_no);
void translate_Cond(TreeNode *p, int true_label, int false_label);
void translate_Args(TreeNode *p);

#endif