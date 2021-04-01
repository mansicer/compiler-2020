#include "symtable.h"

int hidden_struct_num = 0;

unsigned int get_hash_code(char *name)
{
    unsigned int val = 0, i;
    for (; *name; ++name)
    {
        val = (val << 2) + *name;
        if (i = val & (~MAX_SYMTABLE_ENTRY))
            val = (val ^ (i >> 12)) & MAX_SYMTABLE_ENTRY;
    }
    return val;
}

int insert_variable(char *name, Type type, int defined, int is_param)
{
    // look up
    unsigned int idx = get_hash_code(name);
    SymList tmp = symtable[idx];
    while (tmp != NULL)
    {
        if (strcmp(name, tmp->sym.name) == 0  && (tmp->sym.symtype == SYM_VARIABLE || tmp->sym.symtype == SYM_STRUCT))
            return 0;
        tmp = tmp->next;
    }

    // malloc
    SymList sym = malloc(sizeof(struct SymList_));
    if (sym == NULL)
    {
        fprintf(stderr, "malloc Symbol error! \n");
        exit(-1);
    }
    sym->next = NULL;

    // insert
    if (symtable[idx] == NULL)
        symtable[idx] = sym;
    else
    {
        tmp = symtable[idx];
        for (; tmp->next != NULL; tmp = tmp->next);
        tmp->next = sym;
    }
    
    // set value
    sym->sym.symtype = SYM_VARIABLE;
    strcpy(sym->sym.name, name);
    sym->sym.var.type = copy_type(type);
    sym->sym.var.is_param = is_param;
    sym->sym.defined = defined;
    sym->sym.var.var_no = -1;
    return 1;
}

int insert_struct(char *name, Type type, int defined)
{
    // look up
    unsigned int idx = get_hash_code(name);
    SymList tmp = symtable[idx];
    while (tmp != NULL)
    {
        if (strcmp(name, tmp->sym.name) == 0  && (tmp->sym.symtype == SYM_VARIABLE || tmp->sym.symtype == SYM_STRUCT))
            return 0;
        tmp = tmp->next;
    }

    // malloc
    SymList sym = malloc(sizeof(struct SymList_));
    if (sym == NULL)
    {
        fprintf(stderr, "malloc Symbol error! \n");
        exit(-1);
    }
    sym->next = NULL;

    // insert
    if (symtable[idx] == NULL)
        symtable[idx] = sym;
    else
    {
        tmp = symtable[idx];
        for (; tmp->next != NULL; tmp = tmp->next);
        tmp->next = sym;
    }
    
    // set value
    sym->sym.symtype = SYM_STRUCT;
    strcpy(sym->sym.name, name);
    sym->sym.var.type = copy_type(type);
    sym->sym.defined = defined;
    return 1;
}

int insert_func(char *name, Type return_type, FieldList param, int defined)
{
    // look up
    unsigned int idx = get_hash_code(name);
    SymList tmp = symtable[idx];
    while (tmp != NULL)
    {
        if (strcmp(name, tmp->sym.name) == 0 && tmp->sym.symtype == SYM_FUNCTION)
            return 0;
        tmp = tmp->next;
    }

    // malloc
    SymList sym = malloc(sizeof(struct SymList_));
    if (sym == NULL)
    {
        fprintf(stderr, "malloc Symbol error! \n");
        exit(-1);
    }
    sym->next = NULL;

    // insert
    if (symtable[idx] == NULL)
        symtable[idx] = sym;
    else
    {
        tmp = symtable[idx];
        for (; tmp->next != NULL; tmp = tmp->next);
        tmp->next = sym;
    }

    // set value
    sym->sym.symtype = SYM_FUNCTION;
    strcpy(sym->sym.name, name);
    sym->sym.func.return_type = copy_type(return_type);
    sym->sym.func.param = copy_fieldlist(param);
    sym->sym.defined = defined;
    return 1;
}

Symbol search_variable(char *name)
{
    unsigned int idx = get_hash_code(name);
    SymList tmp = symtable[idx];
    while (tmp != NULL)
    {
        if (strcmp(name, tmp->sym.name) == 0 && tmp->sym.symtype == SYM_VARIABLE)
        {
            return &tmp->sym;
        }
        else
            tmp = tmp->next;
    }
    return NULL;
}

Symbol search_struct(char *name)
{
    unsigned int idx = get_hash_code(name);
    SymList tmp = symtable[idx];
    while (tmp != NULL)
    {
        if (strcmp(name, tmp->sym.name) == 0 && tmp->sym.symtype == SYM_STRUCT)
        {
            return &tmp->sym;
        }
        else
            tmp = tmp->next;
    }
    return NULL;
}

Symbol search_func(char *name)
{
    unsigned int idx = get_hash_code(name);
    SymList tmp = symtable[idx];
    while (tmp != NULL)
    {
        if (strcmp(name, tmp->sym.name) == 0 && tmp->sym.symtype == SYM_FUNCTION)
        {
            return &tmp->sym;
        }
        else
            tmp = tmp->next;
    }
    return NULL;
}

int type_equal(Type t1, Type t2)
{
    if (t1 == NULL && t2 == NULL)
        return 1;
    else if (t1 == NULL)
        return 0;
    else if (t2 == NULL)
        return 0;
    if (t1->kind != t2->kind)
        return 0;
    if (t1->kind == BASIC)
    {
        if (t1->basic != t2->basic)
            return 0;
        else
            return 1;
    }
    else if (t1->kind == ARRAY)
    {
        return type_equal(t1->array.elem, t2->array.elem);
    }
    else if (t1->kind == STRUCTURE)
    {
        return fieldlist_equal(t1->structure.field, t2->structure.field);
    }
}

int fieldlist_equal(FieldList f1, FieldList f2)
{
    if (f1 == NULL && f2 == NULL)
        return 1;
    else if (f1 == NULL)
        return 0;
    else if (f2 == NULL)
        return 0;
    
    if (!type_equal(f1->type, f2->type))
        return 0;
    else if (f1->tail == NULL && f2->tail == NULL)
        return 1;
    else if (f1->tail != NULL && f2->tail != NULL)
        return fieldlist_equal(f1->tail, f2->tail);
    else
        return 0;
}

int func_args_equal(FieldList f1, FieldList f2)
{
    if (f1 == NULL && f2 == NULL)
        return 1;
    else if (f1 == NULL)
        return 0;
    else if (f2 == NULL)
        return 0;
    
    if (!type_equal(f1->type, f2->type))
        return 0;
    return func_args_equal(f1->tail, f2->tail);
}

Type copy_type(Type src)
{
    if (src == NULL)
        return NULL;
    Type dest = malloc(sizeof(struct Type_));
    if (src->kind == BASIC)
    {
        dest->kind = BASIC;
        dest->basic = src->basic;
    }
    else if (src->kind == ARRAY)
    {
        dest->kind = ARRAY;
        dest->array.size = src->array.size;
        dest->array.elem = copy_type(src->array.elem);
    }
    else
    {
        dest->kind = STRUCTURE;
        dest->structure.field = copy_fieldlist(src->structure.field);
    }
    return dest;
}

FieldList copy_fieldlist(FieldList src)
{
    if (src == NULL)
        return NULL;
    FieldList dest = malloc(sizeof(struct FieldList_));
    dest->tail = NULL;
    strcpy(dest->name, src->name);
    dest->type = copy_type(src->type);
    if (src->tail)
        dest->tail = copy_fieldlist(src->tail);
    return dest;
}

Type find_fieldlist(FieldList f, char *name)
{
    while (f)
    {
        if (strcmp(f->name, name) == 0)
            return f->type;
        f = f->tail;
    }
    return NULL;
}

void print_symtable()
{
    for (int i = 0; i < MAX_SYMTABLE_ENTRY; i++)
    {
        if (symtable[i] != NULL)
        {
            SymList n = symtable[i];
            while (n)
            {
                Symbol sym = &n->sym;
                char msg[MAX_MSG_LENGTH];
                char TypeList[][16] = { "BASIC", "ARRAY", "STRUCTURE" };
                if (sym->symtype == SYM_VARIABLE)
                    sprintf(msg, "%s(idx %d): %s %d, type %s", sym->name, i, "variable", sym->defined, TypeList[sym->var.type->kind]);
                else
                    sprintf(msg, "%s(idx %d): %s %d", sym->name, i, "function", sym->defined);
                fprintf(stderr, "%s\n", msg);
                if (sym->symtype == SYM_VARIABLE)
                {
                    print_type(sym->var.type);
                    fprintf(stderr, "\n");
                }
                n = n->next;
            }
        }
    }
}

void print_type_help(Type t, int tab)
{
    for (int i = 0; i < tab; i++)
        fprintf(stderr, " ");
    if (t == NULL)
    {
        fprintf(stderr, "no type\n");
        return;
    }
    if (t->kind == BASIC)
    {
        if (t->basic == B_INT)
            fprintf(stderr, "type INT\n");
        else
            fprintf(stderr, "type FLOAT\n");
    }
    else if (t->kind == ARRAY)
    {
        fprintf(stderr, "type array of size %d, element type:\n", t->array.size);
        print_type_help(t->array.elem, tab + 2);
    }
    else if (t->kind == STRUCTURE)
    {
        fprintf(stderr, "type struct, member type:\n");
        print_fieldlist(t->structure.field, tab + 2);
    }
}

void print_type(Type t)
{
    print_type_help(t, 0);
    fprintf(stderr, "\n");
}

void print_fieldlist(FieldList f, int tab)
{
    for (int i = 0; i < tab; i++)
        fprintf(stderr, " ");
    if (f == NULL)
    {
        fprintf(stderr, "no field\n");
        return;
    }
    fprintf(stderr, "field %s, type:\n", f->name);
    print_type_help(f->type, tab + 2);
    if (f->tail)
        print_fieldlist(f->tail, tab);
}