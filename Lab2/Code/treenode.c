#include "treenode.h"

char NAMELIST[][16] = {
    "INT", "FLOAT", "ID", "SEMI", "COMMA", "ASSIGNOP", "RELOP", "PLUS", "MINUS", "STAR", "DIV", "AND", "OR", "NOT", "DOT",
    "TYPE", "LP", "RP", "LB", "RB", "LC", "RC", "STRUCT", "RETURN", "IF", "ELSE", "WHILE",
    "Program", "ExtDefList", "ExtDef", "ExtDecList", "Specifier", "StructSpecifier", "OptTag", "Tag",
    "VarDec", "FunDec", "VarList", "ParamDec", "CompSt", "StmtList", "Stmt", "DefList", "Def", "DecList", "Dec", "Exp", "Args"
};

int isLexicalToken(TreeNode *p)
{
    if (p->identifier <= T_WHILE)
        return 1;
    else return 0;
}

void print_help_func(TreeNode *p)
{
    if (p->identifier == T_INT)
        printf("%s: %d\n", "INT", atoi(p->info));
    else if (p->identifier == T_FLOAT)
        printf("%s: %f\n", "FLOAT", atof(p->info));
    else if (p->identifier == T_TYPE)
        printf("%s: %s\n", "TYPE", p->info);
    else if (p->identifier == T_ID)
        printf("%s: %s\n", "ID", p->info);
    else if (isLexicalToken(p))
        printf("%s\n", NAMELIST[p->identifier]);
    else
        printf("%s (%d)\n", NAMELIST[p->identifier], p->lineno);    
}

void printTree(TreeNode *p, int tab)
{
    if (p == NULL)
        return;
    for (int i = 0; i < tab; i++)
        printf("  ");
    if (p->identifier == PROGRAM && p->num == 0)
    {
        printf("%s (%d)", NAMELIST[p->identifier], p->lineno);
        return;
    }
    print_help_func(p);
    for (int i = 0; i < p->num; i++)
        printTree(p->children[i], tab + 1);
}

void destroyTree(TreeNode *p)
{
    if (p == NULL)
        return;
    for (int i = 0; i < p->num; i++)
        destroyTree(p->children[i]);
    free(p);
}


TreeNode *createTreeNode(TOKEN_NAME identifier, int lineno, char *info)
{
    TreeNode *p = (TreeNode *) malloc(sizeof(TreeNode));
    if (p == NULL)
    {
        fprintf(stderr, "malloc TreeNode error! \n");
        exit(-1);
    }
    p->num = 0;

    p->identifier = identifier;
    strcpy(p->info, info);
    p->lineno = lineno;
    return p;
}

int insertChild(TreeNode *parent, TreeNode *child)
{
    if (parent->num == MAX_CHILDREN_NUM)
    {
        fprintf(stderr, "Chidren of %s is more than maximum value\n", NAMELIST[parent->identifier]);
        exit(-1);
    }
    parent->children[parent->num++] = child;
    return 0;
}

int insertChildren(int num, TreeNode *parent, ...)
{
    va_list list;
    va_start(list, num);
    for (int i = 0; i < num; i++)
        insertChild(parent, va_arg(list, TreeNode *));
    va_end(list);
    return 0;
}


void semantic_analysis(TreeNode *p)
{
    if (p->identifier == PROGRAM)
        ExtDefList(p->children[0]);
}
