#include "semantic.h"

void semantic_error(int type, int lineno, char *msg)
{
    fprintf(stderr, "Error type %d at Line %d: %s.\n", type, lineno, msg);
}

void ExtDefList(TreeNode *p)
{
    // ExtDefList : empty
    if (p == NULL)
    {
        return;
    }
    // ExtDefList : ExtDef ExtDefList
    else
    {
        ExtDef(p->children[0]);
        ExtDefList(p->children[1]);
    }
}

void ExtDef(TreeNode *p)
{
    // ExtDef : Specifier SEMI
    if (p->num == 2)
    {
        Type t = Specifier(p->children[0]);
    }
    // ExtDef : Specifier ExtDecList SEMI
    else if (p->children[1]->identifier == EXTDECLIST)
    {
        
        Type t = Specifier(p->children[0]);
        ExtDecList(p->children[1], t);
    }
    // ExtDef : Specifier FunDec CompSt
    else
    {
        Type t = Specifier(p->children[0]);
        FunDec(p->children[1], t);
        CompSt(p->children[2], t);
    }
}

FieldList ExtDecList(TreeNode *p, Type t)
{
    FieldList f = VarDec(p->children[0], t, 0);
    // add to symtable
    if (!insert_variable(f->name, f->type, 1, 0)) // has existed
    {
        char msg[MAX_MSG_LENGTH];
        sprintf(msg, "redefined variable \"%s\"", f->name);
        semantic_error(3, p->lineno, msg);
        return NULL;
    }
    // ExtDecList : VarDec
    if (p->num == 1)
    {
        return f;
    }
    // ExtDecList : VarDec COMMA ExtDecList
    else
    {
        FieldList f2 = ExtDecList(p->children[2], t);
        if (f == NULL)
            f = f2;
        else
            f->tail = f2;
        return f;
    }
}

void FunDec(TreeNode *p, Type t)
{
    // FunDec : ID LP RP
    if (p->num == 3)
    {
        if (!insert_func(p->children[0]->info, t, NULL, 1))
        {
            char msg[MAX_MSG_LENGTH];
            sprintf(msg, "redefined function \"%s\"", p->children[0]->info);
            semantic_error(4, p->lineno, msg);
        }
    }
    // FunDec : ID LP VarList RP
    else
    {
        FieldList f = VarList(p->children[2], t);
        if (!insert_func(p->children[0]->info, t, f, 1))
        {
            char msg[MAX_MSG_LENGTH];
            sprintf(msg, "redefined function \"%s\"", p->children[0]->info);
            semantic_error(4, p->lineno, msg);
        }
    }
}

FieldList VarList(TreeNode *p, Type t)
{
    FieldList f = ParamDec(p->children[0]);
    // VarList : ParamDec
    if (p->num == 1)
    {
        return f;
    }
    // VarList : ParamDec COMMA VarList
    else
    {
        FieldList f2 = VarList(p->children[2], t);
        if (f == NULL)
            f = f2;
        else
        {
            FieldList tmp = f;
            while (tmp->tail)
            {
                tmp = tmp->tail;
            }
            tmp->tail = f2;
        }
        return f;
    }
}

FieldList ParamDec(TreeNode *p)
{
    Type t = Specifier(p->children[0]);
    FieldList f = VarDec(p->children[1], t, 0);
    if (!insert_variable(f->name, f->type, 1, 1)) // has existed
    {
        char msg[MAX_MSG_LENGTH];
        sprintf(msg, "redefined variable \"%s\"", f->name);
        semantic_error(3, p->lineno, msg);
        return NULL;
    }
    return f;
}

Type Specifier(TreeNode *p)
{
    TreeNode *n = p->children[0];
    // Specifier : TYPE
    if (n->identifier == T_TYPE)
    {
        Type t = malloc(sizeof(struct Type_));
        t->kind = BASIC;
        if (strcmp(n->info, "int") == 0)
            t->basic = B_INT;
        else if (strcmp(n->info, "float") == 0)
            t->basic = B_FLOAT;
        return t;
    }
    // Specifier : StructSpecifier
    else if (n->identifier == STRUCTSPECIFIER)
    {
        // do it in StructSpecifier
        Type t = StructSpecifier(n);
        return t;
    }
}

Type StructSpecifier(TreeNode *p)
{
    // StructSpecifier : STRUCT Tag
    if (p->num == 2)
    {
        Symbol sym = search_struct(p->children[1]->children[0]->info);
        if (!sym)
        {
            char msg[MAX_MSG_LENGTH];
            sprintf(msg, "undefined structure \"%s\"", p->children[1]->children[0]->info);
            semantic_error(17, p->lineno, msg);
            return NULL;
        }
        return sym->var.type;
    }
    // StructSpecifier : STRUCT OptTag LC DefList RC
    else
    {
        Type t = malloc(sizeof(struct Type_));
        t->kind = STRUCTURE;
        t->structure.field = DefList(p->children[3], 1);
        if (p->children[1] == NULL) // OptTag : empty
        {
            char name[MAX_NAME_LENGTH];
            sprintf(name, "HiddenStructure-%d", hidden_struct_num);
            hidden_struct_num++;
            insert_struct(name, t, 1);
            strcpy(t->structure.name, name);
        }
        else
        {
            strcpy(t->structure.name, p->children[1]->children[0]->info);
            if (!insert_struct(t->structure.name, t, 1)) // has existed
            {
                char msg[MAX_MSG_LENGTH];
                sprintf(msg, "Duplicated name \"%s\"", p->children[1]->children[0]->info);
                semantic_error(16, p->children[1]->lineno, msg);
                return NULL;
            }
        }
        return t;
    }
}

FieldList DefList(TreeNode *p, int from_struct)
{
    // DefList : Empty
    if (p == NULL)
        return NULL;
    // DefList : Def DefList
    else
    {
        FieldList f1 = Def(p->children[0], from_struct);
        FieldList f2 = DefList(p->children[1], from_struct);
        if (f1 == NULL)
            f1 = f2;
        else
        {
            FieldList tmp = f1;
            while (tmp->tail)
            {
                tmp = tmp->tail;
            }
            tmp->tail = f2;
        }
        return f1;
    }
}

FieldList Def(TreeNode *p, int from_struct)
{
    // Def : Specifier DecList SEMI
    Type t = Specifier(p->children[0]);
    if (!t)
        return NULL;
    FieldList f = DecList(p->children[1], t, from_struct);
    return f;
}

FieldList DecList(TreeNode *p, Type t, int from_struct)
{
    // DecList : Dec
    if (p->num == 1)
    {
        return Dec(p->children[0], t, from_struct);
    }
    // DecList : Dec COMMA DecList
    else
    {
        FieldList f1 = Dec(p->children[0], t, from_struct);
        FieldList f2 = DecList(p->children[2], t, from_struct);
        if (f1 == NULL)
            f1 = f2;
        else
        {
            FieldList tmp = f1;
            while (tmp->tail)
            {
                tmp = tmp->tail;
            }
            tmp->tail = f2;
        }
        return f1;
    }
}

FieldList Dec(TreeNode *p, Type t, int from_struct)
{
    FieldList f = VarDec(p->children[0], t, from_struct);
    // add to symtable: only 1 item
    int ret = insert_variable(f->name, f->type, 1, 0);
    if (!ret) // has existed
    {
        char msg[MAX_MSG_LENGTH];
        if (from_struct) // domain name repeat
        {
            sprintf(msg, "redefined field \"%s\"", f->name);
            semantic_error(15, p->lineno, msg);
            return NULL;
        }
        else // variable name repeat
        {
            sprintf(msg, "redefined variable \"%s\"", f->name);
            semantic_error(3, p->lineno, msg);
            return NULL;
        }
    }
    // Dec : VarDec
    if (p->num == 1)
    {
        return f;
    }
    // Dec : VarDec ASSIGNOP Exp
    else
    {
        if (from_struct)
        {
            semantic_error(15, p->lineno, "assign a variable when defining a struct");
            return NULL;
        }
        Type t_right = Exp(p->children[2]);
        if (t_right == NULL)
            return f; // NOTE: a controversial point
        if (!type_equal(f->type, t_right))
        {
            char msg[MAX_MSG_LENGTH];
            sprintf(msg, "type mismatched for assignment");
            semantic_error(5, p->lineno, msg);
            return NULL;
        }
        return f;
    }
}

FieldList VarDec(TreeNode *p, Type t, int from_struct)
{
    // VarDec : ID
    if (p->num == 1)
    {
        FieldList f = malloc(sizeof(struct FieldList_));
        f->tail = NULL;
        strcpy(f->name, p->children[0]->info);
        f->type = (t);
        return f;
    }
    // VarDec : VarDec LB INT RB
    else
    {
        FieldList f = VarDec(p->children[0], t, from_struct);
        f->tail = NULL;
        
        Type last = f->type;
        
        if (last->kind != ARRAY)
        {
            Type cur = malloc(sizeof(struct Type_));
            cur->kind = ARRAY;
            cur->array.size = atoi(p->children[2]->info);
            cur->array.elem = last;
            f->type = cur;
        }
        else
        {
            while (last->array.elem->kind == ARRAY)
                last = last->array.elem;
            Type cur = malloc(sizeof(struct Type_));
            cur->kind = ARRAY;
            cur->array.size = atoi(p->children[2]->info);
            cur->array.elem = last->array.elem;
            last->array.elem = cur;
        }
        return f;
    }
}


void CompSt(TreeNode *p, Type return_type)
{
    // CompSt : LC DefList StmtList RC
    DefList(p->children[1], 0);
    StmtList(p->children[2], return_type);
}

void StmtList(TreeNode *p, Type return_type)
{
    // StmtList : empty
    if (p == NULL)
        return;
    // StmtList : Stmt StmtList
    else
    {
        Stmt(p->children[0], return_type);
        StmtList(p->children[1], return_type);
    }
}

void Stmt(TreeNode *p, Type return_type)
{
    // Stmt : CompSt
    if (p->num == 1)
    {
        CompSt(p->children[0], return_type);
    }
    // Stmt : Exp SEMI
    else if (p->num == 2)
    {
        Exp(p->children[0]);
    }
    // Stmt : RETURN Exp SEMI
    else if (p->num == 3)
    {
        Type real_type = Exp(p->children[1]);
        if (real_type == NULL)
            return;
        if (!type_equal(real_type, return_type))
        {
            char msg[MAX_MSG_LENGTH];
            sprintf(msg, "type mismatched for return-statement");
            semantic_error(8, p->lineno, msg);
        }
    }
    // Stmt : WHILE LP Exp RP Stmt
    else if (p->children[0]->identifier == T_WHILE)
    {
        Type t = Exp(p->children[2]);
        if (t != NULL)
        {
            if (!((t->kind == BASIC) && (t->basic == B_INT)))
            {
                char msg[MAX_MSG_LENGTH];
                sprintf(msg, "Expression in while-statement should be INT type");
                // semantic_error(7, p->lineno, msg);
            }
        }
        Stmt(p->children[4], return_type);
    }
    // Stmt : IF LP Exp RP Stmt
    else if (p->num == 5)
    {
        Type t = Exp(p->children[2]);
        if (t != NULL)
        {
            if (!((t->kind == BASIC) && (t->basic == B_INT)))
            {
                char msg[MAX_MSG_LENGTH];
                sprintf(msg, "Expression in if-statement should be INT type");
                // semantic_error(7, p->lineno, msg);
            }
        }
        Stmt(p->children[4], return_type);
    }
    // Stmt : IF LP Exp RP Stmt ELSE Stmt
    else
    {
        Type t = Exp(p->children[2]);
        if (t != NULL)
        {
            if (!((t->kind == BASIC) && (t->basic == B_INT)))
            {
                char msg[MAX_MSG_LENGTH];
                sprintf(msg, "Expression in if-statement should be INT type");
                // semantic_error(7, p->lineno, msg);
            }
        }
        Stmt(p->children[4], return_type);
        Stmt(p->children[6], return_type);
    }
}

int is_basic_type(Type t)
{
    if (t->kind == BASIC)
        return 1;
    else return 0;
}

int logical_operator_detect(Type t1, Type t2, int lineno)
{
    if (!(is_basic_type(t1) && is_basic_type(t2)))
    {
        char msg[MAX_MSG_LENGTH];
        sprintf(msg, "type mismatched for operands");
        semantic_error(7, lineno, msg);
        return 0;
    }
    if (!(type_equal(t1, t2)) && t1->basic == B_INT)
    {
        char msg[MAX_MSG_LENGTH];
        sprintf(msg, "type mismatched for operands");
        semantic_error(7, lineno, msg);
        return 0;
    }
    return 1;
}

int numerical_operator_detect(Type t1, Type t2, int lineno)
{
    if (!(is_basic_type(t1) && is_basic_type(t2)))
    {
        char msg[MAX_MSG_LENGTH];
        sprintf(msg, "type mismatched for operands");
        semantic_error(7, lineno, msg);
        return 0;
    }
    if (!type_equal(t1, t2))
    {
        char msg[MAX_MSG_LENGTH];
        sprintf(msg, "type mismatched for operands");
        semantic_error(7, lineno, msg);
        return 0;
    }
    return 1;
}

Type Exp(TreeNode *p)
{
    // Exp : ID
    if (p->num == 1 && p->children[0]->identifier == T_ID)
    {
        Symbol sym = search_variable(p->children[0]->info);
        if (sym == NULL)
        {
            char msg[MAX_MSG_LENGTH];
            sprintf(msg, "undefined variable \"%s\"", p->children[0]->info);
            semantic_error(1, p->lineno, msg);
            return NULL;
        }
        return sym->var.type;
    }
    // Exp : INT
    else if (p->children[0]->identifier == T_INT)
    {
        Type t = malloc(sizeof(struct Type_));
        t->kind = BASIC;
        t->basic = B_INT;
        return t;
    }
    // Exp : FLOAT
    else if (p->children[0]->identifier == T_FLOAT)
    {
        Type t = malloc(sizeof(struct Type_));
        t->kind = BASIC;
        t->basic = B_FLOAT;
        return t;
    }
    // Exp : MINUS Exp
    else if (p->children[0]->identifier == T_MINUS)
    {
        return Exp(p->children[1]);
    }
    // Exp : NOT Exp
    else if (p->children[0]->identifier == T_NOT)
    {
        return Exp(p->children[1]);
    }
    // Exp : LP Exp RP
    else if (p->children[0]->identifier == T_LP)
    {
        return Exp(p->children[1]);
    }
    // Exp : ID LP RP
    else if (p->children[1]->identifier == T_LP && p->children[2]->identifier == T_RP)
    {
        Symbol sym = search_func(p->children[0]->info);
        if (!search_variable(p->children[0]->info) && !sym)
        {
            char msg[MAX_MSG_LENGTH];
            sprintf(msg, "undefined function \"%s\"", p->children[0]->info);
            semantic_error(2, p->lineno, msg);
            return NULL;
        }
        else if (!sym)
        {
            char msg[MAX_MSG_LENGTH];
            sprintf(msg, "variable %s can't be called", p->children[0]->info);
            semantic_error(11, p->lineno, msg);
            return NULL;
        }
        if (!func_args_equal(sym->func.param, NULL))
        {
            char msg[MAX_MSG_LENGTH];
            sprintf(msg, "function \"%s\" is not applicable for given arguments", p->children[0]->info);
            semantic_error(9, p->lineno, msg);
            return NULL;
        }
        return sym->func.return_type;
    }
    // Exp : Exp ASSIGNOP Exp
    else if (p->children[1]->identifier == T_ASSIGNOP)
    {
        TreeNode *left = p->children[0];
        if (!((left->num == 1 && left->children[0]->identifier == T_ID) || 
        (left->num == 3 && left->children[1]->identifier == T_DOT) || 
        left->num == 4 && left->children[1]->identifier == T_LB))
        {
            char msg[MAX_MSG_LENGTH];
            sprintf(msg, "the left-hand side of an assignment must be a variable");
            semantic_error(6, p->lineno, msg);
            return NULL;
        }

        Type t1 = Exp(p->children[0]);
        if (!t1) return NULL;
        Type t2 = Exp(p->children[2]);
        if (!t2) return NULL;

        if (!type_equal(t1, t2))
        {
            char msg[MAX_MSG_LENGTH];
            sprintf(msg, "unmatched assignment type");
            semantic_error(5, p->lineno, msg);
            return NULL;
        }
        return t1;
    }
    // Exp : Exp AND Exp
    else if (p->children[1]->identifier == T_AND)
    {
        Type t1 = Exp(p->children[0]);
        if (!t1) return NULL;
        Type t2 = Exp(p->children[2]);
        if (!t2) return NULL;
        
        if (!logical_operator_detect(t1, t2, p->lineno))
            return NULL;
        return t1;
    }
    // Exp : Exp OR Exp
    else if (p->children[1]->identifier == T_OR)
    {
        Type t1 = Exp(p->children[0]);
        if (!t1) return NULL;
        Type t2 = Exp(p->children[2]);
        if (!t2) return NULL;

        if (!logical_operator_detect(t1, t2, p->lineno))
            return NULL;
        return t1;
    }
    // Exp : Exp RELOP Exp
    else if (p->children[1]->identifier == T_RELOP)
    {
        Type t1 = Exp(p->children[0]);
        if (!t1) return NULL;
        Type t2 = Exp(p->children[2]);
        if (!t2) return NULL;

        if (!logical_operator_detect(t1, t2, p->lineno))
            return NULL;
        return t1;
    }
    // Exp : Exp PLUS Exp
    else if (p->children[1]->identifier == T_PLUS)
    {
        Type t1 = Exp(p->children[0]);
        if (!t1) return NULL;
        Type t2 = Exp(p->children[2]);
        if (!t2) return NULL;

        if (!numerical_operator_detect(t1, t2, p->lineno))
            return NULL;
        return t1;
    }
    // Exp : Exp MINUS Exp
    else if (p->children[1]->identifier == T_MINUS)
    {
        Type t1 = Exp(p->children[0]);
        if (!t1) return NULL;
        Type t2 = Exp(p->children[2]);
        if (!t2) return NULL;

        if (!numerical_operator_detect(t1, t2, p->lineno))
            return NULL;
        return t1;
    }
    // Exp : Exp STAR Exp
    else if (p->children[1]->identifier == T_STAR)
    {
        Type t1 = Exp(p->children[0]);
        if (!t1) return NULL;
        Type t2 = Exp(p->children[2]);
        if (!t2) return NULL;

        if (!numerical_operator_detect(t1, t2, p->lineno))
            return NULL;
        return t1;
    }
    // Exp : Exp DIV Exp
    else if (p->children[1]->identifier == T_DIV)
    {
        Type t1 = Exp(p->children[0]);
        if (!t1) return NULL;
        Type t2 = Exp(p->children[2]);
        if (!t2) return NULL;

        if (!numerical_operator_detect(t1, t2, p->lineno))
            return NULL;
        return t1;
    }
    // Exp : Exp DOT ID
    else if (p->children[1]->identifier == T_DOT)
    {
        Type t = Exp(p->children[0]);
        if (t == NULL)
            return NULL;
        if (t->kind != STRUCTURE)
        {
            char msg[MAX_MSG_LENGTH];
            sprintf(msg, "visit member of non-struct variable");
            semantic_error(13, p->lineno, msg);
            return NULL;
        }
        Type ret = find_fieldlist(t->structure.field, p->children[2]->info);
        if (ret == NULL)
        {
            char msg[MAX_MSG_LENGTH];
            sprintf(msg, "Non-existent field \"%s\"", p->children[2]->info);
            semantic_error(14, p->lineno, msg);
            return NULL;
        }
        return ret;
    }
    // Exp : Exp LB Exp RB
    else if (p->children[1]->identifier == T_LB)
    {
        Type t = Exp(p->children[0]);
        if (t == NULL)
            return NULL;
        if (t->kind != ARRAY)
        {
            char msg[MAX_MSG_LENGTH];
            sprintf(msg, "visit element of non-array variable");
            semantic_error(10, p->lineno, msg);
            return NULL;
        }
        Type t_idx = Exp(p->children[2]);
        if (t_idx == NULL)
            return NULL;
        if (!(t_idx->kind == BASIC && t_idx->basic == B_INT))
        {
            char msg[MAX_MSG_LENGTH];
            sprintf(msg, "index of array is not integer");
            semantic_error(12, p->lineno, msg);
            return NULL;
        }
        return t->array.elem;
    }
    // Exp : ID LP Args RP
    else
    {
        Symbol sym = search_func(p->children[0]->info);
        if (!search_variable(p->children[0]->info) && !sym)
        {
            char msg[MAX_MSG_LENGTH];
            sprintf(msg, "undefined function \"%s\"", p->children[0]->info);
            semantic_error(2, p->lineno, msg);
            return NULL;
        }
        else if (!sym)
        {
            char msg[MAX_MSG_LENGTH];
            sprintf(msg, "\"%s\" is not a function", p->children[0]->info);
            semantic_error(11, p->lineno, msg);
            return NULL;
        }
        FieldList f = Args(p->children[2]);
        if (f == NULL)
            return NULL;
        if (!func_args_equal(sym->func.param, f))
        {
            char msg[MAX_MSG_LENGTH];
            sprintf(msg, "parameters of funcion %s not matched", p->children[0]->info);
            semantic_error(9, p->lineno, msg);
            return NULL;
        }
        return sym->func.return_type;
    }
}

FieldList Args(TreeNode *p)
{
    // Args : Exp
    if (p->num == 1)
    {
        Type t = Exp(p->children[0]);
        if (t == NULL)
            return NULL;
        FieldList f = malloc(sizeof(struct FieldList_));
        f->tail = NULL;
        f->type = t;
        return f;
    }
    // Args : Exp COMMA Args
    else
    {
        Type t = Exp(p->children[0]);
        if (t == NULL)
            return NULL;
        FieldList f = malloc(sizeof(struct FieldList_));
        f->tail = NULL;
        f->type = t;
        f->tail = Args(p->children[2]);
        return f;
    }
}