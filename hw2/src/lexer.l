%option noyywrap 
%{
#include <stdio.h>
#include "header.h" 
int linenumber;
symtab * lookup();
symtab * ptr;
void     insertID();
void 	 printFreq();
/* You need to define for all tokens in C--, here are some examples */
#define ERROR 		100 
#define INT		10
#define FLOAT 		11 
#define OP_ASSIGN 	12 
#define OP_OR 		13 
#define MK_LPAREN 	14 
#define MK_RPAREN 	15 
#define CON_INT		16
#define CON_FLO		17
#define CON_STR		18
#define RETURN		19
#define ELSE		20
#define FOR		21
#define IF		22
#define STRUCT		23
#define TYPEDEF		24
#define UNION		25
#define VOID		26
#define WHILE		27
#define OP_AND		28
#define OP_NOT		29
#define OP_ADD		30
#define OP_SUB		31
#define OP_MUL		32
#define OP_DIV		33
#define OP_les		34
#define OP_gre		35
#define OP_lese		36
#define OP_gree		37
#define OP_equ		38
#define OP_nequ		39
#define MK_LBRACE	40
#define MK_RBRACE	41
#define MK_LBRACK	42
#define MK_RBRACK	43
#define MK_COMMA	44
#define MK_SEMICOL	45
#define MK_DOL		46

%}

letter   [A-Za-z]
digit    [0-9]
ID	 {letter}({letter}|{digit}|"_")*
WS	 [ \t]+
Int_constant 	{digit}+
Float_constant (({digit}*\.{digit}+|{digit}+\.)([eE][+-]?[0-9]+)?[flFL]?)|([0-9]+[eE][+-]?[0-9]+[flFL]?)
String_constant	\"([^\\\"]|\\.)*\"
comment		"/*"([^*]|\*+[^*/])*\*+"/"
RWfloat		"float"
RWint		"int"
RWret 		"return"
RWelse		"else"
RWfor		"for"
RWif 		"if"
RWstruct	"struct"
RWtypedef	"typedef"
RWunion		"union"
RWvoid		"void"
RWwhile		"while"




/* You need to define the following RE's 
Float_constant
String_constant	
comment
*/

OP_assign        "="
OP_or   	"||"
OP_and   	"&&"
OP_not		"!"
OP_add		"+"
OP_sub		"-"
OP_mul		"*"
OP_div		"/"
OP_les		"<"
OP_gre		">"
OP_lese		"<="
OP_gree		">="
OP_equ		"=="
OP_nequ		"!="
/* Other operators appear here */ 


newline  "\n"

DL_lparen "("
DL_rparen ")"
DL_lbrace "{"
DL_rbrace "}"
DL_lbrack "["
DL_rbrack "]"
DL_comma  ","
DL_semicol ";"
DL_dot  "."

/* Other separators appear here */

error    .

%%





{comment}	{printf("%s\n", yytext);}
{WS}            {}
{RWint}         {/* return INT; */ }
{RWfloat}       {/* return FLOAT; */}
{RWret} 	{/* return RETURN; */}
{RWelse}	{/*return ELSE; */}
{RWfor}		/* return FOR; */
{RWif} 		
{RWstruct}	
{RWtypedef}	
{RWunion}	
{RWvoid}	
{RWwhile}	
{Int_constant}	/*return CON_INT*/
{Float_constant}  /* return CON_FLO; */
{String_constant} /* return CON_STR; */
{ID}            { 
			ptr = lookup(yytext);
			if (ptr == NULL)
			     insertID(yytext);	
			else 
			     ptr->counter++;
		}

{OP_assign}     /* return OP_ASSIGN; */ 
{OP_or}         /* return OP_OR; */
{OP_and}   	/* return OP_AND;*/
{OP_not}	/* return OP_NOT;*/
{OP_add}	
{OP_sub}	
{OP_mul}	
{OP_div}	
{OP_les}	
{OP_gre}	
{OP_lese}	
{OP_gree}	
{OP_equ}	
{OP_nequ}	
{DL_lparen}     /* return MK_LPAREN; */
{DL_rparen}     /* return MK_RPAREN; */
{DL_lbrace}     /* return MK_LBRACE; */
{DL_rbrace}     /* return MK_RBRACE; */
{DL_lbrack}
{DL_rbrack}
{DL_comma}      {}
{DL_semicol}    {}
{DL_dot}	{}
{newline}       linenumber += 1;
{error}         printf("ERR: %s\n", yytext);/*return ERROR; */


%%

int main(int argc, char **argv)
{
 argc--; ++argv;
   if (argc > 0)
      yyin = fopen(argv[0], "r");
   else
      yyin = stdin;
   yylex();
   //printSymTab();
   printFreq();
   return 0;
}

