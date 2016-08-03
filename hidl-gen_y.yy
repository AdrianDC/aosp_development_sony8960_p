%{

#include "AST.h"
#include "ArrayType.h"
#include "CompoundType.h"
#include "Constant.h"
#include "EnumType.h"
#include "Interface.h"
#include "Method.h"
#include "RefType.h"
#include "TypeDef.h"
#include "VectorType.h"

#include "hidl-gen_y.h"

#include <stdio.h>
#include <utils/String8.h>

using namespace android;

extern int yylex(YYSTYPE *yylval_param, void *yyscanner);
extern int column;

int yyerror(AST *, const char *s) {
    fflush(stdout);
    printf("\n%*s\n%*s\n", column, "^", column, s);

    return 0;
}

#define scanner ast->scanner()

%}

%parse-param { android::AST *ast }
%lex-param { void *scanner }
%pure-parser

%token<str> CONST
%token<str> ENUM
%token<str> EXTENDS
%token<str> FQNAME
%token<str> GENERATES
%token<str> IDENTIFIER
%token<str> IMPORT
%token<str> INTEGER
%token<str> INTERFACE
%token<str> PACKAGE
%token<type> SCALAR
%token<str> STRUCT
%token<str> STRING_LITERAL
%token<str> TYPEDEF
%token<str> UNION
%token<str> VEC

%type<str> optIdentifier package
%type<str> const_value
%type<type> fqname

%type<type> type opt_storage_type
%type<type> enum_declaration
%type<type> struct_or_union_declaration
%type<type> opt_extends

%type<field> field_declaration
%type<fields> field_declarations struct_or_union_body
%type<enumValue> enum_value
%type<enumValues> enum_values
%type<typedVars> typed_vars
%type<typedVar> typed_var
%type<method> method_declaration
%type<compoundStyle> struct_or_union_keyword

%start program

%union {
    const char *str;
    android::Type *type;
    android::CompoundType *compoundType;
    android::CompoundField *field;
    std::vector<android::CompoundField *> *fields;
    android::EnumValue *enumValue;
    std::vector<android::EnumValue *> *enumValues;
    android::TypedVar *typedVar;
    std::vector<android::TypedVar *> *typedVars;
    android::Method *method;
    android::CompoundType::Style compoundStyle;
    android::Vector<std::string> *stringVec;
}

%%

program
    : package imports body
    ;

fqname
    : FQNAME
      {
          $$ = ast->lookupType($1);
          if ($$ == NULL) {
              yyerror(ast,
                      android::String8::format(
                          "Failed to lookup type '%s'.", $1).string());
              YYERROR;
          }
      }
    | IDENTIFIER
      {
          $$ = ast->lookupType($1);
          if ($$ == NULL) {
              yyerror(ast,
                      android::String8::format(
                          "Failed to lookup type '%s'.", $1).string());
              YYERROR;
          }
      }
    | SCALAR
    ;

package
    : PACKAGE FQNAME ';'
      {
          if (!ast->setPackage($2)) {
              yyerror(ast, "Malformed package identifier.");
              YYERROR;
          }
      }

imports
    : /* empty */
    | imports IMPORT FQNAME ';'
      {
          if (!ast->addImport($3)) {
              yyerror(ast,
                      android::String8::format(
                          "Unable to import '%s'.", $3));

              YYERROR;
          }
      }
    | imports IMPORT IDENTIFIER ';'
      {
          if (!ast->addImport($3)) {
              yyerror(ast,
                      android::String8::format(
                          "Unable to import '%s'.", $3));

              YYERROR;
          }
      }
    ;

opt_extends
    : /* empty */ { $$ = NULL; }
    | EXTENDS fqname { $$ = $2; }

body
    : INTERFACE IDENTIFIER opt_extends
      {
          Interface *iface = new Interface($3);

          // Register interface immediately so it can be referenced inside
          // definition.
          if (!ast->addScopedType($2, iface)) {
              YYERROR;
          }

          ast->enterScope(iface);
      }
      '{' interface_declarations '}' ';'
      {
          Interface *iface = static_cast<Interface *>(ast->scope());

          ast->leaveScope();
      }
    | type_declarations
    ;

interface_declarations
    : /* empty */
    | interface_declarations type_declaration
    | interface_declarations method_declaration
      {
          Interface *iface = static_cast<Interface *>(ast->scope());
          iface->addMethod($2);
      }
    ;

type_declarations
    : /* empty */
    | type_declarations type_declaration
    ;

type_declaration
    : named_struct_or_union_declaration ';'
    | named_enum_declaration ';'
    | typedef_declaration ';'
    | const_declaration ';'
    ;

typedef_declaration
    : TYPEDEF type IDENTIFIER
      {
          TypeDef *def = new TypeDef($2);
          if (!ast->addScopedType($3, def)) {
              YYERROR;
          }
      }
    ;

const_value
    : INTEGER
    | STRING_LITERAL ;

const_declaration
    : CONST fqname IDENTIFIER '=' const_value
      {
          Constant *constant = new Constant($3, $2, $5);
          ast->scope()->addConstant(constant);
      }
    ;

method_declaration
    : IDENTIFIER '(' typed_vars ')' ';'
      {
          $$ = new Method($1, $3, new std::vector<TypedVar *>);
      }
    | IDENTIFIER '(' typed_vars ')' GENERATES '(' typed_vars ')' ';'
      {
          $$ = new Method($1, $3, $7);
      }
    ;

typed_vars
    : /* empty */
      {
          $$ = new std::vector<TypedVar *>;
      }
    | typed_var
      {
          $$ = new std::vector<TypedVar *>;
          $$->push_back($1);
      }
    | typed_vars ',' typed_var
      {
          $$ = $1;
          $$->push_back($3);
      }
    ;

typed_var : type IDENTIFIER { $$ = new TypedVar($2, $1); }
    ;


struct_or_union_keyword
    : STRUCT { $$ = CompoundType::STYLE_STRUCT; }
    | UNION { $$ = CompoundType::STYLE_UNION; }
    ;

named_struct_or_union_declaration
    : struct_or_union_keyword IDENTIFIER
      {
          CompoundType *container = new CompoundType($1);
          ast->enterScope(container);
      }
      struct_or_union_body
      {
          CompoundType *container = static_cast<CompoundType *>(ast->scope());

          if (!container->setFields($4)) {
              YYERROR;
          }

          ast->leaveScope();
          if (!ast->addScopedType($2, container)) {
              YYERROR;
          }
      }
    ;

struct_or_union_declaration
    : struct_or_union_keyword optIdentifier
      {
          CompoundType *container = new CompoundType($1);
          ast->enterScope(container);
      }
      struct_or_union_body
      {
          CompoundType *container = static_cast<CompoundType *>(ast->scope());

          if (!container->setFields($4)) {
              YYERROR;
          }

          ast->leaveScope();
          if (!ast->addScopedType($2, container)) {
              YYERROR;
          }

          $$ = new RefType(container);
      }
    ;

struct_or_union_body
    : '{' field_declarations '}' { $$ = $2; }
    ;

field_declarations
    : /* empty */ { $$ = new std::vector<CompoundField *>; }
    | field_declarations field_declaration
      {
          $$ = $1;

          if ($2 != NULL) {
              $$->push_back($2);
          }
      }
    ;

field_declaration
    : type IDENTIFIER ';' { $$ = new CompoundField($2, $1); }
    | struct_or_union_declaration ';' { $$ = NULL; }
    | enum_declaration ';' { $$ = NULL; }
    ;

opt_storage_type
    : /* empty */ { $$ = NULL; }
    | ':' fqname { $$ = $2; }
    ;

opt_comma
    : /* empty */
    | ','
    ;

named_enum_declaration
    : ENUM IDENTIFIER opt_storage_type '{' enum_values opt_comma '}'
      {
          EnumType *enumType = new EnumType($5, $3);
          if (!ast->addScopedType($2, enumType)) {
              YYERROR;
          }
      }
    ;

enum_declaration
    : ENUM '{' enum_values opt_comma '}'
      {
          EnumType *enumType = new EnumType($3);
          if (!ast->addScopedType(NULL /* localName */, enumType)) {
              YYERROR;
          }

          $$ = new RefType(enumType);
      }
    | ENUM IDENTIFIER opt_storage_type '{' enum_values opt_comma '}'
      {
          EnumType *enumType = new EnumType($5, $3);
          if (!ast->addScopedType($2, enumType)) {
              YYERROR;
          }

          $$ = new RefType(enumType);
      }
    ;

enum_value
    : IDENTIFIER { $$ = new EnumValue($1); }
    | IDENTIFIER '=' INTEGER { $$ = new EnumValue($1, $3); }
    | IDENTIFIER '=' IDENTIFIER { $$ = new EnumValue($1, $3); }
    | IDENTIFIER '=' IDENTIFIER '+' INTEGER
      {
          $$ = new EnumValue(
                  $1, strdup(
                      android::String8::format("%s + %s", $3, $5).string()));
      }
    ;

enum_values
    : /* empty */
      {
          $$ = new std::vector<EnumValue *>;
      }
    | enum_value
      {
          $$ = new std::vector<EnumValue *>;
          $$->push_back($1);
      }
    | enum_values ',' enum_value
      {
          $$ = $1;
          $$->push_back($3);
      }
    ;

type
    : fqname { $$ = $1; }
    | fqname '[' INTEGER ']' { $$ = new ArrayType($1, $3); }
    | VEC '<' fqname '>' { $$ = new VectorType($3); }
    | struct_or_union_declaration { $$ = $1; }
    | enum_declaration { $$ = $1; }
    ;

optIdentifier
    : /* empty */ { $$ = NULL; }
    | IDENTIFIER { $$ = $1; }
    ;

%%
