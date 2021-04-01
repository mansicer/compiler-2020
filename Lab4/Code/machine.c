#include "machine.h"

VarPos *var_range;
Operand arg_list[10000];
int cur_offset = 0;
FILE *fout;

void prepare_machine(FILE *fp)
{
    fout = fp;

    fprintf(fout, ".data\n");
    fprintf(fout, "_prompt: .asciiz \"Enter an integer: \"\n");
    fprintf(fout, "_ret: .asciiz \"\\n\"\n");
    fprintf(fout, ".globl main\n");
    fprintf(fout, ".text\n");

    fprintf(fout, "read:\n");
    fprintf(fout, "  li $v0, 4\n");
    fprintf(fout, "  la $a0, _prompt\n");
    fprintf(fout, "  syscall\n");
    fprintf(fout, "  li $v0, 5\n");
    fprintf(fout, "  syscall\n");
    fprintf(fout, "  jr $ra\n");
    fprintf(fout, "\n");
    
    fprintf(fout, "write:\n");
    fprintf(fout, "  li $v0, 1\n");
    fprintf(fout, "  syscall\n");
    fprintf(fout, "  li $v0, 4\n");
    fprintf(fout, "  la $a0, _ret\n");
    fprintf(fout, "  syscall\n");
    fprintf(fout, "  move $v0, $0\n");
    fprintf(fout, "  jr $ra\n");
    fprintf(fout, "\n");
    
    
    var_range = malloc(sizeof(VarPos) * (variable_no + temporary_no));
    for (int i = 0; i < variable_no + temporary_no; i++)
    {
        var_range[i].type = P_NOWHERE;
    }
}


int get_idx(Operand *o)
{
    int idx = 0;
    if (o->type == O_VARIABLE)
        idx = o->var_no;
    else if (o->type == O_TEMP)
        idx = o->temp_no + variable_no;
    else
        fprintf(stderr, "Wrong Operand when getting reg\n");
    return idx;
}

void append_operand(Operand *o)
{
    int idx = get_idx(o);

    if (var_range[idx].type == P_NOWHERE)
    {
        cur_offset -= 4;
        var_range[idx].type = P_ADDR;
        var_range[idx].offset = cur_offset;
        fprintf(fout, "  addi $sp, $sp, -4\n");
    }
}

void operand_to_reg(Operand *o, char *reg)
{
    if (o->type == O_CONSTANT)
    {
        fprintf(fout, "  li %s, %d\n", reg, o->const_value);
    }
    else
    {
        int idx = get_idx(o);
        if (var_range[idx].type == P_REG)
        {
            fprintf(fout, "  move %s, %s\n", reg, var_range[idx].reg);
        }
        else
        {
            fprintf(fout, "  lw %s, %d($fp)\n", reg, var_range[idx].offset);
        }
    }
}

void reg_to_operand(Operand *o, char *reg)
{
    append_operand(o);
    int idx = get_idx(o);
    if (var_range[idx].type == P_REG)
    {
        fprintf(fout, "  move %s, %s\n", var_range[idx].reg, reg);
    }
    else if (var_range[idx].type == P_ADDR)
    {
        fprintf(fout, "  sw %s, %d($fp)\n", reg, var_range[idx].offset);
    }
}

void translate_intercode(InterCodeNode n)
{
    InterCode *c = &n->code;
    if (c->type == I_LABEL)
    {
        fprintf(fout, "label%d:\n", c->label_no);
    }
    else if (c->type == I_FUNC)
    {
        fprintf(fout, "%s:\n", c->func_name);
        fprintf(fout, "  addi $sp, $sp, -4\n");
        fprintf(fout, "  sw $fp, 0($sp)\n");
        fprintf(fout, "  move $fp, $sp\n");
    }
    else if (c->type == I_ASSIGN)
    {
        if (c->assign.left.kind == O_RIGHTVAL)
        {
            char reg1[10] = "$t0";
            char reg2[10] = "$t1";
            operand_to_reg(&c->assign.right, reg1);
            operand_to_reg(&c->assign.left, reg2);
            fprintf(fout, "  sw %s, 0(%s)\n", reg1, reg2);
        }
        else if (c->assign.right.kind == O_VAL)
        {
            char reg[10] = "$t0";
            operand_to_reg(&c->assign.right, reg);
            reg_to_operand(&c->assign.left, reg);
        }
        else if (c->assign.right.kind == O_LEFTVAL)
        {
            char reg[10] = "$t0";
            int i = get_idx(&c->assign.right);
            fprintf(fout, "  la %s, %d($fp)\n", reg, var_range[i].offset);
            reg_to_operand(&c->assign.left, reg);
        }
        else if (c->assign.right.kind == O_RIGHTVAL)
        {
            char reg[10] = "$t0";
            operand_to_reg(&c->assign.right, reg);
            fprintf(fout, "  lw %s, 0(%s)\n", reg, reg);
            reg_to_operand(&c->assign.left, reg);
        }
    }
    else if (c->type == I_ADD)
    {
        char reg1[10] = "$t0";
        char reg2[10] = "$t1";
        
        operand_to_reg(&c->add.left, reg1);
        operand_to_reg(&c->add.right, reg2);

        fprintf(fout, "  add %s, %s, %s\n", reg1, reg1, reg2);
        
        reg_to_operand(&c->add.result, reg1);
    }
    else if (c->type == I_SUB)
    {
        char reg1[10] = "$t0";
        char reg2[10] = "$t1";
        
        operand_to_reg(&c->sub.left, reg1);
        operand_to_reg(&c->sub.right, reg2);

        fprintf(fout, "  sub %s, %s, %s\n", reg1, reg1, reg2);
        
        reg_to_operand(&c->sub.result, reg1);
    }
    else if (c->type == I_MULT)
    {
        char reg1[10] = "$t0";
        char reg2[10] = "$t1";
        
        operand_to_reg(&c->mult.left, reg1);
        operand_to_reg(&c->mult.right, reg2);

        fprintf(fout, "  mul %s, %s, %s\n", reg1, reg1, reg2);
        
        reg_to_operand(&c->mult.result, reg1);
    }
    else if (c->type == I_DIV)
    {
        char reg1[10] = "$t0";
        char reg2[10] = "$t1";
        
        operand_to_reg(&c->div.left, reg1);
        operand_to_reg(&c->div.right, reg2);

        fprintf(fout, "  div %s, %s\n", reg1, reg2);
        fprintf(fout, "  mflo %s\n", reg1);
        
        reg_to_operand(&c->add.result, reg1);
    }
    else if (c->type == I_GOTO)
    {
        fprintf(fout, "  j label%d\n", c->goto_no);
    }
    else if (c->type == I_COND)
    {
        char reg1[10] = "$t0";
        char reg2[10] = "$t1";
        
        operand_to_reg(&c->cond.left, reg1);
        operand_to_reg(&c->cond.right, reg2);

        if (c->cond.relop == RE_EQ)
        {
            fprintf(fout, "  beq %s, %s, label%d\n", reg1, reg2, c->cond.goto_no);
        }
        else if (c->cond.relop == RE_NE)
        {
            fprintf(fout, "  bne %s, %s, label%d\n", reg1, reg2, c->cond.goto_no);
        }
        else if (c->cond.relop == RE_L)
        {
            fprintf(fout, "  blt %s, %s, label%d\n", reg1, reg2, c->cond.goto_no);
        }
        else if (c->cond.relop == RE_G)
        {
            fprintf(fout, "  bgt %s, %s, label%d\n", reg1, reg2, c->cond.goto_no);
        }
        else if (c->cond.relop == RE_LE)
        {
            fprintf(fout, "  ble %s, %s, label%d\n", reg1, reg2, c->cond.goto_no);
        }
        else if (c->cond.relop == RE_GE)
        {
            fprintf(fout, "  bge %s, %s, label%d\n", reg1, reg2, c->cond.goto_no);
        }
    }
    else if (c->type == I_RETURN)
    {
        char reg[10] = "$v0";
        operand_to_reg(&c->ret, reg);

        fprintf(fout, "  move $sp, $fp\n");
        fprintf(fout, "  lw $fp, 0($fp)\n");
        fprintf(fout, "  addi $sp, $sp, 4\n");

        fprintf(fout, "  jr $ra\n");
    }
    else if (c->type == I_DEC)
    {
        cur_offset -= c->dec.size;
        int idx = get_idx(&c->dec.decvar);
        var_range[idx].type = P_ADDR;
        var_range[idx].offset = cur_offset;
        fprintf(fout, "  addi $sp, $sp, -%d\n", c->dec.size);
    }
    else if (c->type == I_ARG)
    {
        InterCodeNode l = n->next;
        int remain_args = 0;
        while (l->code.type != I_CALL)
        {
            if (l->code.type == I_ARG)
                remain_args++;
            l = l->next;
        }
        arg_list[remain_args] = c->arg;
        arg_list[remain_args] = c->arg;
    }
    else if (c->type == I_CALL)
    {
        char reg[10] = "$t0";
        
        Symbol sym = search_func(c->call.func_name);
        int length = get_length(sym->func.param);
        
        if (length > 4)
        {
            for (int i = 3; i >= 0; i--)
            {
                fprintf(fout, "  move $t%d, $a%d\n", i + 2, i);
                operand_to_reg(&arg_list[i], reg);
                fprintf(fout, "  move $a%d, %s\n", i, reg);
            }
            for (int i = length - 1; i >= 4; i--)
            {
                operand_to_reg(&arg_list[i], reg);
                fprintf(fout, "  addi $sp, $sp, -4\n");
                fprintf(fout, "  sw %s, 0($sp)\n", reg);
            }
        }
        else
        {
            for (int i = length - 1; i >= 0; i--)
            {
                fprintf(fout, "  move $t%d, $a%d\n", i + 2, i);
                operand_to_reg(&arg_list[i], reg);
                fprintf(fout, "  move $a%d, %s\n", i, reg);
            }
        }

        fprintf(fout, "  addi $sp, $sp, -4\n");
        fprintf(fout, "  sw $ra, 0($sp)\n");

        fprintf(fout, "  jal %s\n", c->call.func_name);
        fprintf(fout, "  lw $ra, 0($sp)\n");
        fprintf(fout, "  addi $sp, $sp, 4\n");

        if (length > 4)
        {
            fprintf(fout, "  addi $sp, $sp, %d\n", (length - 4) * 4);
            for (int i = 0; i < 4; i++)
            {
                fprintf(fout, "  move $a%d, $t%d\n", i, i + 2);
            }
        }
        else
        {
            for (int i = 0; i < length; i++)
            {
                fprintf(fout, "  move $a%d, $t%d\n", i, i + 2);
            }
        }
        
        fprintf(fout, "  move $v0, %s\n", reg);
        reg_to_operand(&c->call.ret, reg);
    }
    else if (c->type == I_PARAM)
    {
        int above_params = 0;
        InterCodeNode l = n->prev;
        while (l->code.type != I_FUNC)
        {
            if (l->code.type == I_PARAM)
                above_params++;
            l = l->prev;
        }
        if (above_params < 4)
        {
            int idx = get_idx(&c->param);
            var_range[idx].type = P_REG;
            char reg[10];
            sprintf(reg, "$a%d", above_params);
            strcpy(var_range[idx].reg, reg);
        }
        else
        {
            int offset = (above_params - 4) * 4 + 8;
            int idx = get_idx(&c->param);
            var_range[idx].type = P_ADDR;
            var_range[idx].offset = offset;
        }
    }
    else if (c->type == I_READ)
    {
        fprintf(fout, "  addi $sp, $sp, -4\n");
        fprintf(fout, "  sw $ra, 0($sp)\n");
        fprintf(fout, "  jal read\n");
        fprintf(fout, "  lw $ra, 0($sp)\n");
        fprintf(fout, "  addi $sp, $sp, 4\n");
        reg_to_operand(&c->read, "$v0");
    }
    else if (c->type == I_WRITE)
    {
        fprintf(fout, "  addi $sp, $sp, -4\n");
        fprintf(fout, "  sw $a0, 0($sp)\n");

        operand_to_reg(&c->write, "$a0");
        fprintf(fout, "  addi $sp, $sp, -4\n");
        fprintf(fout, "  sw $ra, 0($sp)\n");
        fprintf(fout, "  jal write\n");
        fprintf(fout, "  lw $ra, 0($sp)\n");
        fprintf(fout, "  addi $sp, $sp, 4\n");

        fprintf(fout, "  lw $a0, 0($sp)\n");
        fprintf(fout, "  addi $sp, $sp, 4\n");
    }
    else if (c->type == I_END_OF_FUNC)
    {
        cur_offset = 0;
    }
}

void translate_intercode_list()
{
    InterCodeNode p = icodelist;
    do
    {
        translate_intercode(p);
        p = p->next;
    } while (p != icodelist);
}