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

%{

#include "AST.h"
#include "Declaration.h"
#include "Type.h"
#include "VarDeclaration.h"
#include "FunctionDeclaration.h"
#include "CompositeDeclaration.h"
#include "Define.h"
#include "Include.h"
#include "EnumVarDeclaration.h"
#include "Note.h"
#include "TypeDef.h"
#include "Expression.h"

#include "c2hal_y.h"

#include <stdio.h>
#include <utils/String8.h>
#include <algorithm>

using namespace android;

extern int yylex(YYSTYPE *yylval_param, YYLTYPE *llocp, void *);

int yyerror(YYLTYPE *llocp, AST *, const char *s) {
    extern bool should_report_errors;

    if (!should_report_errors) {
      return 0;
    }

    fflush(stdout);
    LOG(ERROR) << " "
               << s
               << " near line "
               << llocp->first_line;

    return 0;
}

#define scanner ast->scanner()

std::string get_last_comment() {
    extern std::string last_comment;

    std::string ret{last_comment};

    // clear the last comment now that it's been taken
    last_comment = "";

    return ret;
}

%}

%parse-param { android::AST *ast }
%lex-param   { void *scanner }
%locations
%pure-parser
%glr-parser

%token START_HEADER
%token START_EXPR

%token STRUCT
%token UNION
%token ENUM
%token CONST
%token VOID
%token INCLUDE
%token DEFINE
%token TYPEDEF
%token UNSIGNED
%token SIGNED
%token LSHIFT
%token RSHIFT
%token VARARGS
%token NAMESPACE
%token EXTERN
%token C_STRING

%left ','
%right '?' ':'
%left '|'
%left '^'
%left '&'
%left RSHIFT LSHIFT
%left '+' '-'
%left '*' '/' '%'
%right '~' '!' UMINUS UPLUS
%left ARRAY_SUBSCRIPT FUNCTION_CALL

%token<str> ID
%token<str> COMMENT
%token<str> VALUE
%token<str> INTEGRAL_VALUE
%token<str> INCLUDE_FILE
%token<str> FUNCTION
%token<str> DEFINE_SLURP
%token<str> OTHER_STATEMENT

%type<expression> opt_array
%type<expression> expr
%type<expressions> args
%type<type> type
%type<qualifier> type_qualifier
%type<qualifiers> type_qualifiers
%type<declaration> declaration
%type<declarations> declarations
%type<composite> struct_or_union_declaration
%type<composite> enum_declaration
%type<param> param
%type<params> params
%type<qualification> struct_or_union
%type<str> opt_id
%type<include> include
%type<enum_var> enum_var
%type<declarations> enum_vars

%start parse_selector

%union {
    const char *str;
    int count;
    android::Declaration *declaration;
    android::CompositeDeclaration *composite;
    std::vector<android::Declaration *> *declarations;
    android::EnumVarDeclaration *enum_var;
    android::Declaration *param;
    std::vector<android::Declaration *> *params;
    android::Type *type;
    android::Type::Qualifier *qualifier;
    android::Type::Qualifier::Qualification qualification;
    std::vector<android::Type::Qualifier*> *qualifiers;
    android::Include *include;
    std::vector<android::Include *> *includes;
    android::Expression *expression;
    std::vector<android::Expression *> *expressions;
}

%%

parse_selector
    : START_HEADER header
    | START_EXPR expr_parser
    ;

expr_parser
    : expr
      {
        ast->setExpression($1);
      }
    ;

header
    : declarations /* well, we are a header file */
      {
        std::reverse($1->begin(), $1->end());
        ast->setDeclarations($1);
      }
    ;

declarations
    : /* EMPTY */
      {
        $$ = new std::vector<Declaration *>;
      }
    | declaration declarations
      {
        $$ = $2;
        $$->push_back($1);
      }
    | EXTERN C_STRING '{' declarations '}' declarations
      {
        $6->push_back(new Note("} // end of extern C"));
        $6->insert($6->end(), $4->begin(), $4->end());
        $6->push_back(new Note("extern \"C\" { "));
        delete $4;

        $$ = $6;
      }
    ;

declaration
    : param ';'
      {
        $$ = $1;
        $$->setComment(get_last_comment());
      }
    | struct_or_union_declaration ';'
      {
        $$ = $1;
      }
    | enum_declaration ';'
      {
        $$ = $1;
      }
    | TYPEDEF struct_or_union_declaration ';'
      {
        // ignore that it is a typedef, for our purposes it doesn't matter
        $$ = $2;
      }
    | TYPEDEF enum_declaration ';'
      {
        // ignore that it is a typedef, for our purposes it doesn't matter
        $$ = $2;
      }
    | TYPEDEF param ';' /* looks like 'typedef const int8_t store;' */
      {
        $$ = new TypeDef($2->getName(), $2);
        $$->setComment(get_last_comment());
      }
    | DEFINE ID DEFINE_SLURP
      {
        $$ = new Define($2, $3);
        $$->setComment(get_last_comment());
      }
    | OTHER_STATEMENT
      {
        $$ = new Note($1);
        $$->setComment(get_last_comment());
      }
    | FUNCTION
      {
        $$ = new Note($1);
        $$->setComment(get_last_comment());
      }
    | type ID '=' expr ';'
      {
        $$ = new Note($1->decorateName($2) + " = " + $4->toString());
      }
    | include
      {
        $$ = $1;
        $$->setComment(get_last_comment());
      }
    | NAMESPACE ID '{' declarations '}'
      {
        $$ = new CompositeDeclaration(Type::Qualifier::STRUCT,
                                               $2,
                                               $4);

        get_last_comment(); // clear it
        $$->setComment("/* from namespace declaration */");
      }
    ;

include
    : INCLUDE '<' INCLUDE_FILE '>'
      {
        $$ = new Include($3, true /* isLibrary */);
      }
    | INCLUDE '"' INCLUDE_FILE '"'
      {
        $$ = new Include($3, false /* isLibrary */);
      }
    ;

struct_or_union_declaration
    : struct_or_union opt_id
      {
        $<str>$ = strdup(get_last_comment().c_str());
      }
                             '{' declarations '}' opt_id
      {
        std::reverse($5->begin(), $5->end());
        $$ = new CompositeDeclaration($1, $2, $5);
        $$->setComment($<str>3);

        if(!std::string($7).empty()) {
          $$->setName($7);
        }
      }
    ;

enum_declaration
    : ENUM opt_id
      {
        $<str>$ = strdup(get_last_comment().c_str());
      }
                           '{' enum_vars '}' opt_id
      {
        std::reverse($5->begin(), $5->end());
        $$ = new CompositeDeclaration(Type::Qualifier::ENUM, $2, $5);
        $$->setComment($<str>3);

        if(!std::string($7).empty()) {
          $$->setName($7);
        }
      }
    ;

enum_vars
    : /* EMPTY */
      {
        $$ = new std::vector<Declaration *>;
      }
    | enum_var /* a comma is optional on the last item */
      {
        $$ = new std::vector<Declaration *>;
        $$->push_back($1);
      }
    | enum_var ',' enum_vars
      {
        $$ = $3;
        $$->push_back($1);
      }
    ;

enum_var
    : ID
      {
        $$ = new EnumVarDeclaration($1, NULL);
        $$->setComment(get_last_comment());
      }
    | ID '=' expr
      {
        $$ = new EnumVarDeclaration($1, $3);
        $$->setComment(get_last_comment());
      }
    ;

params
    : /* EMPTY */
      {
        $$ = new std::vector<Declaration *>;
      }
    | param
      {
        $$ = new std::vector<Declaration *>;
        $$->push_back($1);
      }
    | param ',' params
      {
        $$ = $3;
        $$->push_back($1);
      }
    ;

param
    : type opt_array
      {
        $1->setArray($2);

        // allow for either "const int myvar" or "const int"
        // as a parameter declaration
        std::string lastId = $1->removeLastId();

        $$ = new VarDeclaration($1, lastId);
      }
    | type '(' '*' ID opt_array ')' '(' params ')'
      {
        $1->setArray($5);
        std::reverse($8->begin(), $8->end());
        $$ = new FunctionDeclaration($1, $4, $8);
      }
    | type ID '(' params ')'
      {
        std::reverse($4->begin(), $4->end());
        $$ = new FunctionDeclaration($1, $2, $4);
      }
    | type '(' ID ')' '(' params ')'
      {
        std::reverse($6->begin(), $6->end());
        $$ = new FunctionDeclaration($1, $3, $6);
      }
    | VARARGS
      {
        $$ = new VarDeclaration(new Type(NULL), "...");
      }
    ;

type
    : type_qualifiers
      {
        std::reverse($1->begin(), $1->end());
        $$ = new Type($1);
      }
    ;

type_qualifiers
    : type_qualifier
     {
        $$ = new std::vector<Type::Qualifier *>;
        $$->push_back($1);
     }
    | type_qualifier type_qualifiers
     {
        $$ = $2;
        $$->push_back($1);
     }
    ;

opt_id
    : /* EMPTY */             { $$ = ""; }
    |
     ID                       { $$ = $1; }
    ;

expr
    : ID                      { $$ = Expression::atom(Expression::Type::UNKOWN, $1); }
    | VALUE                   { $$ = Expression::atom(Expression::Type::UNKOWN, $1); }
    | INTEGRAL_VALUE          { $$ = Expression::atom(Expression::integralType($1), $1); }
    | '(' expr ')'            { $$ = Expression::parenthesize($2); }
    | ID '[' expr ']' %prec ARRAY_SUBSCRIPT {
                                $$ = Expression::arraySubscript($1, $3);
                              }
    | ID '(' args ')' %prec FUNCTION_CALL {
                                std::reverse($3->begin(), $3->end());
                                $$ = Expression::functionCall($1, $3);
                              }
    | expr '?' expr ':' expr  { $$ = Expression::ternary($1, $3, $5); }
    | expr '+' expr           { $$ = Expression::binary($1, "+", $3); }
    | expr '-' expr           { $$ = Expression::binary($1, "-", $3); }
    | expr '/' expr           { $$ = Expression::binary($1, "/", $3); }
    | expr '*' expr           { $$ = Expression::binary($1, "*", $3); }
    | expr '%' expr           { $$ = Expression::binary($1, "%%", $3); }
    | expr '&' expr           { $$ = Expression::binary($1, "&", $3); }
    | expr '|' expr           { $$ = Expression::binary($1, "|", $3); }
    | expr '^' expr           { $$ = Expression::binary($1, "^", $3); }
    | expr LSHIFT expr        { $$ = Expression::binary($1, "<<", $3); }
    | expr RSHIFT expr        { $$ = Expression::binary($1, ">>", $3); }
    | '~' expr                { $$ = Expression::unary("~", $2); }
    | '-' expr %prec UMINUS   { $$ = Expression::unary("-", $2); }
    | '+' expr %prec UPLUS    { $$ = Expression::unary("+", $2); }
    ;

args
    : /* empty */
      {
        $$ = new std::vector<Expression *>;
      }
    | expr
      {
        $$ = new std::vector<Expression *>;
        $$->push_back($1);
      }
    | expr ',' args
      {
        $$ = $3;
        $$->push_back($1);
      }
    ;

type_qualifier
    : UNSIGNED                { $$ = new Type::Qualifier(Type::Qualifier::UNSIGNED); }
    | SIGNED                  { $$ = new Type::Qualifier(Type::Qualifier::SIGNED); }
    | VOID                    { $$ = new Type::Qualifier(Type::Qualifier::VOID); }
    | '*'                     { $$ = new Type::Qualifier(Type::Qualifier::POINTER); }
    | CONST                   { $$ = new Type::Qualifier(Type::Qualifier::CONST); }
    | ID                      { $$ = new Type::Qualifier(Type::Qualifier::ID, $1); }
    | '<' type '>'            { $$ = new Type::Qualifier(Type::Qualifier::GENERICS, $2); }
    | ENUM                    { $$ = new Type::Qualifier(Type::Qualifier::ENUM); }
    | struct_or_union         { $$ = new Type::Qualifier($1); }
    ;

struct_or_union
    : STRUCT                  { $$ = android::Type::Qualifier::STRUCT; }
    | UNION                   { $$ = android::Type::Qualifier::UNION; }
    ;

opt_array
    : /* empty */             { $$ = NULL; }
    | '[' ']'                 { $$ = Expression::atom(Expression::Type::UNKOWN, " "); }
    | '[' expr ']'            { $$ = $2; }
    ;

%%