#include "treenode.h"

TreeNode *root = NULL;
int error_num = 0;

extern int yydebug;
extern FILE *yyin;
extern int yylex();
extern int yyrestart(FILE *);
extern int yyparse();

int main(int argc, char *argv[])
{
    if (argc <= 1)
    {
        fprintf(stderr, "Please provide cmm file to compile!\n");
        return 1;
    }
    FILE *f = fopen(argv[1], "r");
    if (!f)
    {
        perror(argv[1]);
        return 1;
    }
    FILE *fout = fopen(argv[2], "w");
    if (!fout)
    {
        perror(argv[2]);
        return 1;
    }

    yyrestart(f);
    yyparse();
    if (error_num == 0)
    {
        // printTree(root, 0);
        semantic_analysis(root);
        generate_immediate(root, fout);
        generate_machine(fout);
    }
    // print_symtable();
    destroyTree(root);

    fclose(f);
    fclose(fout);

    return 0;
}
