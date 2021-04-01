%locations
%{
    #include "lex.yy.c"
    #include "treenode.h"
    
    extern TreeNode *root;
    extern int yylineno;
    extern int error_num;
    // declarations
    int yyerror(char *msg);
    void syntax_error(char *msg);
%}

%union {
    TreeNode *node;
}

// terminal token
%token <node> INT FLOAT ID SEMI COMMA ASSIGNOP RELOP PLUS MINUS STAR DIV AND OR DOT NOT
%token <node> TYPE LP RP LB RB LC RC STRUCT RETURN IF ELSE WHILE

%type <node> Program ExtDefList ExtDef ExtDecList Specifier StructSpecifier OptTag Tag VarDec FunDec VarList ParamDec 
%type <node> CompSt StmtList Stmt DefList Def DecList Dec Exp Args

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left LP RP LB RB DOT


%%

// High level definations
Program     :   ExtDefList { YY_USER_ACTION; $$ = createTreeNode(PROGRAM, @$.first_line, ""); insertChild($$, $1); root = $$; }
    ;
ExtDefList  :   ExtDef ExtDefList { YY_USER_ACTION; $$ = createTreeNode(EXTDEFLIST, @$.first_line, ""); insertChildren(2, $$, $1, $2); }
    |	/* empty */ { YY_USER_ACTION; $$ = NULL; }
    ;
ExtDef      :   Specifier ExtDecList SEMI { YY_USER_ACTION; $$ = createTreeNode(EXTDEF, @$.first_line, ""); insertChildren(3, $$, $1, $2, $3); }
    |	Specifier SEMI { YY_USER_ACTION; $$ = createTreeNode(EXTDEF, @$.first_line, ""); insertChildren(2, $$, $1, $2); }
    |	Specifier FunDec CompSt { YY_USER_ACTION; $$ = createTreeNode(EXTDEF, @$.first_line, ""); insertChildren(3, $$, $1, $2, $3); }
    
    |   error SEMI { YY_USER_ACTION; syntax_error("invalid defination"); }
    ;
ExtDecList  :   VarDec { YY_USER_ACTION; $$ = createTreeNode(EXTDECLIST, @$.first_line, ""); insertChild($$, $1); }
    |	VarDec COMMA ExtDecList { YY_USER_ACTION; $$ = createTreeNode(EXTDECLIST, @$.first_line, ""); insertChildren(3, $$, $1, $2, $3); }

    |   VarDec error ExtDecList { YY_USER_ACTION; syntax_error("invalid defination"); }
    ;

// Specifiers
Specifier       :   TYPE { YY_USER_ACTION; $$ = createTreeNode(SPECIFIER, @$.first_line, ""); insertChild($$, $1); }
    |	StructSpecifier { YY_USER_ACTION; $$ = createTreeNode(SPECIFIER, @$.first_line, ""); insertChild($$, $1); }
    ;
StructSpecifier :   STRUCT OptTag LC DefList RC { YY_USER_ACTION; $$ = createTreeNode(STRUCTSPECIFIER, @$.first_line, ""); insertChildren(5, $$, $1, $2, $3, $4, $5); }
    |	STRUCT Tag { YY_USER_ACTION; $$ = createTreeNode(STRUCTSPECIFIER, @$.first_line, ""); insertChildren(2, $$, $1, $2); }
    ;
OptTag  :   ID { YY_USER_ACTION; $$ = createTreeNode(OPTTAG, @$.first_line, ""); insertChild($$, $1); }
    |	/* empty */ { YY_USER_ACTION; $$ = NULL; }
    ;
Tag     :   ID { YY_USER_ACTION; $$ = createTreeNode(TAG, @$.first_line, ""); insertChild($$, $1); }
    ;

// Declarators
VarDec      :   ID { YY_USER_ACTION; $$ = createTreeNode(VARDEC, @$.first_line, ""); insertChild($$, $1); }
    |	VarDec LB INT RB { YY_USER_ACTION; $$ = createTreeNode(VARDEC, @$.first_line, ""); insertChildren(4, $$, $1, $2, $3, $4); }

    |   VarDec LB error RB { YY_USER_ACTION; syntax_error("invalid array declaration"); }
    ;
FunDec      :   ID LP VarList RP { YY_USER_ACTION; $$ = createTreeNode(FUNDEC, @$.first_line, ""); insertChildren(4, $$, $1, $2, $3, $4); }
    |	ID LP RP { YY_USER_ACTION; $$ = createTreeNode(FUNDEC, @$.first_line, ""); insertChildren(3, $$, $1, $2, $3); }

    |   LP error RP { YY_USER_ACTION; syntax_error("invalid function declaration"); }
    ;
VarList     :   ParamDec COMMA VarList { YY_USER_ACTION; $$ = createTreeNode(VARLIST, @$.first_line, ""); insertChildren(3, $$, $1, $2, $3); }
    |	ParamDec { YY_USER_ACTION; $$ = createTreeNode(VARLIST, @$.first_line, ""); insertChild($$, $1); }

    |   ParamDec error { YY_USER_ACTION; syntax_error("invalid function defination"); }
    ;
ParamDec    :   Specifier VarDec { YY_USER_ACTION; $$ = createTreeNode(PARAMDEC, @$.first_line, ""); insertChildren(2, $$, $1, $2);}
    ;

// Statements
CompSt      :   LC DefList StmtList RC { YY_USER_ACTION; $$ = createTreeNode(COMPST, @$.first_line, ""); insertChildren(4, $$, $1, $2, $3, $4); }

    |   LC error RC { YY_USER_ACTION; syntax_error("invalid statement block"); }
    ;
StmtList    :   Stmt StmtList { YY_USER_ACTION; $$ = createTreeNode(STMTLIST, @$.first_line, ""); insertChildren(2, $$, $1, $2); }
    |	/* empty */ { YY_USER_ACTION; $$ = NULL; }
    ;
Stmt        :   Exp SEMI { YY_USER_ACTION; $$ = createTreeNode(STMT, @$.first_line, ""); insertChildren(2, $$, $1, $2); }
    |	CompSt { YY_USER_ACTION; $$ = createTreeNode(STMT, @$.first_line, ""); insertChild($$, $1); }
    |	RETURN Exp SEMI { YY_USER_ACTION; $$ = createTreeNode(STMT, @$.first_line, ""); insertChildren(3, $$, $1, $2, $3); }
    |	IF LP Exp RP Stmt %prec LOWER_THAN_ELSE { YY_USER_ACTION; $$ = createTreeNode(STMT, @$.first_line, ""); insertChildren(5, $$, $1, $2, $3, $4, $5); }
    |	IF LP Exp RP Stmt ELSE Stmt { YY_USER_ACTION; $$ = createTreeNode(STMT, @$.first_line, ""); insertChildren(7, $$, $1, $2, $3, $4, $5, $6, $7); }
    |	WHILE LP Exp RP Stmt { YY_USER_ACTION; $$ = createTreeNode(STMT, @$.first_line, ""); insertChildren(5, $$, $1, $2, $3, $4, $5); }
    
    |   error SEMI { YY_USER_ACTION; syntax_error("invalid statement"); }
    |   RETURN Exp error { YY_USER_ACTION; syntax_error("invalid return statement"); }
    |   Exp error { YY_USER_ACTION; syntax_error("invalid statement"); }
    |   IF LP error RP Stmt %prec LOWER_THAN_ELSE { YY_USER_ACTION; syntax_error("invalid expression in if-statement"); }
    |   IF LP error RP Stmt ELSE Stmt { YY_USER_ACTION; syntax_error("invalid expression in if-statement"); }
    |   WHILE LP error RP Stmt { YY_USER_ACTION; syntax_error("invalid expression in while-statement"); }
    ;

// Local definations
DefList     :   Def DefList { YY_USER_ACTION; $$ = createTreeNode(DEFLIST, @$.first_line, ""); insertChildren(2, $$, $1, $2); }
    |	/* empty */ { YY_USER_ACTION; $$ = NULL; }
    ;
Def         :   Specifier DecList SEMI { YY_USER_ACTION; $$ = createTreeNode(DEF, @$.first_line, ""); insertChildren(3, $$, $1, $2, $3); }

            |   Specifier error SEMI { YY_USER_ACTION; syntax_error("invalid defination"); }
            |   Specifier DecList error { YY_USER_ACTION; syntax_error("invalid defination"); }
            ;
DecList     :   Dec { YY_USER_ACTION; $$ = createTreeNode(DECLIST, @$.first_line, ""); insertChild($$, $1); }
    |	Dec COMMA DecList { YY_USER_ACTION; $$ = createTreeNode(DECLIST, @$.first_line, ""); insertChildren(3, $$, $1, $2, $3); }
    ;
Dec         :   VarDec { YY_USER_ACTION; $$ = createTreeNode(DEC, @$.first_line, ""); insertChild($$, $1); }
    |	VarDec ASSIGNOP Exp { YY_USER_ACTION; $$ = createTreeNode(DEC, @$.first_line, ""); insertChildren(3, $$, $1, $2, $3); }
    ;

// Expressions
Exp :   Exp ASSIGNOP Exp { YY_USER_ACTION; $$ = createTreeNode(EXP, @$.first_line, ""); insertChildren(3, $$, $1, $2, $3); }
    |	Exp AND Exp { YY_USER_ACTION; $$ = createTreeNode(EXP, @$.first_line, ""); insertChildren(3, $$, $1, $2, $3); }
    |   Exp OR Exp { YY_USER_ACTION; $$ = createTreeNode(EXP, @$.first_line, ""); insertChildren(3, $$, $1, $2, $3); }
    |   Exp RELOP Exp { YY_USER_ACTION; $$ = createTreeNode(EXP, @$.first_line, ""); insertChildren(3, $$, $1, $2, $3); }
    |   Exp PLUS Exp { YY_USER_ACTION; $$ = createTreeNode(EXP, @$.first_line, ""); insertChildren(3, $$, $1, $2, $3); }
    |   Exp MINUS Exp { YY_USER_ACTION; $$ = createTreeNode(EXP, @$.first_line, ""); insertChildren(3, $$, $1, $2, $3); }
    |   Exp STAR Exp { YY_USER_ACTION; $$ = createTreeNode(EXP, @$.first_line, ""); insertChildren(3, $$, $1, $2, $3); }
    |   Exp DIV Exp { YY_USER_ACTION; $$ = createTreeNode(EXP, @$.first_line, ""); insertChildren(3, $$, $1, $2, $3); }
    |   LP Exp RP { YY_USER_ACTION; $$ = createTreeNode(EXP, @$.first_line, ""); insertChildren(3, $$, $1, $2, $3); }
    |   MINUS Exp { YY_USER_ACTION; $$ = createTreeNode(EXP, @$.first_line, ""); insertChildren(2, $$, $1, $2); }
    |   NOT Exp { YY_USER_ACTION; $$ = createTreeNode(EXP, @$.first_line, ""); insertChildren(2, $$, $1, $2); }
    |   ID LP Args RP { YY_USER_ACTION; $$ = createTreeNode(EXP, @$.first_line, ""); insertChildren(4, $$, $1, $2, $3, $4); }
    |   ID LP RP { YY_USER_ACTION; $$ = createTreeNode(EXP, @$.first_line, ""); insertChildren(3, $$, $1, $2, $3); }
    |   Exp LB Exp RB { YY_USER_ACTION; $$ = createTreeNode(EXP, @$.first_line, ""); insertChildren(4, $$, $1, $2, $3, $4); }
    |   Exp DOT ID { YY_USER_ACTION; $$ = createTreeNode(EXP, @$.first_line, ""); insertChildren(3, $$, $1, $2, $3); }
    |   ID { YY_USER_ACTION; $$ = createTreeNode(EXP, @$.first_line, ""); insertChild($$, $1); }
    |   INT { YY_USER_ACTION; $$ = createTreeNode(EXP, @$.first_line, ""); insertChild($$, $1); }
    |   FLOAT { YY_USER_ACTION; $$ = createTreeNode(EXP, @$.first_line, ""); insertChild($$, $1); }
    
    |   ASSIGNOP error { YY_USER_ACTION; syntax_error("invalid expression"); }
    |   AND error { YY_USER_ACTION; syntax_error("invalid expression"); }           
    |   OR error { YY_USER_ACTION; syntax_error("invalid expression"); }
    |   RELOP error { YY_USER_ACTION; syntax_error("invalid expression"); }
    |   PLUS error { YY_USER_ACTION; syntax_error("invalid expression"); }
    |   MINUS error { YY_USER_ACTION; syntax_error("invalid expression"); }
    |   STAR error { YY_USER_ACTION; syntax_error("invalid expression"); }
    |   DIV error { YY_USER_ACTION; syntax_error("invalid expression"); }
    |   NOT error { YY_USER_ACTION; syntax_error("invalid expression"); }
    |   ID LP error RP { YY_USER_ACTION; syntax_error("invalid expression"); }
    |   LP error RP { YY_USER_ACTION; syntax_error("invalid expression"); }
    |   Exp LB error RB { YY_USER_ACTION; syntax_error("invalid array element"); }
    ;
Args:   Exp COMMA Args { YY_USER_ACTION; $$ = createTreeNode(ARGS, @$.first_line, ""); insertChildren(3, $$, $1, $2, $3); }
    |   Exp { YY_USER_ACTION; $$ = createTreeNode(ARGS, @$.first_line, ""); insertChild($$, $1); }
    
    |   Exp error { YY_USER_ACTION; syntax_error("invalid function call"); }
    ;


%%
int yyerror(char *msg)
{
    // do nothing
    // if (error_num == 0)
    //     fprintf(stderr, "Error type B at Line %d: %s\n", yylineno, msg);
}
void syntax_error(char *msg)
{
    error_num++;
    fprintf(stderr, "Error type B at Line %d: %s.\n", yylineno, msg);
}