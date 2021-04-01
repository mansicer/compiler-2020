#ifndef __TREENODE_H__
#define __TREENODE_H__

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdarg.h>
#include <string.h>

#define MAX_CHILDREN_NUM 10

typedef struct node
{
    struct node *children[MAX_CHILDREN_NUM];
    char name[32];
    int lineno;
    int num;  // number of children
    // optional info
    char info[50];
} TreeNode;


TreeNode *createTreeNode(char *name, int lineno, char *info);
int insertChild(TreeNode *parent, TreeNode *child);
int insertChildren(int num, TreeNode *parent, ...);
void printTree(TreeNode *p, int tab);
void destroyTree(TreeNode *p);

#endif