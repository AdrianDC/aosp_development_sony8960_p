D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
E			[Ee][+-]?{D}+
FS			(f|F|l|L)
IS			(u|U|l|L)*

%{

#include "AST.h"
#include "CompoundType.h"
#include "EnumType.h"
#include "HandleType.h"
#include "Method.h"
#include "RefType.h"
#include "ScalarType.h"
#include "StringType.h"

#include "hidl-gen_y.h"

#include <assert.h>

using namespace android;

void count(struct yyguts_t *yyg);
void comment(yyscan_t yyscanner, struct yyguts_t *yyg);
int check_type(yyscan_t yyscanner, struct yyguts_t *yyg);

#define SCALAR_TYPE(kind)                                       \
    do {                                                        \
        count(yyg);                                             \
        yylval->type = new ScalarType(ScalarType::kind);        \
        return TYPENAME;                                        \
    } while (0)

%}

%option reentrant
%option bison-bridge
%option extra-type="android::AST *"

%%

"/*"			{ comment(yyscanner, yyg); }
"//"[^\r\n]*            { /* skip C++ style comment */ }

"const"			{ count(yyg); return(CONST); }
"enum"			{ count(yyg); return(ENUM); }
"extends"		{ count(yyg); return(EXTENDS); }
"generates"		{ count(yyg); return(GENERATES); }
"import"		{ count(yyg); return(IMPORT); }
"interface"		{ count(yyg); return(INTERFACE); }
"package"		{ count(yyg); return(PACKAGE); }
"struct"		{ count(yyg); return(STRUCT); }
"typedef"		{ count(yyg); return(TYPEDEF); }
"union"			{ count(yyg); return(UNION); }
"vec"			{ count(yyg); return(VEC); }
"version"		{ count(yyg); return(VERSION); }

"char"			{ SCALAR_TYPE(KIND_CHAR); }
"bool"			{ SCALAR_TYPE(KIND_BOOL); }
"opaque"		{ SCALAR_TYPE(KIND_OPAQUE); }
"int8_t"		{ SCALAR_TYPE(KIND_INT8); }
"uint8_t"		{ SCALAR_TYPE(KIND_UINT8); }
"int16_t"		{ SCALAR_TYPE(KIND_INT16); }
"uint16_t"		{ SCALAR_TYPE(KIND_UINT16); }
"int32_t"		{ SCALAR_TYPE(KIND_INT32); }
"uint32_t"		{ SCALAR_TYPE(KIND_UINT32); }
"int64_t"		{ SCALAR_TYPE(KIND_INT64); }
"uint64_t"		{ SCALAR_TYPE(KIND_UINT64); }
"float"			{ SCALAR_TYPE(KIND_FLOAT); }
"double"		{ SCALAR_TYPE(KIND_DOUBLE); }

"handle"		{ count(yyg); yylval->type = new HandleType; return TYPENAME; }
"string"		{ count(yyg); yylval->type = new StringType; return TYPENAME; }

"("			{ count(yyg); return('('); }
")"			{ count(yyg); return(')'); }
"<"			{ count(yyg); return('<'); }
">"			{ count(yyg); return('>'); }
"{"			{ count(yyg); return('{'); }
"}"			{ count(yyg); return('}'); }
"["			{ count(yyg); return('['); }
"]"			{ count(yyg); return(']'); }
":"			{ count(yyg); return(':'); }
";"			{ count(yyg); return(';'); }
","			{ count(yyg); return(','); }
"."			{ count(yyg); return('.'); }
"="			{ count(yyg); return('='); }

{L}({L}|{D})*		{ count(yyg); return check_type(yyscanner, yyg); }

0[xX]{H}+{IS}?		{ count(yyg); yylval->str = strdup(yytext); return(INTEGER); }
0{D}+{IS}?		{ count(yyg); yylval->str = strdup(yytext); return(INTEGER); }
{D}+{IS}?		{ count(yyg); yylval->str = strdup(yytext); return(INTEGER); }
L?\"(\\.|[^\\"])*\"	{ count(yyg); yylval->str = strdup(yytext); return(STRING_LITERAL); }

[ \t\v\n\f]		{ count(yyg); }
.			{ /* ignore bad characters */ }

%%

int yywrap(yyscan_t scanner) {
    return 1;
}

void comment(yyscan_t yyscanner, yyguts_t *yyg) {
    char c, c1;

loop:
    while ((c = yyinput(yyscanner)) != '*' && c != 0)
        putchar(c);

    if ((c1 = yyinput(yyscanner)) != '/' && c != 0)
    {
        unput(c1);
        goto loop;
    }

    if (c != 0) {
        putchar(c1);
    }
}


int column = 0;

void count(yyguts_t *yyg) {
    int i;

    for (i = 0; yytext[i] != '\0'; i++)
        if (yytext[i] == '\n')
            column = 0;
        else if (yytext[i] == '\t')
            column += 8 - (column % 8);
        else
            column++;

    ECHO;
}

int check_type(yyscan_t yyscanner, yyguts_t *yyg) {
    AST *ast = yyextra;

    Type *type = ast->lookupType(yytext);
    if (type != NULL) {
        yylval->type = new RefType(yytext, type);

        return TYPENAME;
    }

    yylval->str = strdup(yytext);

    return IDENTIFIER;
}

void parseFile(AST *ast, const char *path) {
    FILE *file = fopen(path, "rb");

    yyscan_t scanner;
    yylex_init_extra(ast, &scanner);
    ast->setScanner(scanner);

    yyset_in(file, scanner);
    int res = yyparse(ast);
    assert(res == 0);

    yylex_destroy(scanner);
    ast->setScanner(NULL);

    fclose(file);
    file = NULL;
}
