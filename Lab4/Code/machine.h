#ifndef __MACHINE_H__
#define __MACHINE_H__

#include "treenode.h"

typedef struct {
    enum {P_REG, P_ADDR, P_NOWHERE} type;
    char reg[16];
    int offset;
} VarPos;

void prepare_machine(FILE *fp);
void translate_intercode(InterCodeNode n);
void translate_intercode_list();

#endif