#include "treenode.h"

void print_help_func(TreeNode *p)
{
    if (strcmp(p->name, "INT") == 0)
        printf("%s: %d\n", p->name, atoi(p->info));
    else if (strcmp(p->name, "FLOAT") == 0)
        printf("%s: %f\n", p->name, atof(p->info));
    else if (strcmp(p->name, "TYPE") == 0)
        printf("%s: %s\n", p->name, p->info);
    else if (strcmp(p->name, "ID") == 0)
        printf("%s: %s\n", p->name, p->info);
    else if (strcmp(p->name, "SEMI") == 0)
        printf("%s\n", p->name);
    else if (strcmp(p->name, "COMMA") == 0)
        printf("%s\n", p->name);
    else if (strcmp(p->name, "ASSIGNOP") == 0)
        printf("%s\n", p->name);
    else if (strcmp(p->name, "RELOP") == 0)
        printf("%s\n", p->name);
    else if (strcmp(p->name, "PLUS") == 0)
        printf("%s\n", p->name);
    else if (strcmp(p->name, "MINUS") == 0)
        printf("%s\n", p->name);
    else if (strcmp(p->name, "STAR") == 0)
        printf("%s\n", p->name);
    else if (strcmp(p->name, "DIV") == 0)
        printf("%s\n", p->name);
    else if (strcmp(p->name, "AND") == 0)
        printf("%s\n", p->name);
    else if (strcmp(p->name, "OR") == 0)
        printf("%s\n", p->name);
    else if (strcmp(p->name, "DOT") == 0)
        printf("%s\n", p->name);
    else if (strcmp(p->name, "NOT") == 0)
        printf("%s\n", p->name);
    else if (strcmp(p->name, "LP") == 0)
        printf("%s\n", p->name);
    else if (strcmp(p->name, "RP") == 0)
        printf("%s\n", p->name);
    else if (strcmp(p->name, "LB") == 0)
        printf("%s\n", p->name);
    else if (strcmp(p->name, "RB") == 0)
        printf("%s\n", p->name);
    else if (strcmp(p->name, "LC") == 0)
        printf("%s\n", p->name);
    else if (strcmp(p->name, "RC") == 0)
        printf("%s\n", p->name);
    else if (strcmp(p->name, "STRUCT") == 0)
        printf("%s\n", p->name);
    else if (strcmp(p->name, "RETURN") == 0)
        printf("%s\n", p->name);
    else if (strcmp(p->name, "IF") == 0)
        printf("%s\n", p->name);
    else if (strcmp(p->name, "ELSE") == 0)
        printf("%s\n", p->name);
    else if (strcmp(p->name, "WHILE") == 0)
        printf("%s\n", p->name);
    else
        printf("%s (%d)\n", p->name, p->lineno);
}

TreeNode *createTreeNode(char *name, int lineno, char *info)
{
    TreeNode *p = (TreeNode *) malloc(sizeof(TreeNode));
    p->num = 0;

    strcpy(p->name, name);
    strcpy(p->info, info);
    p->lineno = lineno;
}

int insertChild(TreeNode *parent, TreeNode *child)
{
    if (parent->num == MAX_CHILDREN_NUM)
        return 1;
    if (child == NULL)
        return 1;
    parent->children[parent->num++] = child;
    return 0;
}

int insertChildren(int num, TreeNode *parent, ...)
{
    va_list list;
    va_start(list, num);
    for (int i = 0; i < num; i++)
        if (insertChild(parent, va_arg(list, TreeNode *)))
            return 1;
    va_end(list);
    return 0;
}

void printTree(TreeNode *p, int tab)
{
    if (p == NULL)
        return;
    for (int i = 0; i < tab; i++)
        printf("  ");
    if (strcmp(p->name, "Program") == 0 && p->num == 0)
    {
        printf("%s (%d)", p->name, p->lineno);
        return;
    }
    print_help_func(p);
    if (p->num > 0)
    {
        for (int i = 0; i < p->num; i++)
            printTree(p->children[i], tab + 1);
    }
}

void destroyTree(TreeNode *p)
{
    if (p == NULL)
        return;
    for (int i = 0; i < p->num; i++)
        destroyTree(p->children[i]);
    free(p);
}
