%option yylineno

%{
    #include "treenode.h"
    int yycolumn = 1;
    #define YY_USER_ACTION \
        yylloc.first_line = yylloc.last_line = yylineno; \
        yylloc.first_column = yycolumn; \
        yylloc.last_column = yycolumn + yyleng - 1; \
        yycolumn += yyleng;
    
    extern int error_num;
    #include "syntax.tab.h"
%}

SEMI        ";"
COMMA       ","
ASSIGNOP    "="
RELOP       ">"|"<"|">="|"<="|"=="|"!="
PLUS        "+"
MINUS       "-"
STAR        "*"
DIV         "/"
AND         "&&"
OR          "||"
DOT         "."
NOT         "!"
TYPE        int|float
LP          "("
RP          ")"
LB          "["
RB          "]"
LC          "{"
RC          "}"
STRUCT      struct
RETURN      return
IF          if
ELSE        else
WHILE       while
INT         0|([1-9][0-9]*)
OCTONARY    0[0-7]+
HEXADECIMAL 0[xX][0-9A-Fa-f]+
FLOAT       [0-9]+\.[0-9]+
ID          [_a-zA-Z][_a-zA-Z0-9]*
WHITESPACE  [ \t\r]

%%

\n {
    yycolumn = 1;
}
{WHITESPACE}+ { }
{SEMI} {
    yylval.node = createTreeNode("SEMI", yylineno, "");
    return SEMI;
}
{COMMA} {
    yylval.node = createTreeNode("COMMA", yylineno, "");
    return COMMA;
}
{ASSIGNOP} {
    yylval.node = createTreeNode("ASSIGNOP", yylineno, "");
    return ASSIGNOP;
}
{RELOP} {
    yylval.node = createTreeNode("RELOP", yylineno, "");
    return RELOP;
}
{PLUS} {
    yylval.node = createTreeNode("PLUS", yylineno, "");
    return PLUS;
}
{MINUS} {
    yylval.node = createTreeNode("MINUS", yylineno, "");
    return MINUS;
}
{STAR} {
    yylval.node = createTreeNode("STAR", yylineno, "");
    return STAR;
}
{DIV} {
    yylval.node = createTreeNode("DIV", yylineno, "");
    return DIV;
}
{AND} {
    yylval.node = createTreeNode("AND", yylineno, "");
    return AND;
}
{OR} {
    yylval.node = createTreeNode("OR", yylineno, "");
    return OR;
}
{DOT} {
    yylval.node = createTreeNode("DOT", yylineno, "");
    return DOT;
}
{NOT} {
    yylval.node = createTreeNode("NOT", yylineno, "");
    return NOT;
}
{TYPE} {
    yylval.node = createTreeNode("TYPE", yylineno, yytext);
    return TYPE;
}
{LP} {
    yylval.node = createTreeNode("LP", yylineno, "");
    return LP;
}
{RP} {
    yylval.node = createTreeNode("RP", yylineno, "");
    return RP;
}
{LB} {
    yylval.node = createTreeNode("LB", yylineno, "");
    return LB;
}
{RB} {
    yylval.node = createTreeNode("RB", yylineno, "");
    return RB;
}
{LC} {
    yylval.node = createTreeNode("LC", yylineno, "");
    return LC;
}
{RC} {
    yylval.node = createTreeNode("RC", yylineno, "");
    return RC;
}
{STRUCT} {
    yylval.node = createTreeNode("STRUCT", yylineno, "");
    return STRUCT;
}
{RETURN} {
    yylval.node = createTreeNode("RETURN", yylineno, "");
    return RETURN;
}
{IF} {
    yylval.node = createTreeNode("IF", yylineno, "");
    return IF;
}
{ELSE} {
    yylval.node = createTreeNode("ELSE", yylineno, "");
    return ELSE;
}
{WHILE} {
    yylval.node = createTreeNode("WHILE", yylineno, "");
    return WHILE;
}
{ID} {
    yylval.node = createTreeNode("ID", yylineno, yytext);
    return ID;
}
{OCTONARY} {
    char val[32];
    int tmp = strtol(yytext, NULL, 8);
    sprintf(val, "%d", tmp);
    yylval.node = createTreeNode("INT", yylineno, val);
    return INT;
}
{HEXADECIMAL} {
    char val[32];
    int tmp = strtol(yytext, NULL, 16);
    sprintf(val, "%d", tmp);
    yylval.node = createTreeNode("INT", yylineno, val);
    return INT;
}
{INT} {
    yylval.node = createTreeNode("INT", yylineno, yytext);
    return INT;
}
{FLOAT} {
    yylval.node = createTreeNode("FLOAT", yylineno, yytext);
    return INT;
}

. { error_num++; fprintf(stderr, "Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext); }
%%
