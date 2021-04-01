#ifndef __TREENODE_H__
#define __TREENODE_H__

#include "symtable.h"

typedef struct node
{
    struct node *children[MAX_CHILDREN_NUM];
    TOKEN_NAME identifier;
    int lineno;
    int num;  // number of children
    // optional info
    char info[MAX_NAME_LENGTH];
} TreeNode;

extern char NAMELIST[][16];

#include "semantic.h"
#include "immediate.h"

int isLexicalToken(TreeNode *p);
TreeNode *createTreeNode(TOKEN_NAME identifier, int lineno, char *info);
int insertChild(TreeNode *parent, TreeNode *child);
int insertChildren(int num, TreeNode *parent, ...);
void printTree(TreeNode *p, int tab);
void destroyTree(TreeNode *p);
void semantic_analysis(TreeNode *p);
void generate_immediate(TreeNode *p, FILE *fp);

#endif