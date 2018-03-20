#include <stdio.h>

#define NRW        21     // number of reserved words
#define TXMAX      500    // length of identifier table
#define MAXNUMLEN  14     // maximum number of digits in numbers
#define NSYM       21     // maximum number of symbols in array ssym and csym
#define MAXIDLEN   10     // length of identifiers

#define MAXADDRESS 32767  // maximum address
#define MAXLEVEL   32     // maximum depth of nesting block
#define CXMAX      500    // size of code array

#define MAXSYM     30     // maximum number of symbols

#define STACKSIZE  1000   // maximum storage

enum symtype
{
	SYM_NULL,
	SYM_IDENTIFIER,
	SYM_NUMBER,
	SYM_PLUS,
	SYM_MINUS,
	SYM_TIMES,	//5
	SYM_SLASH,
	SYM_ODD,
	SYM_EQU,
	SYM_NEQ,
	SYM_LES,	//10
	SYM_LEQ,
	SYM_GTR,
	SYM_GEQ,
	SYM_LPAREN,
	SYM_RPAREN,	//15
	SYM_COMMA,
	SYM_SEMICOLON,
	SYM_PERIOD,
	SYM_COLON,
	SYM_BECOMES, //20
    SYM_BEGIN,
	SYM_END,
	SYM_IF,
	SYM_THEN,
	SYM_WHILE,	//25
	SYM_DO,
	SYM_CONST,
	SYM_VAR,
	SYM_FUNCTION,

	SYM_AND,	//&&
	SYM_OR,		//||
	SYM_NOT,	//!				//2017.9.25
	SYM_LSQURBRA, //[
	SYM_RSQURBRA, //]
	SYM_BIT_AND, //&
	SYM_BIT_OR,	//|
	SYM_BIT_XOR,//^
	SYM_MOD,	//%
	SYM_SHL,	//<<
	SYM_SHR,	//>>			//2017.10.19
	SYM_INC,	//++
	SYM_DEC,	//--			//2017.10.29
	SYM_QUESTION,//?

	SYM_BIT_AND_BEC, //&=
	SYM_BIT_OR_BEC,	///=
	SYM_BIT_XOR_BEC,//^=
	SYM_PLUS_BEC,	//+=
	SYM_MINUS_BEC,	//-=
	SYM_TIMES_BEC,	//*=
	SYM_SLASH_BEC,	///=
	SYM_MOD_BEC,	//%=
	SYM_SHL_BEC, 	//<<=
    SYM_SHR_BEC,	//>>=		//2017.10.29

	SYM_ELSE,
	SYM_ELIF,
	SYM_EXIT,
	SYM_RETURN,
	SYM_FOR,						//2017.10.9
	SYM_GOTO,
	SYM_BREAK,
	SYM_CONTINUE,

	SYM_PRINT,
	SYM_RANDOM,
	SYM_CALLSTACK
};

enum idtype
{
	ID_CONSTANT, ID_VARIABLE, ID_FUNCTION , ID_PVARIABLE , ID_DIM, ID_LABEL , ID_PDIM , ID_PFUNCTION//ID_PROCEDURE
};

enum opcode
{
	LIT, OPR, LOD, STO, CAL, INT, JMP, JPC,
	POP , LOA , STA , LAD , CPY, RAN, CAS , CAP
};

enum oprcode
{
	OPR_RET, OPR_NEG, OPR_ADD, OPR_MIN,
	OPR_MUL, OPR_DIV, OPR_ODD, OPR_EQU,
	OPR_NEQ, OPR_LES, OPR_LEQ, OPR_GTR,
	OPR_GEQ, OPR_AND, OPR_OR,  OPR_NOT,
	OPR_MOD, OPR_BIT_XOR, OPR_BIT_AND, OPR_BIT_OR,
	OPR_SHL, OPR_SHR, OPR_LEAVE
};


typedef struct
{
	int f; // function code
	int l; // level
	int a; // displacement address
} instruction;

//////////////////////////////////////////////////////////////////////
char* err_msg[] =
{
/*  0 */    "",
/*  1 */    "Found ':=' when expecting '='.",
/*  2 */    "There must be a number to follow '='.",
/*  3 */    "There must be an '=' to follow the identifier.",
/*  4 */    "There must be an identifier to follow 'const', 'var', or 'procedure'.",
/*  5 */    "Missing ',' or ';'.",
/*  6 */    "Incorrect procedure name.",
/*  7 */    "Statement expected.",
/*  8 */    "Follow the statement is an incorrect symbol.",
/*  9 */    "'.' expected.",
/* 10 */    "';' expected.",
/* 11 */    "Undeclared identifier.",
/* 12 */    "Illegal assignment.",
/* 13 */    "':=' expected.",
/* 14 */    "There must be an label to follow the 'goto'.",
/* 15 */    "A constant or variable can not be called.",
/* 16 */    "'then' expected.",
/* 17 */    "';' or 'end' expected.",
/* 18 */    "'do' expected.",
/* 19 */    "Incorrect symbol.",
/* 20 */    "Relative operators expected.",
/* 21 */    "Procedure identifier can not be in an expression.",
/* 22 */    "Missing ')'.",
/* 23 */    "The symbol can not be followed by a factor.",
/* 24 */    "The symbol can not be as the beginning of an expression.",
/* 25 */    "The number is too great.",
/* 26 */    "Operand must be L-value.",
/* 27 */    "Missing '('.",
/* 28 */    "Not in a loop.",
/* 29 */    "':' expected.",
/* 30 */    "",
/* 31 */    "",
/* 32 */    "",
/* 33 */    "There are too many levels."
};

//////////////////////////////////////////////////////////////////////
char ch;         // last character read
int  sym;        // last symbol read
char id[MAXIDLEN + 1]; // last identifier read
int  num;        // last number read
int  cc;         // character count
int  ll;         // line length
int  kk;
int  err;
int  cx;         // index of current instruction to be generated.
int  level = 0;
int  tx = 0;
int  cx_0[MAXIDLEN + 1];//save cx for goto loop
int  cflag = 0, bflag = 0, lflag=0 ;
int  cx3, cx4;
char line[80];

instruction code[CXMAX];

char* word[NRW + 1] =
{
	"", /* place holder */
	"begin", "function", "const", "do", "end","if",
	"odd", "then", "var", "while", "else", "elif",
	"exit",	"return", "for", "random", "print", "callstack",
	"goto", "break", "continue"
};

int wsym[NRW + 1] =
{
	SYM_NULL, SYM_BEGIN, SYM_FUNCTION, SYM_CONST, SYM_DO, SYM_END,
	SYM_IF, SYM_ODD, SYM_THEN, SYM_VAR, SYM_WHILE,
	SYM_ELSE, SYM_ELIF, SYM_EXIT, SYM_RETURN, SYM_FOR,
	SYM_RANDOM, SYM_PRINT, SYM_CALLSTACK, SYM_GOTO,
	SYM_BREAK, SYM_CONTINUE
};

int ssym[NSYM + 1] =
{
	SYM_NULL, SYM_PLUS, SYM_MINUS, SYM_TIMES, SYM_SLASH,
	SYM_LPAREN, SYM_RPAREN, SYM_EQU, SYM_COMMA, SYM_PERIOD,
	SYM_SEMICOLON, SYM_LSQURBRA, SYM_RSQURBRA, SYM_BIT_AND,
	SYM_BIT_OR, SYM_NOT, SYM_MOD, SYM_BIT_XOR, SYM_BEGIN,
	SYM_END, SYM_QUESTION, SYM_COLON
};

char csym[NSYM + 1] =
{
	' ', '+', '-', '*', '/', '(', ')', '=', ',', '.', ';', '[', ']',
	'&', '|', '!', '%', '^', '{', '}', '?', ':'
};

#define MAXINS   18
char* mnemonic[MAXINS] =
{
	"LIT", "OPR", "LOD", "STO", "CAL", "INT", "JMP", "JPC",
	"POP" ,"LOA" ,"STA" ,"LAD" , "CPY", "RAN", "CAS" , "CAP"
};

typedef struct
{
	char name[MAXIDLEN + 1];
	int  kind;
	int  value;
} comtab;

comtab table[TXMAX];

typedef struct
{
	char  name[MAXIDLEN + 1];
	int   kind;
	short level;
	short address;
} mask;

FILE* infile;

typedef struct
{
 char name[MAXIDLEN+1];
 int c ;
 char var[40][MAXIDLEN+1];
 int kind[MAXIDLEN+1];
}funct;

typedef struct
{
    char name[MAXIDLEN+1];
    int c ;
    int l[100];
}dim;

dim d[40];

int dimx = 0;

funct f[40];

int fx = 0;
int afx = 0 ;
// EOF PL0.h
