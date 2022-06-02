/*
 *  The scanner definition for COOL.
 */

/*
 *  Stuff enclosed in %{ %} in the first section is copied verbatim to the
 *  output, so headers and global definitions are placed here to be visible
 * to the code in the file.  Don't remove anything that was here initially
 */
%{
#include <cool-parse.h>
#include <stringtab.h>
#include <utilities.h>

/* The compiler assumes these identifiers. */
#define yylval cool_yylval
#define yylex  cool_yylex

/* Max size of string constants */
#define MAX_STR_CONST 1025
#define YY_NO_UNPUT   /* keep g++ happy */

extern FILE *fin; /* we read from this file */

/* define YY_INPUT so we read from the FILE fin:
 * This change makes it possible to use this scanner in
 * the Cool compiler.
 */
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
	if ( (result = fread( (char*)buf, sizeof(char), max_size, fin)) < 0) \
		YY_FATAL_ERROR( "read() in flex scanner failed");

char string_buf[MAX_STR_CONST]; /* to assemble string constants */
char *string_buf_ptr;

extern int curr_lineno;
extern int verbose_flag;

extern YYSTYPE cool_yylval;

/*
 *  Add Your own definitions here
 */

%}

/*
 * Define names for regular expressions here.
 */
class           [Cc][Ll][Aa][Ss]
else            [Ee][Ll][Ss][Ee]
fi              [Ff][Ii]
if              [Ii][Ff]
in              [Ii][Nn]
inherits        [Ii][Nn][Hh][Ee][Rr][Ii][Tt][Ss]
let             [Ll][Ee][Tt]
loop            [Ll][Oo][Oo][Pp]
pool            [Pp][Oo][Oo][Ll]
then            [Tt][Hh][Ee][Nn]
while           [Ww][Hh][Ii][Ll][Ee]
case            [Cc][Aa][Ss][Ee]
esac            [Ee][Ss][Aa][Cc]
of              [Oo][Ff]
darrow         =>
new             [Nn][Ee][Ww]
isvoid          [Ii][Ss][Vv][Oo][Ii][Dd]

str_const 
int_const
bool_const 

typeid
objectid
assign          <-
not             [Nn][Oo][Tt]
le              <=
error



digits         [0-9]+
words          [a-zA-Z]+



%%

 /*
  *  Nested comments
  */


 /*
  *  The multiple-character operators.
  */
{class}      { return CLASS; }
{else}       { return ELSE; }
{fi}         { return FI; }
{if}         { return IF; }
{in}         { return IN; }
{inherits}   { return INHERITS; }
{let}        { return LET; }
{loop}       { return LOOP; }
{pool}       { return POOL; }
{then}       { return THEN; }
{while}      { return WHILE; }
{case}       { return CASE; }
{esac}       { return ESAC; }
{of}         { return OF; }
{darrow}		 { return DARROW; }
{new}        { return NEW; }
{isvoid}     { return ISVOID; }


 /*
  * Keywords are case-insensitive except for the values true and false,
  * which must begin with a lower-case letter.
  */


 /*
  *  String constants (C syntax)
  *  Escape sequence \c is accepted for all characters c. Except for 
  *  \n \t \b \f, the result is c.
  *
  */


%%

int main(void) {
  yylex();
  return 0;
}