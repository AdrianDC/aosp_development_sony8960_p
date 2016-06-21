%{
#include <string.h>
#include <stdlib.h>

#include "ast.h"
#include "hidl_language_y.h"

#define YY_USER_ACTION yylloc->columns(yyleng);
%}

%option yylineno
%option noyywrap
%option reentrant
%option bison-bridge
%option bison-locations

%x LONG_COMMENT LINE_COMMENT STRING

identifier  [_a-zA-Z]([_a-zA-Z0-9]|\:\:)*
whitespace  ([ \t\r]+)
intvalue    [-+]?(0[0-7']*|[1-9][0-9']*|0[xX][0-9a-fA-F']+|0[bB][01']+)((l|L|ll|LL)?(u|U)?|(u|U)?(l|L|ll|LL)?)

%%
%{
  /* This happens at every call to yylex (every time we receive one token) */
  std::string extra_text;
  yylloc->step();
%}


\/\*                  { extra_text = yytext; BEGIN(LONG_COMMENT); }
<LONG_COMMENT>\*+\/   { extra_text += yytext;
		        yylval->element = new CommentElement(extra_text, "",
		          yylloc->begin.line);
		      	yylloc->step(); BEGIN(INITIAL); return yy::parser::token::COMMENT; }
<LONG_COMMENT>\*+     { extra_text += yytext; }
<LONG_COMMENT>\n+     { extra_text += yytext; yylloc->lines(yyleng); }
<LONG_COMMENT>[^*\n]+ { extra_text += yytext; }


\"([^\"]|\\\")*\"     { yylval->element = new StringElement(yytext,
		          yylloc->begin.line);
                        return yy::parser::token::C_STR; }

\'([^\\']|\\.)\'      { yylval->element = new IntElement(yytext, extra_text,
		          		yylloc->begin.line);
                        return yy::parser::token::C_CHAR; }

\/\/.*\n              { yylval->element = new CommentElement(yytext, "",
		            yylloc->begin.line);
			yylloc->lines(1);
		        yylloc->step();
			return yy::parser::token::COMMENT; }

\n+                   { yylloc->lines(yyleng); yylloc->step(); }
{whitespace}          {}
<<EOF>>               { yyterminate(); }

    /* symbols */
;                     { return ';'; }
:                     { return ':'; }
\{                    { return '{'; }
\}                    { return '}'; }
=                     { return '='; }
,                     { return ','; }
@                     { return '@'; }
\.                    { return '.'; }
\(                    { return '('; }
\)                    { return ')'; }
\[                    { return '['; }
\]                    { return ']'; }
\<                    { return '<'; }
\>                    { return '>'; }

    /* keywords */
const                 { return yy::parser::token::CONST; }
oneway                { return yy::parser::token::ONEWAY; }
struct                { return yy::parser::token::STRUCT; }
union                 { return yy::parser::token::UNION; }
enum                  { return yy::parser::token::ENUM; }
typedef               { return yy::parser::token::TYPEDEF; }
version               { return yy::parser::token::VERSION; }
package               { return yy::parser::token::PACKAGE; }
interface             { return yy::parser::token::INTERFACE; }
generates             { return yy::parser::token::GENERATES; }
selects               { return yy::parser::token::SELECTS; }
import                { return yy::parser::token::IMPORT; }
ref                   { return yy::parser::token::REF; }
vec                   { return yy::parser::token::VEC; }
on                    { return yy::parser::token::ON; }

int8_t                { yylval->type = new ScalarType(new NameElement(yytext, extra_text,
		          yylloc->begin.line)); return yy::parser::token::INT8; }
int16_t               { yylval->type = new ScalarType(new NameElement(yytext, extra_text,
		          yylloc->begin.line)); return yy::parser::token::INT16; }
int32_t               { yylval->type = new ScalarType(new NameElement(yytext, extra_text,
		          yylloc->begin.line)); return yy::parser::token::INT32; }
int64_t               { yylval->type = new ScalarType(new NameElement(yytext, extra_text,
		          yylloc->begin.line)); return yy::parser::token::INT64; }
uint8_t               { yylval->type = new ScalarType(new NameElement(yytext, extra_text,
		          yylloc->begin.line)); return yy::parser::token::UINT8; }
uint16_t              { yylval->type = new ScalarType(new NameElement(yytext, extra_text,
		          yylloc->begin.line)); return yy::parser::token::UINT16; }
uint32_t              { yylval->type = new ScalarType(new NameElement(yytext, extra_text,
		          yylloc->begin.line)); return yy::parser::token::UINT32; }
uint64_t              { yylval->type = new ScalarType(new NameElement(yytext, extra_text,
		          yylloc->begin.line)); return yy::parser::token::UINT64; }
char                  { yylval->type = new ScalarType(new NameElement(yytext, extra_text,
		          yylloc->begin.line)); return yy::parser::token::CHAR; }
opaque                { yylval->type = new ScalarType(new NameElement(yytext, extra_text,
		          yylloc->begin.line)); return yy::parser::token::OPAQUE; }
handle                { yylval->type = new HandleType(); return yy::parser::token::HANDLE; }
string                { yylval->type = new StringType(); return yy::parser::token::STRINGTOK; }


    /* scalars */
{identifier}          { yylval->element = new NameElement(yytext, extra_text,
		          yylloc->begin.line);
                        return yy::parser::token::IDENTIFIER;
                      }
{intvalue}            { yylval->element = new IntElement(yytext, extra_text,
		          yylloc->begin.line);
                        return yy::parser::token::INTVALUE; }

    /* syntax Error! */
.                     { printf("UNKNOWN(%s)", yytext);
                        yylval->element = new ErrorElement(yytext, extra_text, yylloc->begin.line);
                        return yy::parser::token::IDENTIFIER;
                      }

%%

// comment and whitespace handling
// ================================================
