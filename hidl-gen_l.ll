/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
E			[Ee][+-]?{D}+
FS			(f|F|l|L)
IS			(u|U|l|L)*

COMPONENT               {L}({L}|{D})*
DOT                     [.]
PATH                    {COMPONENT}({DOT}{COMPONENT})*
AT                      [@]
VERSION                 {AT}{D}+{DOT}{D}+

%{

#include "Annotation.h"
#include "AST.h"
#include "CompoundType.h"
#include "ConstantExpression.h"
#include "EnumType.h"
#include "HandleType.h"
#include "Method.h"
#include "ScalarType.h"
#include "StringType.h"

#include "hidl-gen_y.h"

#include <assert.h>

using namespace android;
using token = yy::parser::token;

void comment(yyscan_t yyscanner, struct yyguts_t *yyg);
int check_type(yyscan_t yyscanner, struct yyguts_t *yyg);

#define SCALAR_TYPE(kind)                                       \
    do {                                                        \
        yylval->type = new ScalarType(ScalarType::kind);        \
        return token::SCALAR;                                   \
    } while (0)

#define YY_USER_ACTION yylloc->step(); yylloc->columns(yyleng);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

%}

%option yylineno
%option noyywrap
%option reentrant
%option bison-bridge
%option bison-locations

%%

"/*"			{ comment(yyscanner, yyg); }
"//"[^\r\n]*            { /* skip C++ style comment */ }

"enum"			{ return token::ENUM; }
"extends"		{ return token::EXTENDS; }
"generates"		{ return token::GENERATES; }
"import"		{ return token::IMPORT; }
"interface"		{ return token::INTERFACE; }
"package"		{ return token::PACKAGE; }
"struct"		{ return token::STRUCT; }
"typedef"		{ return token::TYPEDEF; }
"union"			{ return token::UNION; }
"vec"			{ return token::VEC; }
"oneway"		{ return token::ONEWAY; }

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

"handle"		{ yylval->type = new HandleType; return token::SCALAR; }
"string"		{ yylval->type = new StringType; return token::SCALAR; }

"("			{ return('('); }
")"			{ return(')'); }
"<"			{ return('<'); }
">"			{ return('>'); }
"{"			{ return('{'); }
"}"			{ return('}'); }
"["			{ return('['); }
"]"			{ return(']'); }
":"			{ return(':'); }
";"			{ return(';'); }
","			{ return(','); }
"."			{ return('.'); }
"="			{ return('='); }
"+"			{ return('+'); }
"-"			{ return('-'); }
"*"			{ return('*'); }
"/"			{ return('/'); }
"%"			{ return('%'); }
"&"			{ return('&'); }
"|"			{ return('|'); }
"^"			{ return('^'); }
"<<"			{ return(token::LSHIFT); }
">>"			{ return(token::RSHIFT); }
"&&"			{ return(token::LOGICAL_AND); }
"||"			{ return(token::LOGICAL_OR);  }
"!"			{ return('!'); }
"~"			{ return('~'); }
"<="			{ return(token::LEQ); }
">="			{ return(token::GEQ); }
"=="			{ return(token::EQUALITY); }
"!="			{ return(token::NEQ); }
"?"			{ return('?'); }
"@"			{ return('@'); }

{PATH}{VERSION}?"::"{PATH}      { yylval->str = strdup(yytext); return token::FQNAME; }
{VERSION}"::"{PATH}             { yylval->str = strdup(yytext); return token::FQNAME; }
{PATH}{VERSION}                 { yylval->str = strdup(yytext); return token::FQNAME; }
{COMPONENT}({DOT}{COMPONENT})+  { yylval->str = strdup(yytext); return token::FQNAME; }
{COMPONENT}                     { yylval->str = strdup(yytext); return token::IDENTIFIER; }

0[xX]{H}+{IS}?		{ yylval->str = strdup(yytext); return token::INTEGER; }
0{D}+{IS}?		{ yylval->str = strdup(yytext); return token::INTEGER; }
{D}+{IS}?		{ yylval->str = strdup(yytext); return token::INTEGER; }
L?\"(\\.|[^\\"])*\"	{ yylval->str = strdup(yytext); return token::STRING_LITERAL; }

{D}+{E}{FS}?		{ yylval->str = strdup(yytext); return token::FLOAT; }
{D}+\.{E}?{FS}?		{ yylval->str = strdup(yytext); return token::FLOAT; }
{D}*\.{D}+{E}?{FS}?	{ yylval->str = strdup(yytext); return token::FLOAT; }

[\n]		{ yylloc->lines(); }
.			{ /* ignore bad characters */ }

%%

#pragma clang diagnostic pop

void comment(yyscan_t yyscanner, yyguts_t *yyg) {
    char c, c1;

loop:
    while ((c = yyinput(yyscanner)) != '*' && c != 0) {
    }

    if ((c1 = yyinput(yyscanner)) != '/' && c != 0)
    {
        unput(c1);
        goto loop;
    }
}

status_t parseFile(AST *ast) {
    FILE *file = fopen(ast->getFilename().c_str(), "rb");

    if (file == NULL) {
        return -errno;
    }

    yyscan_t scanner;
    yylex_init_extra(ast, &scanner);
    ast->setScanner(scanner);

    yyset_in(file, scanner);
    int res = yy::parser(ast).parse();

    yylex_destroy(scanner);
    ast->setScanner(NULL);

    fclose(file);
    file = NULL;

    if (res != 0) {
        return UNKNOWN_ERROR;
    }

    return OK;
}
