%{

#include "Annotation.h"
#include "AST.h"
#include "ArrayType.h"
#include "CompoundType.h"
#include "ConstantExpression.h"
#include "EnumType.h"
#include "GenericBinder.h"
#include "Interface.h"
#include "Method.h"
#include "VectorType.h"

#include "hidl-gen_y.h"

#include <stdio.h>
#include <utils/String8.h>

using namespace android;

extern int yylex(yy::parser::semantic_type *, yy::parser::location_type *, void *);

#define scanner ast->scanner()

%}

%initial-action {
    // Initialize the initial location.
    @$.begin.filename = @$.end.filename =
        const_cast<std::string *>(&ast->getFilename());
}

%parse-param { android::AST *ast }
%lex-param { void *scanner }
%pure-parser
%skeleton "glr.cc"

%token<str> ENUM
%token<str> EXTENDS
%token<str> FQNAME
%token<str> GENERATES
%token<str> IDENTIFIER
%token<str> IMPORT
%token<str> INTEGER
%token<str> FLOAT
%token<str> INTERFACE
%token<str> PACKAGE
%token<type> SCALAR
%token<str> STRUCT
%token<str> STRING_LITERAL
%token<str> TYPEDEF
%token<str> UNION
%token<str> VEC
%token<void> ONEWAY

/* Operator precedence and associativity, as per
 * http://en.cppreference.com/w/cpp/language/operator_precedence */
/* Precedence level 15 ternary operator */
%right '?' ':'
/* Precedence level 13 - 14, LTR, logical operators*/
%left LOGICAL_OR
%left LOGICAL_AND
/* Precedence level 10 - 12, LTR, bitwise operators*/
%left '|'
%left '^'
%left '&'
/* Precedence level 9, LTR */
%left EQUALITY NEQ
/* Precedence level 8, LTR */
%left '<' '>' LEQ GEQ
/* Precedence level 7, LTR */
%left LSHIFT RSHIFT
/* Precedence level 6, LTR */
%left '+' '-'
/* Precedence level 5, LTR */
%left '*' '/' '%'
/* Precedence level 3, RTL; but we have to use %left here */
%left UNARY_MINUS UNARY_PLUS '!' '~'

%type<str> optIdentifier package
%type<type> fqname

%type<type> type opt_storage_type
%type<type> enum_declaration
%type<type> struct_or_union_declaration
%type<type> opt_extends

%type<field> field_declaration
%type<fields> field_declarations struct_or_union_body
%type<constantExpression> const_expr
%type<enumValue> enum_value
%type<enumValues> enum_values
%type<typedVars> typed_vars
%type<typedVar> typed_var
%type<method> method_declaration
%type<compoundStyle> struct_or_union_keyword
%type<stringVec> annotation_string_values annotation_value
%type<annotationParam> annotation_param
%type<annotationParams> opt_annotation_params annotation_params
%type<annotation> annotation
%type<annotations> opt_annotations

%start program

%union {
    const char *str;
    android::Type *type;
    android::CompoundType *compoundType;
    android::CompoundField *field;
    std::vector<android::CompoundField *> *fields;
    android::EnumValue *enumValue;
    android::ConstantExpression *constantExpression;
    std::vector<android::EnumValue *> *enumValues;
    android::TypedVar *typedVar;
    std::vector<android::TypedVar *> *typedVars;
    android::Method *method;
    android::CompoundType::Style compoundStyle;
    std::vector<std::string> *stringVec;
    std::pair<std::string, std::vector<std::string> *> *annotationParam;
    android::DefaultKeyedVector<std::string, std::vector<std::string> *> *annotationParams;
    android::Annotation *annotation;
    android::DefaultKeyedVector<std::string, android::Annotation *> *annotations;
}

%%

opt_annotations
    : /* empty */
      {
          $$ = new DefaultKeyedVector<std::string, Annotation *>;
      }
    | opt_annotations annotation
      {
          $$ = $1;
          $$->add($2->name(), $2);
      }
    ;

annotation
    : '@' IDENTIFIER opt_annotation_params
      {
          $$ = new Annotation($2, $3);
      }
    ;

opt_annotation_params
    : /* empty */
      {
          $$ = new DefaultKeyedVector<std::string, std::vector<std::string> *>;
      }
    | '(' annotation_params ')'
      {
          $$ = $2;
      }
    ;

annotation_params
    : annotation_param
      {
          $$ = new DefaultKeyedVector<std::string, std::vector<std::string> *>;
          $$->add($1->first, $1->second);
      }
    | annotation_params ',' annotation_param
      {
          $$ = $1;
          $$->add($3->first, $3->second);
      }
    ;

annotation_param
    : IDENTIFIER '=' annotation_value
      {
          $$ = new std::pair<std::string, std::vector<std::string> *>($1, $3);
      }
    ;

annotation_value
    : STRING_LITERAL
      {
          $$ = new std::vector<std::string>;
          $$->push_back($1);
      }
    | '{' annotation_string_values '}' { $$ = $2; }
    ;

annotation_string_values
    : STRING_LITERAL
      {
          $$ = new std::vector<std::string>;
          $$->push_back($1);
      }
    | annotation_string_values ',' STRING_LITERAL
      {
          $$ = $1;
          $$->push_back($3);
      }
    ;

program
    : package imports body
    ;

fqname
    : FQNAME
      {
          $$ = ast->lookupType($1);
          if ($$ == NULL) {
              std::cerr << "ERROR: Failed to lookup type '" << $1 << "' at "
                        << @1
                        << "\n";

              YYERROR;
          }
      }
    | IDENTIFIER
      {
          $$ = ast->lookupType($1);
          if ($$ == NULL) {
              std::cerr << "Failed to lookup type '" << $1 << "' at " << @1
                        << "\n";

              YYERROR;
          }
      }
    | SCALAR
    ;

package
    : PACKAGE FQNAME ';'
      {
          if (!ast->setPackage($2)) {
              std::cerr << "ERROR: Malformed package identifier '"
                        << $2
                        << "' at "
                        << @2
                        << "\n";

              YYERROR;
          }
      }

imports
    : /* empty */
    | imports IMPORT FQNAME ';'
      {
          if (!ast->addImport($3)) {
              std::cerr << "ERROR: Unable to import '" << $3 << "' at " << @3
                        << "\n";

              YYERROR;
          }
      }
    | imports IMPORT IDENTIFIER ';'
      {
          if (!ast->addImport($3)) {
              std::cerr << "ERROR: Unable to import '" << $3 << "' at " << @3
                        << "\n";

              YYERROR;
          }
      }
    ;

opt_extends
    : /* empty */ { $$ = NULL; }
    | EXTENDS fqname { $$ = $2; }

body
    : opt_annotations INTERFACE IDENTIFIER opt_extends
      {
          if ($4 != NULL && !$4->isInterface()) {
              std::cerr << "ERROR: You can only extend interfaces. at" << @4
                        << "\n";

              YYERROR;
          }

          if ($3[0] != 'I') {
              std::cerr << "ERROR: All interface names must start with an 'I' "
                        << "prefix. at " << @3 << "\n";

              YYERROR;
          }

          Interface *iface = new Interface($3, static_cast<Interface *>($4), $1);

          // Register interface immediately so it can be referenced inside
          // definition.
          std::string errorMsg;
          if (!ast->addScopedType(iface, &errorMsg)) {
              std::cerr << "ERROR: " << errorMsg << " at " << @3 << "\n";
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
    ;

typedef_declaration
    : TYPEDEF type IDENTIFIER
      {
          std::string errorMsg;
          if (!ast->addTypeDef($3, $2, &errorMsg)) {
              std::cerr << "ERROR: " << errorMsg << " at " << @3 << "\n";
              YYERROR;
          }
      }
    ;

const_expr
    : INTEGER                   { $$ = new ConstantExpression($1); }
    | IDENTIFIER                { $$ = new ConstantExpression($1); }
    | const_expr '?' const_expr ':' const_expr
      {
        $$ = new ConstantExpression($1, $3, $5);
      }
    | const_expr LOGICAL_OR const_expr  { $$ = new ConstantExpression($1, "||", $3); }
    | const_expr LOGICAL_AND const_expr { $$ = new ConstantExpression($1, "&&", $3); }
    | const_expr '|' const_expr { $$ = new ConstantExpression($1, "|" , $3); }
    | const_expr '^' const_expr { $$ = new ConstantExpression($1, "^" , $3); }
    | const_expr '&' const_expr { $$ = new ConstantExpression($1, "&" , $3); }
    | const_expr EQUALITY const_expr { $$ = new ConstantExpression($1, "==", $3); }
    | const_expr NEQ const_expr { $$ = new ConstantExpression($1, "!=", $3); }
    | const_expr '<' const_expr { $$ = new ConstantExpression($1, "<" , $3); }
    | const_expr '>' const_expr { $$ = new ConstantExpression($1, ">" , $3); }
    | const_expr LEQ const_expr { $$ = new ConstantExpression($1, "<=", $3); }
    | const_expr GEQ const_expr { $$ = new ConstantExpression($1, ">=", $3); }
    | const_expr LSHIFT const_expr { $$ = new ConstantExpression($1, "<<", $3); }
    | const_expr RSHIFT const_expr { $$ = new ConstantExpression($1, ">>", $3); }
    | const_expr '+' const_expr { $$ = new ConstantExpression($1, "+" , $3); }
    | const_expr '-' const_expr { $$ = new ConstantExpression($1, "-" , $3); }
    | const_expr '*' const_expr { $$ = new ConstantExpression($1, "*" , $3); }
    | const_expr '/' const_expr { $$ = new ConstantExpression($1, "/" , $3); }
    | const_expr '%' const_expr { $$ = new ConstantExpression($1, "%" , $3); }
    | '+' const_expr %prec UNARY_PLUS  { $$ = new ConstantExpression("+", $2); }
    | '-' const_expr %prec UNARY_MINUS { $$ = new ConstantExpression("-", $2); }
    | '!' const_expr { $$ = new ConstantExpression("!", $2); }
    | '~' const_expr { $$ = new ConstantExpression("~", $2); }
    | '(' const_expr ')' { $$ = $2; }
    ;

method_declaration
    : opt_annotations IDENTIFIER '(' typed_vars ')' ';'
      {
          $$ = new Method($2, $4, new std::vector<TypedVar *>, false, $1);
      }
    | opt_annotations ONEWAY IDENTIFIER '(' typed_vars ')' ';'
      {
          $$ = new Method($3, $5, new std::vector<TypedVar *>, true, $1);
      }
    | opt_annotations IDENTIFIER '(' typed_vars ')' GENERATES '(' typed_vars ')' ';'
      {
          $$ = new Method($2, $4, $8, false, $1);
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
          CompoundType *container = new CompoundType($1, $2);
          ast->enterScope(container);
      }
      struct_or_union_body
      {
          CompoundType *container = static_cast<CompoundType *>(ast->scope());

          std::string errorMsg;
          if (!container->setFields($4, &errorMsg)) {
              std::cerr << "ERROR: " << errorMsg << " at " << @4 << "\n";
              YYERROR;
          }

          ast->leaveScope();

          if (!ast->addScopedType(container, &errorMsg)) {
              std::cerr << "ERROR: " << errorMsg << " at " << @2 << "\n";
              YYERROR;
          }
      }
    ;

struct_or_union_declaration
    : struct_or_union_keyword optIdentifier
      {
          const char *localName = $2;
          std::string anonName;
          if (localName == nullptr) {
              anonName = ast->scope()->pickUniqueAnonymousName();
              localName = anonName.c_str();
          }

          CompoundType *container = new CompoundType($1, localName);
          ast->enterScope(container);
      }
      struct_or_union_body
      {
          CompoundType *container = static_cast<CompoundType *>(ast->scope());

          std::string errorMsg;
          if (!container->setFields($4, &errorMsg)) {
              std::cerr << "ERROR: " << errorMsg << " at " << @4 << "\n";
              YYERROR;
          }

          ast->leaveScope();

          if (!ast->addScopedType(container, &errorMsg)) {
              std::cerr << "ERROR: " << errorMsg << " at " << @2 << "\n";
              YYERROR;
          }

          $$ = container->ref();
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
    | ':' fqname
      {
          $$ = $2;

          if ($$ != NULL && !$$->isValidEnumStorageType()) {
              std::cerr << "ERROR: Invalid enum storage type specified. at "
                        << @2 << "\n";

              YYABORT;
          }
      }
    ;

opt_comma
    : /* empty */
    | ','
    ;

named_enum_declaration
    : ENUM IDENTIFIER opt_storage_type '{' enum_values opt_comma '}'
      {
          EnumType *enumType = new EnumType($2, $5, $3);

          std::string errorMsg;
          if (!ast->addScopedType(enumType, &errorMsg)) {
              std::cerr << "ERROR: " << errorMsg << " at " << @2 << "\n";
              YYERROR;
          }
      }
    ;

enum_declaration
    : ENUM '{' enum_values opt_comma '}'
      {
          std::string anonName = ast->scope()->pickUniqueAnonymousName();

          EnumType *enumType = new EnumType(anonName.c_str(), $3);

          std::string errorMsg;
          if (!ast->addScopedType(enumType, &errorMsg)) {
              // This should never fail.
              std::cerr << "ERROR: " << errorMsg << "\n";
              YYERROR;
          }

          $$ = enumType->ref();
      }
    | ENUM IDENTIFIER opt_storage_type '{' enum_values opt_comma '}'
      {
          EnumType *enumType = new EnumType($2, $5, $3);

          std::string errorMsg;
          if (!ast->addScopedType(enumType, &errorMsg)) {
              std::cerr << "ERROR: " << errorMsg << " at " << @2 << "\n";
              YYERROR;
          }

          $$ = enumType->ref();
      }
    ;

enum_value
    : IDENTIFIER { $$ = new EnumValue($1); }
    | IDENTIFIER '=' const_expr { $$ = new EnumValue($1, $3->value()); }
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
    | fqname '[' INTEGER ']'
      {
          if ($1->isBinder()) {
              std::cerr << "ERROR: Arrays of interface types are not supported."
                        << " at " << @1 << "\n";

              YYERROR;
          }

          $$ = new ArrayType($1, $3);
      }
    | VEC '<' fqname '>'
      {
          if ($3->isBinder()) {
              std::cerr << "ERROR: Vectors of interface types are not "
                        << "supported. at " << @3 << "\n";

              YYERROR;
          }

          $$ = new VectorType($3);
      }
    | struct_or_union_declaration { $$ = $1; }
    | enum_declaration { $$ = $1; }
    | INTERFACE { $$ = new GenericBinder; }
    ;

optIdentifier
    : /* empty */ { $$ = NULL; }
    | IDENTIFIER { $$ = $1; }
    ;

%%

#include <android-base/logging.h>

void yy::parser::error(
        const yy::parser::location_type &where,
        const std::string &errstr) {
    std::cerr << "ERROR: " << errstr << " at " << where << "\n";
}

