#ifndef __SEMANTIC_H__
#define __SEMANTIC_H__

#include "treenode.h"

void ExtDefList(TreeNode *p);
void ExtDef(TreeNode *p);
FieldList ExtDecList(TreeNode *p, Type t);
void FunDec(TreeNode *p, Type t);
FieldList VarList(TreeNode *p, Type t);
FieldList ParamDec(TreeNode *p);
Type Specifier(TreeNode *p);
Type StructSpecifier(TreeNode *p);
FieldList DefList(TreeNode *p, int from_struct);
FieldList Def(TreeNode *p, int from_struct);
FieldList DecList(TreeNode *p, Type t, int from_struct);
FieldList Dec(TreeNode *p, Type t, int from_struct);
FieldList VarDec(TreeNode *p, Type t, int from_struct);
void CompSt(TreeNode *p, Type return_type);
void StmtList(TreeNode *p, Type return_type);
void Stmt(TreeNode *p, Type return_type);
Type Exp(TreeNode *p);
FieldList Args(TreeNode *p);

#endif