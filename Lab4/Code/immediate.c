#include "immediate.h"

InterCodeNode icodelist = NULL;
InterCodeNode current_iterator = NULL;
int temporary_no = 1;
int label_no = 1;
int variable_no = 1;
int inter_no_error_flag = 1;

int new_temp()
{
    return temporary_no++;
}

int new_variable()
{
    return variable_no++;
}

int new_label()
{
    return label_no++;
}

void help_insert_temp(Operand *o, int no)
{
    o->type = O_TEMP;
    o->kind = O_VAL;
    o->temp_no = no;
}

void help_insert_variable(Operand *o, int no)
{
    o->type = O_VARIABLE;
    o->kind = O_VAL;
    o->var_no = no;
}

void help_insert_constant(Operand *o, int val)
{
    o->type = O_CONSTANT;
    o->kind = O_VAL;
    o->const_value = val;
}

void prepare_ir()
{
    // add read to symtable
    Type t = malloc(sizeof(struct Type_));
    t->kind = BASIC;
    t->basic = B_INT;
    insert_func("read", t, NULL, 1);
    // add write to symtable
    FieldList f = malloc(sizeof(struct FieldList_));
    f->tail = NULL;
    f->type = t;
    insert_func("write", t, f, 1);

    free(t);
    free(f);
}

void insert_intercode(InterCode *p)
{
    InterCodeNode node = malloc(sizeof(struct InterCodeNode_));
    node->next = NULL;
    node->prev = NULL;
    
    // copy intercode
    memcpy(&node->code, p, sizeof(InterCode));

    if (icodelist == NULL)
    {
        node->next = node;
        node->prev = node;
        icodelist = node;
        current_iterator = node;
    }
    else
    {
        current_iterator->next = node;
        node->prev = current_iterator;
        node->next = icodelist;
        icodelist->prev = node;
        current_iterator = node;
    }
}

void insert_intercode_from_node(InterCodeNode n, InterCode *p)
{
    InterCodeNode node = malloc(sizeof(struct InterCodeNode_));
    node->next = NULL;
    node->prev = NULL;
    
    // copy intercode
    memcpy(&node->code, p, sizeof(InterCode));

    if (icodelist == n)
    {
        node->next = n;
        node->prev = n->prev;
        n->prev = node;
        node->prev->next = node;
        icodelist = node;
    }
    else
    {
        node->next = n;
        node->prev = n->prev;
        n->prev = node;
        node->prev->next = n;
    }
}

void delete_intercode(InterCodeNode n)
{
    InterCodeNode prev = n->prev;
    InterCodeNode next = n->next;
    // only 1 node
    if (prev == n)
    {
        free(n);
        icodelist = NULL;
    }
    else
    {
        prev->next = next;
        next->prev = prev;
        if (icodelist == n)
            icodelist = next;
        free(n);
    }
}

void check_operand(int tno, Operand *o1, Operand *o2)
{
    if (o1->type == O_TEMP && o1->temp_no == tno)
    {
        if (o1->kind == O_VAL)
            memcpy(o1, o2, sizeof(Operand));
        else if (o1->kind == O_LEFTVAL)
        {
            if (o2->kind == O_VAL)
            {
                memcpy(o1, o2, sizeof(Operand));
                o1->kind = O_LEFTVAL;   
            }
            else
            {
                // do not exist
                // fprintf(stderr, "failed\n");
            }
        }
        else if (o1->kind == O_RIGHTVAL)
        {
            if (o2->kind == O_VAL)
            {
                memcpy(o1, o2, sizeof(Operand));
                o1->kind = O_RIGHTVAL;   
            }
            else
            {
                // do not exist
                // fprintf(stderr, "failed\n");
            }
        }
    }
}

void replace_temp(InterCodeNode p, int tno, Operand *o)
{
    p = p->next;
    do
    {
        InterCode *c = &p->code;
        if (c->type == I_ASSIGN)
        {
            check_operand(tno, &c->assign.left, o);
            check_operand(tno, &c->assign.right, o);
        }
        else if (c->type == I_ADD)
        {
            check_operand(tno, &c->add.result, o);
            check_operand(tno, &c->add.left, o);
            check_operand(tno, &c->add.right, o);
        }
        else if (c->type == I_SUB)
        {
            check_operand(tno, &c->sub.result, o);
            check_operand(tno, &c->sub.left, o);
            check_operand(tno, &c->sub.right, o);
        }
        else if (c->type == I_MULT)
        {
            check_operand(tno, &c->mult.result, o);
            check_operand(tno, &c->mult.left, o);
            check_operand(tno, &c->mult.right, o);
        }
        else if (c->type == I_DIV)
        {
            check_operand(tno, &c->div.result, o);
            check_operand(tno, &c->div.left, o);
            check_operand(tno, &c->div.right, o);
        }
        else if (c->type == I_COND)
        {
            check_operand(tno, &c->cond.left, o);
            check_operand(tno, &c->cond.right, o);
        }
        else if (c->type == I_RETURN)
        {
            check_operand(tno, &c->ret, o);
        }
        else if (c->type == I_ARG)
        {
            check_operand(tno, &c->arg, o);
        }
        else if (c->type == I_CALL)
        {
            check_operand(tno, &c->call.ret, o);
        }
        else if (c->type == I_PARAM)
        {
            check_operand(tno, &c->param, o);
        }
        else if (c->type == I_READ)
        {
            check_operand(tno, &c->read, o);
        }
        else if (c->type == I_WRITE)
        {
            check_operand(tno, &c->write, o);
        }
        p = p->next;
    } while (p != icodelist);
}

int reverse_cond(int cond)
{
    if (cond == RE_EQ)
        return RE_NE;
    else if (cond == RE_NE)
        return RE_EQ;
    else if (cond == RE_G)
        return RE_LE;
    else if (cond == RE_L)
        return RE_GE;
    else if (cond == RE_LE)
        return RE_G;
    else if (cond == RE_GE)
        return RE_L;
    return -1;
}

int exist_label(InterCodeNode p1, InterCodeNode p2, int label_no)
{
    while (p1 != icodelist)
    {
        p1 = p1->prev;
        InterCode *c = &p1->code;
        if (c->type == I_GOTO && c->goto_no == label_no)
            return 1;
        else if (c->type == I_COND && c->cond.goto_no == label_no)
            return 1;
    }
    while (p2 != icodelist->prev)
    {
        p2 = p2->next;
        InterCode *c = &p2->code;
        if (c->type == I_GOTO && c->goto_no == label_no)
            return 1;
        else if (c->type == I_COND && c->cond.goto_no == label_no)
            return 1;
    }
    return 0;
}

int check_temp_can_replace(InterCodeNode n, int temp_no)
{
    InterCodeNode p = n;
    while (p != icodelist)
    {
        p = p->prev;
        InterCode *c = &p->code;
        if (c->type == I_ASSIGN && c->assign.left.type == O_TEMP && c->assign.left.temp_no == temp_no)
            return 0;
        if (c->type == I_ADD && c->add.result.type == O_TEMP && c->add.result.temp_no == temp_no)
            return 0;
        if (c->type == I_SUB && c->sub.result.type == O_TEMP && c->sub.result.temp_no == temp_no)
            return 0;
        if (c->type == I_MULT && c->mult.result.type == O_TEMP && c->mult.result.temp_no == temp_no)
            return 0;
        if (c->type == I_DIV && c->div.result.type == O_TEMP && c->div.result.temp_no == temp_no)
            return 0;
        if (c->type == I_CALL && c->call.ret.type == O_TEMP && c->call.ret.temp_no == temp_no)
            return 0;
    }
    p = n;
    while (p != icodelist->prev)
    {
        p = p->next;
        InterCode *c = &p->code;
        if (c->type == I_ASSIGN && c->assign.left.type == O_TEMP && c->assign.left.temp_no == temp_no)
            return 0;
        if (c->type == I_ADD && c->add.result.type == O_TEMP && c->add.result.temp_no == temp_no)
            return 0;
        if (c->type == I_SUB && c->sub.result.type == O_TEMP && c->sub.result.temp_no == temp_no)
            return 0;
        if (c->type == I_MULT && c->mult.result.type == O_TEMP && c->mult.result.temp_no == temp_no)
            return 0;
        if (c->type == I_DIV && c->div.result.type == O_TEMP && c->div.result.temp_no == temp_no)
            return 0;
        if (c->type == I_CALL && c->call.ret.type == O_TEMP && c->call.ret.temp_no == temp_no)
            return 0;
    }
    return 1;
}

void optimize_intercode()
{
    InterCodeNode p = icodelist;
    do
    {
        InterCode *c = &p->code;
        if (c->type == I_ASSIGN && c->assign.left.type == O_TEMP && c->assign.left.kind == O_VAL)
        {
            if (check_temp_can_replace(p, c->assign.left.temp_no))
            {
                Operand *o = &c->assign.right;
                replace_temp(p, c->assign.left.temp_no, o);
                delete_intercode(p);
            }
        }

        InterCodeNode p2 = p->next;
        InterCodeNode p3 = p2->next;
        if (p2 != icodelist && p3 != icodelist)
        {
            InterCode *c2 = &p2->code, *c3 = &p3->code;
            if (c->type == I_COND && c2->type == I_GOTO && c3->type == I_LABEL)
            {
                if (c->cond.goto_no == c3->label_no)
                {
                    if (!exist_label(p, p3, c3->label_no))
                    {
                        c->cond.relop = reverse_cond(c->cond.relop);
                        c->cond.goto_no = c2->goto_no;
                        delete_intercode(p2);
                        delete_intercode(p3);
                    }
                }
            }
            else if (c->type == I_GOTO && c2->type == I_LABEL && c->goto_no == c2->label_no)
            {
                if (!exist_label(p, p2, c2->label_no))
                {
                    delete_intercode(p);
                    delete_intercode(p2);
                }
            }
        }

        p = p->next;
    } while (p != icodelist);
}

void translate_Operand(Operand *o, char *dest)
{
    if (o->type == O_CONSTANT)
        sprintf(dest, "#%d", o->const_value);
    else if (o->type == O_VARIABLE)
    {
        if (o->kind == O_VAL)
            sprintf(dest, "v%d", o->var_no);
        else if (o->kind == O_LEFTVAL)
        {
            sprintf(dest, "&v%d", o->var_no);
        }
        else if (o->kind == O_RIGHTVAL)
            sprintf(dest, "*v%d", o->var_no);
    }
    else if (o->type == O_TEMP)
    {
        if (o->kind == O_VAL)
            sprintf(dest, "t%d", o->temp_no);
        else if (o->kind == O_LEFTVAL)
            sprintf(dest, "&t%d", o->temp_no);
        else if (o->kind == O_RIGHTVAL)
            sprintf(dest, "*t%d", o->temp_no);
    }
}

void print_intercode(FILE *fp)
{
    InterCodeNode p = icodelist;
    do
    {
        InterCode *c = &p->code;
        if (c->type == I_LABEL)
        {
            fprintf(fp, "LABEL label%d :\n", c->label_no);
        }
        else if (c->type == I_FUNC)
        {
            fprintf(fp, "FUNCTION %s :\n", c->func_name);
        }
        else if (c->type == I_ASSIGN)
        {
            char l[MAX_NAME_LENGTH], r[MAX_NAME_LENGTH];
            translate_Operand(&c->assign.left, l);
            translate_Operand(&c->assign.right, r);
            fprintf(fp, "%s := %s\n", l, r);
        }
        else if (c->type == I_ADD)
        {
            char l[MAX_NAME_LENGTH], r[MAX_NAME_LENGTH], res[MAX_NAME_LENGTH];
            translate_Operand(&c->add.left, l);
            translate_Operand(&c->add.right, r);
            translate_Operand(&c->add.result, res);
            fprintf(fp, "%s := %s + %s\n", res, l, r);
        }
        else if (c->type == I_SUB)
        {
            char l[MAX_NAME_LENGTH], r[MAX_NAME_LENGTH], res[MAX_NAME_LENGTH];
            translate_Operand(&c->sub.left, l);
            translate_Operand(&c->sub.right, r);
            translate_Operand(&c->sub.result, res);
            fprintf(fp, "%s := %s - %s\n", res, l, r);
        }
        else if (c->type == I_MULT)
        {
            char l[MAX_NAME_LENGTH], r[MAX_NAME_LENGTH], res[MAX_NAME_LENGTH];
            translate_Operand(&c->mult.left, l);
            translate_Operand(&c->mult.right, r);
            translate_Operand(&c->mult.result, res);
            fprintf(fp, "%s := %s * %s\n", res, l, r);
        }
        else if (c->type == I_DIV)
        {
            char l[MAX_NAME_LENGTH], r[MAX_NAME_LENGTH], res[MAX_NAME_LENGTH];
            translate_Operand(&c->div.left, l);
            translate_Operand(&c->div.right, r);
            translate_Operand(&c->div.result, res);
            fprintf(fp, "%s := %s / %s\n", res, l, r);
        }
        else if (c->type == I_GOTO)
        {
            fprintf(fp, "GOTO label%d\n", c->goto_no);
        }
        else if (c->type == I_COND)
        {
            char l[MAX_NAME_LENGTH], r[MAX_NAME_LENGTH];
            translate_Operand(&c->cond.left, l);
            translate_Operand(&c->cond.right, r);
            if (c->cond.relop == RE_EQ)
            {
                char op[] = "==";
                fprintf(fp, "IF %s %s %s GOTO label%d\n", l, op, r, c->cond.goto_no);
            }
            else if (c->cond.relop == RE_NE)
            {
                char op[] = "!=";
                fprintf(fp, "IF %s %s %s GOTO label%d\n", l, op, r, c->cond.goto_no);
            }
            else if (c->cond.relop == RE_L)
            {
                char op[] = "<";
                fprintf(fp, "IF %s %s %s GOTO label%d\n", l, op, r, c->cond.goto_no);
            }
            else if (c->cond.relop == RE_G)
            {
                char op[] = ">";
                fprintf(fp, "IF %s %s %s GOTO label%d\n", l, op, r, c->cond.goto_no);
            }
            else if (c->cond.relop == RE_LE)
            {
                char op[] = "<=";
                fprintf(fp, "IF %s %s %s GOTO label%d\n", l, op, r, c->cond.goto_no);
            }
            else if (c->cond.relop == RE_GE)
            {
                char op[] = ">=";
                fprintf(fp, "IF %s %s %s GOTO label%d\n", l, op, r, c->cond.goto_no);
            }
        }
        else if (c->type == I_RETURN)
        {
            char exp[MAX_NAME_LENGTH];
            translate_Operand(&c->ret, exp);
            fprintf(fp, "RETURN %s\n", exp);
        }
        else if (c->type == I_DEC)
        {
            char exp[MAX_NAME_LENGTH];
            translate_Operand(&c->dec.decvar, exp);
            fprintf(fp, "DEC %s %d\n", exp, c->dec.size);
        }
        else if (c->type == I_ARG)
        {
            char exp[MAX_NAME_LENGTH];
            translate_Operand(&c->arg, exp);
            fprintf(fp, "ARG %s\n", exp);
        }
        else if (c->type == I_CALL)
        {
            char exp[MAX_NAME_LENGTH];
            translate_Operand(&c->call.ret, exp);
            fprintf(fp, "%s := CALL %s\n", exp, c->call.func_name);
        }
        else if (c->type == I_PARAM)
        {
            char exp[MAX_NAME_LENGTH];
            translate_Operand(&c->param, exp);
            fprintf(fp, "PARAM %s\n", exp);
        }
        else if (c->type == I_READ)
        {
            char exp[MAX_NAME_LENGTH];
            translate_Operand(&c->read, exp);
            fprintf(fp, "READ %s\n", exp);
        }
        else if (c->type == I_WRITE)
        {
            char exp[MAX_NAME_LENGTH];
            translate_Operand(&c->write, exp);
            fprintf(fp, "WRITE %s\n", exp);
        }
        p = p->next;
    } while (p != icodelist);
}

int check_param(FieldList p)
{
    while (p != NULL)
    {
        if (p->type->kind == STRUCTURE)
        {
            fprintf(stderr, "Cannot translate: Code contains variables or parameters of structure type.\n");
            inter_no_error_flag = 0;
            return 0;
        }
        else if (p->type->kind == ARRAY && p->type->array.elem->kind == ARRAY)
        {
            fprintf(stderr, "Cannot translate: Code contains parameters of array with more than 1 dimension.\n");
            inter_no_error_flag = 0;
            return 0;
        }
        p = p->tail;
    }
    return 1;
}

int check_return(Type t)
{
    if (t->kind == STRUCTURE)
    {
        fprintf(stderr, "Cannot translate: Code contains function which returns a structure\n");
        inter_no_error_flag = 0;
        return 0;
    }
    else if (t->kind == ARRAY)
    {
        fprintf(stderr, "Cannot translate: Code contains function which returns a array\n");
        inter_no_error_flag = 0;
        return 0;
    }
    return 1;
}

void translate_ExtDefList(TreeNode *p)
{
    // ExtDefList : empty
    if (p == NULL)
    {
        return;
    }
    // ExtDefList : ExtDef ExtDefList
    else
    {
        translate_ExtDef(p->children[0]);
        translate_ExtDefList(p->children[1]);
    }
}

void translate_ExtDef(TreeNode *p)
{
    // ExtDef : Specifier SEMI
    if (p->num == 2)
    {
        // do nothing
    }
    // ExtDef : Specifier ExtDecList SEMI
    else if (p->children[1]->identifier == EXTDECLIST)
    {
        // no global variables, do nothing
    }
    // ExtDef : Specifier FunDec CompSt
    else
    {
        if (!translate_FunDec(p->children[1]))
            return;
        translate_CompSt(p->children[2]);
        InterCode c;
        c.type = I_END_OF_FUNC;
        insert_intercode(&c);
    }
}

int translate_FunDec(TreeNode *p)
{
    // FunDec : ID LP RP
    // FunDec : ID LP VarList RP
    Symbol sym = search_func(p->children[0]->info);
    if (!check_param(sym->func.param))
        return 0;
    if (!check_return(sym->func.return_type))
        return 0;
    
    // define func
    InterCode c;
    c.type = I_FUNC;
    strcpy(c.func_name, sym->name);
    insert_intercode(&c);

    FieldList f = sym->func.param;
    while (f != NULL)
    {
        int no = new_variable();
        Symbol s = search_variable(f->name);
        s->var.var_no = no;

        c.type = I_PARAM;
        help_insert_variable(&c.param, no);
        insert_intercode(&c);

        f = f->tail;
    }
    return 1;
}

void translate_CompSt(TreeNode *p)
{
    // CompSt : LC DefList StmtList RC
    translate_DefList(p->children[1]);
    translate_StmtList(p->children[2]);    
}

void translate_DefList(TreeNode *p)
{
    // DefList : Empty
    if (p == NULL)
        return;
    // DefList : Def DefList
    translate_Def(p->children[0]);
    translate_DefList(p->children[1]);
}

void translate_Def(TreeNode *p)
{
    // Def : Specifier DecList SEMI
    translate_Specifier(p->children[0]);
    translate_DecList(p->children[1]);
}

void translate_Specifier(TreeNode *p)
{
    if (p->children[0]->identifier == STRUCTSPECIFIER)
    {
        if (inter_no_error_flag)
        {
            fprintf(stderr, "Cannot translate: Code contains variables of structure type.\n");
            inter_no_error_flag = 0;
        }
    }
}

void translate_DecList(TreeNode *p)
{
    // DecList : Dec
    if (p->num == 1)
    {
        translate_Dec(p->children[0]);
    }
    // DecList : Dec COMMA DecList
    else
    {
        translate_Dec(p->children[0]);
        translate_DecList(p->children[2]);
    }
}

void translate_Dec(TreeNode *p)
{
    // Dec : VarDec
    if (p->num == 1)
    {
        translate_VarDec(p->children[0]);
    }
    // Dec : VarDec ASSIGNOP Exp
    else
    {
        int no = translate_VarDec(p->children[0]);

        int t1 = new_temp();
        translate_Exp(p->children[2], t1);
        InterCode c;
        c.type = I_ASSIGN;
        help_insert_variable(&c.assign.left, no);
        help_insert_temp(&c.assign.right, t1);
        insert_intercode(&c);
    }
}

int translate_VarDec(TreeNode *p)
{
    // VarDec : ID
    if (p->num == 1)
    {
        Symbol s = search_variable(p->children[0]->info);
        int no = new_variable();
        s->var.var_no = no;
        return no;
    }
    // VarDec : VarDec LB INT RB
    else
    {
        while (p->num != 1)
            p = p->children[0];
        Symbol s = search_variable(p->children[0]->info);
        int no = new_variable();
        s->var.var_no = no;

        Type t = s->var.type;
        int sz = 1;
        t->array.size;
        while (t->kind == ARRAY)
        {
            sz *= t->array.size;
            t = t->array.elem;
        }

        InterCode c;
        c.type = I_DEC;
        c.dec.size = sz * 4;
        help_insert_variable(&c.dec.decvar, no);
        insert_intercode(&c);
        return no;
    }
}

void translate_StmtList(TreeNode *p)
{
    // StmtList : Empty
    if (p == NULL)
        return;
    // StmtList : Stmt StmtList
    translate_Stmt(p->children[0]);
    translate_StmtList(p->children[1]);
}

void translate_Stmt(TreeNode *p)
{
    // Stmt : CompSt
    if (p->num == 1)
        translate_CompSt(p->children[0]);
    // Stmt : Exp SEMI
    else if (p->num == 2)
        translate_Exp(p->children[0], -1);
    // Stmt : RETURN Exp SEMI
    else if (p->num == 3)
    {
        int t1 = new_temp();
        translate_Exp(p->children[1], t1);
        InterCode c;
        c.type = I_RETURN;
        c.ret.type = O_TEMP;
        c.ret.kind = O_VAL;
        c.ret.temp_no = t1;
        insert_intercode(&c);
    }
    // Stmt : WHILE LP Exp RP Stmt
    else if (p->children[0]->identifier == T_WHILE)
    {
        int l1 = new_label();
        int l2 = new_label();
        int l3 = new_label();
        InterCode c;

        c.type = I_LABEL;
        c.label_no = l1;
        insert_intercode(&c);

        translate_Cond(p->children[2], l2, l3);

        c.type = I_LABEL;
        c.label_no = l2;
        insert_intercode(&c);

        translate_Stmt(p->children[4]);

        c.type = I_GOTO;
        c.goto_no = l1;
        insert_intercode(&c);

        c.type = I_LABEL;
        c.label_no = l3;
        insert_intercode(&c);
    }
    // Stmt : IF LP Exp RP Stmt
    else if (p->num == 5)
    {
        int l1 = new_label();
        int l2 = new_label();
        InterCode c;

        translate_Cond(p->children[2], l1, l2);

        c.type = I_LABEL;
        c.label_no = l1;
        insert_intercode(&c);

        translate_Stmt(p->children[4]);

        c.type = I_LABEL;
        c.label_no = l2;
        insert_intercode(&c);
    }
    // Stmt : IF LP Exp RP Stmt ELSE Stmt
    else
    {
        int l1 = new_label();
        int l2 = new_label();
        int l3 = new_label();
        InterCode c;

        translate_Cond(p->children[2], l1, l2);

        c.type = I_LABEL;
        c.label_no = l1;
        insert_intercode(&c);

        translate_Stmt(p->children[4]);

        c.type = I_GOTO;
        c.goto_no = l3;
        insert_intercode(&c);

        c.type = I_LABEL;
        c.label_no = l2;
        insert_intercode(&c);

        translate_Stmt(p->children[6]);

        c.type = I_LABEL;
        c.label_no = l3;
        insert_intercode(&c);
    }
}

void translate_Exp(TreeNode *p, int temp_no)
{
    // Exp : ID
    if (p->num == 1 && p->children[0]->identifier == T_ID)
    {
        if (temp_no == -1)
            return;
        Symbol s = search_variable(p->children[0]->info);
        InterCode c;
        c.type = I_ASSIGN;
        help_insert_temp(&c.assign.left, temp_no);
        help_insert_variable(&c.assign.right, s->var.var_no);

        if (s->var.type->kind == ARRAY && s->var.is_param == 0)
            c.assign.right.kind = O_LEFTVAL;

        insert_intercode(&c);
    }
    // Exp : INT
    else if (p->children[0]->identifier == T_INT)
    {
        if (temp_no == -1)
            return;
        int val = atoi(p->children[0]->info);
        InterCode c;
        c.type = I_ASSIGN;
        help_insert_temp(&c.assign.left, temp_no);
        help_insert_constant(&c.assign.right, val);
        insert_intercode(&c);
    }
    // Exp : FLOAT
    else if (p->children[0]->identifier == T_FLOAT)
    {
        // do nothing
    }
    // Exp : MINUS Exp
    else if (p->children[0]->identifier == T_MINUS)
    {
        if (temp_no == -1)
            return;
        int t1 = new_temp();
        translate_Exp(p->children[1], t1);
        InterCode c;
        c.type = I_SUB;
        help_insert_constant(&c.sub.left, 0);
        help_insert_temp(&c.sub.right, t1);
        help_insert_temp(&c.sub.result, temp_no);
        insert_intercode(&c);
    }
    // Exp : NOT Exp
    // Exp : Exp AND Exp
    // Exp : Exp OR Exp
    // Exp : Exp RELOP Exp
    else if (p->children[0]->identifier == T_NOT
        || p->children[1]->identifier == T_AND
        || p->children[1]->identifier == T_OR
        || p->children[1]->identifier == T_RELOP)
    {
        if (temp_no == -1)
            return;
        int l1 = new_label();
        int l2 = new_label();
        InterCode c;

        c.type = I_ASSIGN;
        help_insert_temp(&c.assign.left, temp_no);
        help_insert_constant(&c.assign.right, 0);
        insert_intercode(&c);

        translate_Cond(p, l1, l2);

        c.type = I_LABEL;
        c.label_no = l1;
        insert_intercode(&c);

        c.type = I_ASSIGN;
        help_insert_temp(&c.assign.left, temp_no);
        help_insert_constant(&c.assign.right, 1);
        insert_intercode(&c);

        c.type = I_LABEL;
        c.label_no = l2;
        insert_intercode(&c);
    }
    // Exp : LP Exp RP
    else if (p->children[0]->identifier == T_LP)
    {
        if (temp_no == -1)
            return;
        translate_Exp(p->children[1], temp_no);
    }
    // Exp : Exp ASSIGNOP Exp
    else if (p->children[1]->identifier == T_ASSIGNOP)
    {
        // left value must be id or array
        if (p->children[0]->num == 1)
        {
            // ID
            InterCode c;

            Symbol s = search_variable(p->children[0]->children[0]->info);
            int t1 = s->var.var_no;
            
            int t2 = new_temp();
            translate_Exp(p->children[2], t2);

            c.type = I_ASSIGN;
            help_insert_variable(&c.assign.left, t1);
            help_insert_temp(&c.assign.right, t2);
            insert_intercode(&c);

            if (temp_no != -1)
            {
                c.type = I_ASSIGN;
                help_insert_temp(&c.assign.left, temp_no);
                help_insert_variable(&c.assign.right, t1);
                insert_intercode(&c);
            }
        }
        // TODO: assignment to array element
        else if (p->children[0]->num == 4)
        {
            // array
            int t2 = new_temp();
            translate_Exp(p->children[2], t2);

            int t1 = new_temp();
            int a = translate_array(p->children[0], t1);
            
            InterCode c;
            
            if (a >= 0)
            {
                int t3 = new_temp();
                c.type = I_ASSIGN;
                help_insert_temp(&c.assign.left, t3);
                help_insert_variable(&c.assign.right, a);
                c.assign.right.kind = O_LEFTVAL;
                insert_intercode(&c);

                c.type = I_ADD;
                help_insert_temp(&c.add.left, t3);
                help_insert_temp(&c.add.right, t1);
                help_insert_temp(&c.add.result, t1);
                insert_intercode(&c);
            }
            else
            {
                c.type = I_ADD;
                help_insert_temp(&c.add.result, t1);
                help_insert_variable(&c.add.left, -a);
                help_insert_temp(&c.add.right, t1);
                insert_intercode(&c);
            }

            c.type = I_ASSIGN;
            help_insert_temp(&c.assign.left, t1);
            c.assign.left.kind = O_RIGHTVAL;
            help_insert_temp(&c.assign.right, t2);
            insert_intercode(&c);

            if (temp_no != -1)
            {
                c.type = I_ASSIGN;
                help_insert_temp(&c.assign.left, temp_no);
                help_insert_temp(&c.assign.right, t2);
                insert_intercode(&c);
            }
        }
    }
    // Exp : Exp PLUS Exp
    else if (p->children[1]->identifier == T_PLUS)
    {
        if (temp_no == -1)
            return;
        InterCode c;
        c.type = I_ADD;
        int t1 = new_temp();
        int t2 = new_temp();
        translate_Exp(p->children[0], t1);
        translate_Exp(p->children[2], t2);
        help_insert_temp(&c.add.left, t1);
        help_insert_temp(&c.add.right, t2);
        help_insert_temp(&c.add.result, temp_no);
        insert_intercode(&c);
    }
    // Exp : Exp MINUS Exp
    else if (p->children[1]->identifier == T_MINUS)
    {
        if (temp_no == -1)
            return;
        InterCode c;
        c.type = I_SUB;
        int t1 = new_temp();
        int t2 = new_temp();
        translate_Exp(p->children[0], t1);
        translate_Exp(p->children[2], t2);
        help_insert_temp(&c.sub.left, t1);
        help_insert_temp(&c.sub.right, t2);
        help_insert_temp(&c.sub.result, temp_no);
        insert_intercode(&c);
    }
    // Exp : Exp STAR Exp
    else if (p->children[1]->identifier == T_STAR)
    {
        if (temp_no == -1)
            return;
        InterCode c;
        c.type = I_MULT;
        int t1 = new_temp();
        int t2 = new_temp();
        translate_Exp(p->children[0], t1);
        translate_Exp(p->children[2], t2);
        help_insert_temp(&c.mult.left, t1);
        help_insert_temp(&c.mult.right, t2);
        help_insert_temp(&c.mult.result, temp_no);
        insert_intercode(&c);
    }
    // Exp : Exp DIV Exp
    else if (p->children[1]->identifier == T_DIV)
    {
        if (temp_no == -1)
            return;
        InterCode c;
        c.type = I_DIV;
        int t1 = new_temp();
        int t2 = new_temp();
        translate_Exp(p->children[0], t1);
        translate_Exp(p->children[2], t2);
        help_insert_temp(&c.div.left, t1);
        help_insert_temp(&c.div.right, t2);
        help_insert_temp(&c.div.result, temp_no);
        insert_intercode(&c);
    }
    // Exp : Exp DOT ID
    else if (p->children[1]->identifier == T_DOT)
    {
        if (inter_no_error_flag)
        {
            fprintf(stderr, "Cannot translate: Code contains variables of structure type.\n");
            inter_no_error_flag = 0;
        }
    }
    // Exp : ID LP RP
    else if (p->children[1]->identifier == T_LP && p->children[2]->identifier == T_RP)
    {
        Symbol s = search_func(p->children[0]->info);

        InterCode c;
        if (strcmp(s->name, "read") == 0)
        {
            c.type = I_READ;
            help_insert_temp(&c.read, temp_no);
            insert_intercode(&c);
            return;
        }
        if (temp_no == -1)
            temp_no = new_temp();

        c.type = I_CALL;
        help_insert_temp(&c.call.ret, temp_no);
        strcpy(c.call.func_name, s->name);
        insert_intercode(&c);
    }
    // Exp : ID LP Args RP
    else if (p->children[1]->identifier == T_LP && p->children[2]->identifier == ARGS)
    {
        Symbol s = search_func(p->children[0]->info);
        InterCode c;

        if (strcmp(s->name, "write") == 0)
        {
            int t1 = new_temp();
            translate_Exp(p->children[2]->children[0], t1);

            c.type = I_WRITE;
            help_insert_temp(&c.write, t1);
            insert_intercode(&c);

            if (temp_no == -1)
                temp_no = new_temp();

            c.type = I_ASSIGN;
            help_insert_temp(&c.assign.left, temp_no);
            help_insert_constant(&c.assign.right, 0);
            insert_intercode(&c);
            return;
        }

        if (temp_no == -1)
            temp_no = new_temp();

        translate_Args(p->children[2]);

        c.type = I_CALL;
        help_insert_temp(&c.call.ret, temp_no);
        strcpy(c.call.func_name, s->name);
        insert_intercode(&c);
    }
    // Exp : Exp LB Exp RB
    else if (p->children[1]->identifier == T_LB)
    {
        if (temp_no == -1)
            return;
        
        int t1 = new_temp();
        int a = translate_array(p, t1);
        
        InterCode c;

        if (a >= 0)
        {
            int t3 = new_temp();
            c.type = I_ASSIGN;
            help_insert_temp(&c.assign.left, t3);
            help_insert_variable(&c.assign.right, a);
            c.assign.right.kind = O_LEFTVAL;
            insert_intercode(&c);

            c.type = I_ADD;
            help_insert_temp(&c.add.left, t3);
            help_insert_temp(&c.add.right, t1);
            help_insert_temp(&c.add.result, t1);
            insert_intercode(&c);
        }
        else
        {
            c.type = I_ADD;
            help_insert_temp(&c.add.result, t1);
            help_insert_variable(&c.add.left, -a);
            help_insert_temp(&c.add.right, t1);
            insert_intercode(&c);
        }

        c.type = I_ASSIGN;
        help_insert_temp(&c.assign.left, temp_no);
        help_insert_temp(&c.assign.right, t1);
        c.assign.right.kind = O_RIGHTVAL;
        insert_intercode(&c);
    }
}

int translate_array(TreeNode *p, int temp_no)
{
    // Exp : Exp LB Exp RB
    int t1 = new_temp();
    translate_Exp(p->children[2], t1);

    Type t = Exp(p->children[0]);
    int ret = -1;
    int t2 = -1;
    if (p->children[0]->children[0]->identifier == T_ID)
    {
        Symbol s = search_variable(p->children[0]->children[0]->info);
        ret = s->var.var_no;
        if (s->var.is_param)
            ret = -ret;
    }
    else
    {
        t2 = new_temp();
        ret = translate_array(p->children[0], t2);
    }
    int sz = 4;
    if (t->array.elem->kind == ARRAY)
        sz = t->array.elem->array.size;
    
    InterCode c;
    if (t2 != -1)
    {
        c.type = I_ADD;
        help_insert_temp(&c.add.result, t1);
        help_insert_temp(&c.add.left, t2);
        help_insert_temp(&c.add.right, t1);
        insert_intercode(&c);
    }
    c.type = I_MULT;
    help_insert_temp(&c.mult.result, temp_no);
    help_insert_temp(&c.mult.left, t1);
    help_insert_constant(&c.mult.right, sz);
    insert_intercode(&c);
    return ret;
}

void translate_Args(TreeNode *p)
{
    // Args : Exp
    if (p->num == 1)
    {
        int t1 = new_temp();
        translate_Exp(p->children[0], t1);
        InterCode c;
        
        c.type = I_ARG;
        help_insert_temp(&c.arg, t1);
        
        insert_intercode(&c);
    }
    // Args : Exp COMMA Args
    else
    {
        translate_Args(p->children[2]);

        int t1 = new_temp();
        translate_Exp(p->children[0], t1);
        InterCode c;
        
        c.type = I_ARG;
        help_insert_temp(&c.arg, t1);

        insert_intercode(&c);
    }
}

int get_relop(char *s)
{
    if (strcmp(s, "==") == 0)
        return RE_EQ;
    else if (strcmp(s, "<") == 0)
        return RE_L;
    else if (strcmp(s, "<=") == 0)
        return RE_LE;
    else if (strcmp(s, ">") == 0)
        return RE_G;
    else if (strcmp(s, ">=") == 0)
        return RE_GE;
    else if (strcmp(s, "!=") == 0)
        return RE_NE;
    return -1;
}

void translate_Cond(TreeNode *p, int true_label, int false_label)
{
    // Exp : NOT Exp
    if (p->num == 2 && p->children[0]->identifier == T_NOT)
    {
        translate_Cond(p->children[1], false_label, true_label);
    }
    // Exp : Exp AND Exp
    else if (p->num == 3 && p->children[1]->identifier == T_AND)
    {
        int l1 = new_label();
        translate_Cond(p->children[0], l1, false_label);
        InterCode c;
        c.type = I_LABEL;
        c.label_no = l1;
        insert_intercode(&c);
        translate_Cond(p->children[2], true_label, false_label);
    }
    // Exp : Exp OR Exp
    else if (p->num == 3 && p->children[1]->identifier == T_OR)
    {
        int l1 = new_label();
        translate_Cond(p->children[0], true_label, l1);
        InterCode c;
        c.type = I_LABEL;
        c.label_no = l1;
        insert_intercode(&c);
        translate_Cond(p->children[2], true_label, false_label);
    }
    // Exp : Exp RELOP Exp
    else if (p->num == 3 && p->children[1]->identifier == T_RELOP)
    {
        int t1 = new_temp(), t2 = new_temp();
        translate_Exp(p->children[0], t1);
        translate_Exp(p->children[2], t2);

        InterCode c;
        c.type = I_COND;
        c.cond.relop = get_relop(p->children[1]->info);
        help_insert_temp(&c.cond.left, t1);
        help_insert_temp(&c.cond.right, t2);
        c.cond.goto_no = true_label;
        insert_intercode(&c);

        c.type = I_GOTO;
        c.goto_no = false_label;
        insert_intercode(&c);
    }
    else
    {
        int t1 = new_temp();
        translate_Exp(p, t1);
        InterCode c;
        
        c.type = I_COND;
        help_insert_temp(&c.cond.left, t1);
        help_insert_constant(&c.cond.right, 0);
        c.cond.relop = RE_NE;
        c.cond.goto_no = true_label;
        insert_intercode(&c);

        c.type = I_GOTO;
        c.goto_no = false_label;
        insert_intercode(&c);
    }
}