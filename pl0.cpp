// pl0 compiler source code

#pragma warning(disable:4996)


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "PL0.h"
#include "set.c"

//////////////////////////////////////////////////////////////////////
// print error message.
void error(int n)
{
	int i;

	printf("      ");
	for (i = 1; i <= cc - 1; i++)
		printf(" ");
	printf("^\n");
	printf("Error %3d: %s\n", n, err_msg[n]);
	err++;
} // error

//////////////////////////////////////////////////////////////////////
void getch(void)
{
	if (cc == ll)
	{
		if (feof(infile))
		{
			printf("\nPROGRAM INCOMPLETE\n");
			exit(1);
		}
		ll = cc = 0;
		printf("%5d  ", cx);
		while ((!feof(infile)) // added & modified by alex 01-02-09
			&& ((ch = getc(infile)) != '\n'))
		{
			printf("%c", ch);
			line[++ll] = ch;
		} // while
		printf("\n");
		line[++ll] = ' ';
	}
	ch = line[++cc];
} // getch

//////////////////////////////////////////////////////////////////////
// gets a symbol from input stream.
int presym;//2017.12.15

void getsym(void)
{
	int i, k;
	char a[MAXIDLEN + 1];

	while (ch == ' ' || ch == '\t')
		getch();

	if (isalpha(ch))
	{ // symbol is a reserved word or an identifier.
		k = 0;
		do
		{
			if (k < MAXIDLEN)
				a[k++] = ch;
			getch();
		} while (isalpha(ch) || isdigit(ch));
		a[k] = 0;
		strcpy(id, a);
		word[0] = id;
		i = NRW;
		while (strcmp(id, word[i--]));
		if (++i)
			sym = wsym[i]; // symbol is a reserved word
		else
			sym = SYM_IDENTIFIER;   // symbol is an identifier
	}
	else if (isdigit(ch))
	{ // symbol is a number.
		k = num = 0;
		sym = SYM_NUMBER;
		do
		{
			num = num * 10 + ch - '0';
			k++;
			getch();
		} while (isdigit(ch));
		if (k > MAXNUMLEN)
			error(25);     // The number is too great.
	}
	else if (ch == ':')
	{
		getch();
		if (ch == '=')
		{
			sym = SYM_BECOMES; // :=
			getch();
		}
		else
		{
			sym = SYM_COLON;       // illegal?
		}
	}
	else if (ch == '>')
	{
		getch();
		if (ch == '=')
		{
			sym = SYM_GEQ;     // >=
			getch();
		}
		else if (ch == '>')
		{
			getch();
			if (ch == '=')
			{
				sym = SYM_SHR_BEC;  // >>=
				getch();
			}
			else
				sym = SYM_SHR;  // >>
		}
		else
			sym = SYM_GTR;     // >
	}
	else if (ch == '<')
	{
		getch();
		if (ch == '=')
		{
			sym = SYM_LEQ;     // <=
			getch();
		}
		else if (ch == '>')
		{
			sym = SYM_NEQ;     // <>
			getch();
		}
		else if (ch == '<')
		{
			getch();
			if (ch == '=')
			{
				sym = SYM_SHL_BEC;  // <<=
				getch();
			}
			else
				sym = SYM_SHL;  // <<

		}
		else
			sym = SYM_LES;     // <
	}
	else if (ch == '&')
	{
		getch();
		if (ch == '&')
		{
			sym = SYM_AND;     // &&
			getch();
		}
		else if (ch == '=')
		{
			sym = SYM_BIT_AND_BEC; // &=
			getch();
		}
		else
			sym = SYM_BIT_AND;    // &
	}
	else if (ch == '^')
	{
		getch();
		if (ch == '=')
		{
			sym = SYM_BIT_XOR_BEC; // ^=
			getch();
		}
		else
			sym = SYM_BIT_XOR;    // ^
	}
	else if (ch == '|')
	{
		getch();
		if (ch == '|')
		{
			sym = SYM_OR;      // ||
			getch();
		}
		else if (ch == '=')
		{
			sym = SYM_BIT_OR_BEC;  // |=
			getch();
		}
		else
			sym = SYM_BIT_OR;     // |
	}
	else if (ch == '+')
	{
		getch();
		if (ch == '+')
		{
			sym = SYM_INC;      // ++
			getch();
		}
		else if (ch == '=')
		{
			sym = SYM_PLUS_BEC;  // +=
			getch();
		}
		else
			sym = SYM_PLUS;     // +
	}
	else if (ch == '-')
	{
		getch();
		if (ch == '-')
		{
			sym = SYM_DEC;      // --
			getch();
		}
		else if (ch == '=')
		{
			sym = SYM_MINUS_BEC;  // -=
			getch();
		}
		else
			sym = SYM_MINUS;   // -
	}
	else if (ch == '*')
	{
		getch();
		if (ch == '=')
		{
			sym = SYM_TIMES_BEC;  // *=
			getch();
		}
		else
			sym = SYM_TIMES;   // *
	}
	else if (ch == '%')
	{
		getch();
		if (ch == '=')
		{
			sym = SYM_MOD_BEC;   // %=
			getch();
		}
		else
			sym = SYM_MOD;      // %
	}
	else if (ch == '=')
	{
		getch();
		if (ch == '=')
		{
			sym = SYM_EQU; // :=
			getch();
		}
		else
		{
			sym = SYM_NULL;       // illegal?
		}
	}
	//2017.9.25
	else if(ch == '/')
	{
		getch();
		if(ch == '/')	// // ...
		{
			while(cc != ll)
				getch();
			getsym();
		}
		else if(ch == '*') // /*...*/
		{
			getch();
			getch();
			while (1) {
				if (ch == '*') {
					getch();
					if (ch == '/')
					break;
				}
				getch();
			}
			getch();
			getsym();
			}
		else if (ch == '=')
			{
				sym = SYM_SLASH_BEC;   // /=
				getch();
			}
			else
				sym = SYM_SLASH;    // /
		}//2017.9.21
	else
	{ // other tokens
		i = NSYM;
		csym[0] = ch;
		while (csym[i--] != ch);
		if (++i)
		{
			sym = ssym[i];
			getch();
		}
		else
		{
			printf("Fatal Error: Unknown character.\n");
			exit(1);
		}
	}
	presym = sym;//2017.12.15
} // getsym

//////////////////////////////////////////////////////////////////////
// generates (assembles) an instruction.
void gen(int x, int y, int z)
{
	if (cx > CXMAX)
	{
		printf("Fatal Error: Program too long.\n");
		exit(1);
	}
	code[cx].f = x;
	code[cx].l = y;
	code[cx++].a = z;
} // gen

//////////////////////////////////////////////////////////////////////
// tests if error occurs and skips all symbols that do not belongs to s1 or s2.
void test(symset s1, symset s2, int n)
{
	symset s;

	if (!inset(sym, s1))
	{
		error(n);
		s = uniteset(s1, s2);
		while (!inset(sym, s))
			getsym();
		destroyset(s);
	}
} // test

//////////////////////////////////////////////////////////////////////
int dx;  // data allocation index
int offset;

// enter object(constant, variable or procedre) into table.
void enter(int kind)
{
	mask* mk;

	tx++;
	strcpy(table[tx].name, id);
	table[tx].kind = kind;
	switch (kind)
	{
	case ID_CONSTANT:
		if (num > MAXADDRESS)
		{
			error(25); // The number is too great.
			num = 0;
		}
		table[tx].value = num;
		break;
	case ID_VARIABLE:
		mk = (mask*)&table[tx];
		mk->level = level;
		mk->address = dx++;
		break;
    case ID_PVARIABLE:
		mk = (mask*)&table[tx];
		mk->level = 0;
		mk->address = offset++;
		break;
    case ID_PFUNCTION:
        mk = (mask*)&table[tx];
		mk->level = 0;
		mk->address = offset++;
		afx ++ ;
		strcpy(f[afx].name , id);
		f[afx].c = 0;
		break;
	case ID_FUNCTION:
		mk = (mask*)&table[tx];
		mk->address = cx;
		mk->level = level;
		fx ++ ;
		afx ++ ;
		strcpy(f[fx].name , id);
		f[fx].c = 0;
		break;
	case ID_LABEL:
		mk = (mask*)&table[tx];
		mk->level = level;
		mk->address = dx++;
		break;
	} // switch
} // enter

//////////////////////////////////////////////////////////////////////
// locates identifier in symbol table.
int position(char* id)
{
	int i;
	strcpy(table[0].name, id);
	i = tx + 1;
	while (strcmp(table[--i].name, id) != 0);
	return i;
} // position

int fposition(char *id)
{
    int i;
    strcpy(f[0].name , id);
    i = afx + 1;
    while (strcmp(f[--i].name, id) != 0);
	return i;
}

int dposition(char *id)
{
    int i;
    strcpy(d[0].name , id);
    i = dimx + 1;
    while (strcmp(d[--i].name, id) != 0);
	return i;
}

//////////////////////////////////////////////////////////////////////
void constdeclaration()
{
	if (sym == SYM_IDENTIFIER)
	{
		getsym();
		if (sym == SYM_EQU || sym == SYM_BECOMES)
		{
			if (sym == SYM_BECOMES)
				error(1); // Found ':=' when expecting '='.
			getsym();
			if (sym == SYM_NUMBER)
			{
				enter(ID_CONSTANT);
				getsym();
			}
			else
			{
				error(2); // There must be a number to follow '='.
			}
		}
		else
		{
			error(3); // There must be an '=' to follow the identifier.
		}
	}
	else	error(4);
	// There must be an identifier to follow 'const', 'var', or 'function'.
} // constdeclaration

//////////////////////////////////////////////////////////////////////
void vardeclaration(void)
{
	if (sym == SYM_IDENTIFIER)
	{
		enter(ID_VARIABLE);
		getsym();
		if(sym == SYM_LSQURBRA)
        {
            strcpy(d[++dimx].name,table[tx].name);
            int c = 0;
            offset = 1;
            do{
            getsym();
            if(sym == SYM_NUMBER)
            {
                c++;
                d[dimx].l[c] = num;
                offset *= num;
                getsym();
            }
            else if(sym == SYM_IDENTIFIER)
            {
                c++;
                int k = position(id);
                d[dimx].l[c] = table[k].value;
                offset *= table[k].value;
                getsym();
            }
            else error(28);
            if(sym == SYM_RSQURBRA)
                getsym();
            else  error(28);}
            while(sym == SYM_LSQURBRA);
            dx = dx +offset - 1;
            d[dimx].c = c ;
            table[tx].kind = ID_DIM;
        }
	}
	else
	{
		error(4); // There must be an identifier to follow 'const', 'var', or 'function'.
	}
} // vardeclaration

void pvardeclaration(void)
{
	if (sym == SYM_IDENTIFIER)
	{
		enter(ID_PVARIABLE);
		getsym();
		if(sym == SYM_LSQURBRA)
        {
            strcpy(d[++dimx].name,table[tx].name);
            int c = 0;
            offset = 1;
            do{
            getsym();
            if(sym == SYM_NUMBER)
            {
                c++;
                d[dimx].l[c] = num;
                offset *= num;
                getsym();
            }
            else if(sym == SYM_IDENTIFIER)
            {
                c++;
                int k = position(id);
                d[dimx].l[c] = table[k].value;
                offset *= table[k].value;
                getsym();
            }
            else error(28);
            if(sym == SYM_RSQURBRA)
                getsym();
            else  error(28);}
            while(sym == SYM_LSQURBRA);
            dx = dx +offset - 1;
            d[dimx].c = c ;
            table[tx].kind = ID_PDIM;
            //offset ++ ;
        }
	}
	else
	{
		error(4); // There must be an identifier to follow 'const', 'var', or 'function'.
	}
} // vardeclaration

//////////////////////////////////////////////////////////////////////
void listcode(int from, int to)
{
	int i;

	printf("\n");
	for (i = from; i < to; i++)
	{
		printf("%5d %s\t%d\t%d\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
	}
	printf("\n");
} // listcode

//////////////////////////////////////////////////////////////////////
void factor(symset fsys)
{
	void expression(symset fsys);
	void or_expr(symset fsys);
	void assign_expr(symset fsys);
	void statement(symset fsys);
	int i ,dp ,coun;
	symset set;
	set = uniteset(fsys, createset(SYM_IDENTIFIER, SYM_NUMBER, SYM_LPAREN,  SYM_NULL));

	test(facbegsys, fsys, 24); // The symbol can not be as the beginning of an expression.

	//while (inset(sym, facbegsys)) commented!
	if (inset(sym, facbegsys))//2017-10-27
	{
		if (sym == SYM_IDENTIFIER)
		{
			if ((i = position(id)) == 0)
			{
				getsym();
				if(sym == SYM_COLON)
				{
					enter(ID_LABEL);
					getsym();
					statement(fsys);
				}
				else
					error(11); // Undeclared identifier.
			}
			else
			{
				getsym();
				if(sym == SYM_COLON && cx_0[i] != 0)
				{
					code[cx_0[i]].a = cx;
					getsym();
					statement(fsys);
				}
				switch (table[i].kind)
				{
					mask* mk;
				case ID_CONSTANT:
					gen(LIT, 0, table[i].value);
				//	getsym();
					break;
				case ID_VARIABLE:
					mk = (mask*)&table[i];
					gen(LOD, level - mk->level, mk->address);
				//	getsym();
					break;
                case ID_PVARIABLE:
					mk = (mask*)&table[i];
					gen(LOD, 0, mk->address);
				//	getsym();
                    break;
                case ID_DIM:
                    dp = dposition(id);
                    coun = 0;
                    offset = 1;
                    for(coun=1;coun<=d[dp].c;coun++)
                        offset *= d[dp].l[coun];
 					mk = (mask*)&table[i];
 					gen(LIT , 0 , mk->address);
 				//	getsym();
 					coun = 1;
 					while(sym == SYM_LSQURBRA&&coun <= d[dp].c)
                    {
                          getsym();
                          offset /= d[dp].l[coun++];
                          gen(LIT,0,offset);
                          assign_expr(fsys);
                          gen(OPR,0,OPR_MUL);
                          gen(OPR,0,OPR_ADD);
                          if(sym == SYM_RSQURBRA)
                            getsym();
                          else error(32);
 					}
 					gen(LOA,level - mk->level,0);
                    break;
                case ID_PDIM:
                    dp = dposition(id);
                    coun = 0;
                    offset = 1;
                    for(coun=1;coun<=d[dp].c;coun++)
                        offset *= d[dp].l[coun];
 					mk = (mask*)&table[i];
 					//gen(LOD , 0 , mk->address+1);
 					gen(LOD , 0 , mk->address);
 				//	getsym();
 					coun = 1;
 					while(sym == SYM_LSQURBRA&&coun <= d[dp].c)
                    {
                          getsym();
                          offset /= d[dp].l[coun++];
                          gen(LIT,0,offset);
                          assign_expr(fsys);
                          gen(OPR,0,OPR_MUL);
                          gen(OPR,0,OPR_ADD);
                          if(sym == SYM_RSQURBRA)
                            getsym();
                          else error(32);
 					}
 					gen(LOA,level - mk->level,0);
                    break;
                case ID_PFUNCTION:{
                    gen(INT , 0 , 4);
                    int k1 = fposition(id);
                    int v1 = f[k1].c;
                    int c = 1;
                    if(sym == SYM_LPAREN)
                    {
                    //  getsym();
                        while(sym != SYM_RPAREN)
                        {
                            if(f[k1].kind[c] == ID_PVARIABLE)
                                assign_expr(fsys);
                            else if(f[k1].kind[c] == ID_PDIM)
                            {
                               int x = position(id);
                               mk = (mask *) &table[x];
                               gen(LIT , 0 , mk->address) ;
                               //gen(LIT , 0 , mk->level) ;
                               //v++;
                               getsym();
                            }
                            else
                            {
                               int x = fposition(id);
                               mk = (mask *) &table[x];
                               gen(LIT , 0 , mk->address) ;
                               //gen(LIT , 0 , mk->level) ;
                               //v++;
                               getsym();
                            }
                            c++;
                            if(sym == SYM_COMMA)
                                getsym();
                            else if(sym != SYM_RPAREN){
                                error(22);
                                break;
                            }
                        }
                        getsym();
                        /*while(v > 0)
                        {
                            int i = position(f[k].var[v]);
                            mk = (mask *) &table[i];
                            gen(STO, level - mk->level, mk->address);
                            gen(POP,0,0);
                            v--;
                        }*/
                    }
                    gen(INT,0,-4-v1);
                    mk = (mask *) &table[i];
                    gen(LOD,  0 ,  mk->address);
					gen(CAP, 0,  mk->address);
					//error(21); // Procedure identifier can not be in an expression.
					break;}
				case ID_FUNCTION:		//2017.11.3
				    int k =fposition(id);
				    int v = f[k].c;
				    gen(INT,0,4);
				    int flag = 1 ;
		 		//	getsym();
				    //gen(POP,0,0);
				    if(sym == SYM_LPAREN)
                    {
                        getsym();
                        while(sym != SYM_RPAREN)
                        {
                            if(f[k].kind[flag] == ID_PVARIABLE)
                                assign_expr(fsys);
                            else if(f[k].kind[flag] == ID_PDIM)
                            {
                               int x = position(id);
                               mk = (mask *) &table[x];
                               gen(LIT , 0 , mk->address) ;
                               //gen(LIT , 0 , mk->level) ;
                               //v++;
                               getsym();
                            }
                            else
                            {
                               int x = fposition(id);
                               mk = (mask *) &table[x];
                               gen(LIT , 0 , mk->address) ;
                               //gen(LIT , 0 , mk->level) ;
                               //v++;
                               getsym();
                            }
                            flag++;
                            if(sym == SYM_COMMA)
                                getsym();
                            else if(sym != SYM_RPAREN){
                                error(22);
                                break;
                            }
                        }
                        getsym();
                        /*while(v > 0)
                        {
                            int i = position(f[k].var[v]);
                            mk = (mask *) &table[i];
                            gen(STO, level - mk->level, mk->address);
                            gen(POP,0,0);
                            v--;
                        }*/
                    }
                    gen(INT,0,-4-v);
					mk = (mask *) &table[i];
					gen(CAL, level - mk->level,  mk->address);
					//error(21); // Procedure identifier can not be in an expression.
					break;
				} // switch
			}
		}
		else if (sym == SYM_NUMBER)
		{
			if (num > MAXADDRESS)
			{
				error(25); // The number is too great.
				num = 0;
			}
			gen(LIT, 0, num);
			getsym();
		}
		else if (sym == SYM_LPAREN)
		{
			getsym();
			set = uniteset(set, createset(SYM_RPAREN, SYM_NULL));
			assign_expr(set);//expression(set);		//2017.11.3  or_expr ->	assign_expr
			//destroyset(set);
			if (sym == SYM_RPAREN)
			{
				getsym();
			}
			else
			{
				error(22); // Missing ')'.
			}
		}
		else if (sym == SYM_MINUS) // UMINUS,  Expr -> '-' Expr
		{
			getsym();
			factor(fsys);//2017-10-27
			gen(OPR, 0, OPR_NEG);
		}
		else if (sym == SYM_NOT) // UMINUS,  Expr -> '!' Expr
		{
			getsym();
			factor(fsys);//or_expr -> factor	//2017.10.9
			gen(OPR, 0, OPR_NOT);
		}
		else if (sym == SYM_ODD) 		//2017-09-24
		{
			getsym();
			expression(fsys);
			gen(OPR, 0, 6);
		}
		else if (sym == SYM_INC)//++i
		{
			getsym();
			factor(fsys);   //stack[top] = i
		//	mask* mk;
		//	i = position(id);
		//	mk = (mask*) &table[i];
			int c = cx - 1;
			if (code[cx - 1].f == LOD || code[cx - 1].f == LOA)//l-value check   //i must be a l-value
			{
				if(code[cx - 1].f == LOA)
				 	code[cx-1].f == LAD;
				gen(LIT, 0, 1);
				gen(OPR, 0, OPR_ADD);
				if(code[c].f == LOD)
				  	gen(STO, code[c].l, code[c].a);
				else
				  	gen(STA, code[c].l, 1);
			}
			else
				error(26);
		}
		else if (sym == SYM_DEC)	//--i		//2017.10.29
		{
			getsym();
			factor(fsys);
		//	mask* mk;
		//	i = position(id);
		//	mk = (mask*) &table[i];
			int c = cx - 1;
			if (code[c].f == LOD || code[c].f == LOA)//l-value check   //i must be a l-value
			{
				if(code[c].f == LOA)
				  	code[c].f == LAD;
				gen(LIT, 0, 1);
				gen(OPR, 0, OPR_MIN);
				if(code[c].f == LOD)
				  	gen(STO, code[c].l, code[c].a);
				else
					gen(STA, code[c].l, 1);
			}
			else
				error(26);
		}
		else if(sym == SYM_RANDOM)
		{
			getsym();
			//assign_expr(fsys);
			if (sym == SYM_LPAREN)
			{
				getsym();
				set = uniteset(set, createset(SYM_RPAREN, SYM_NULL));
				if (sym == SYM_RPAREN)
				{
					gen(LIT , 0 , 0);
					getsym();
				}
				else
				{
					assign_expr(set);//expression(set);		//2017.11.3  or_expr ->	assign_expr
					//destroyset(set);
					if (sym == SYM_RPAREN)
						getsym();
					else
						error(22); // Missing ')'.
				}
			}
			int c = cx - 1;
			gen(RAN, 0, 0);
			gen(STO, code[c].l, code[c].a);
		}
		else if(sym == SYM_PRINT)
		{
			getsym();
			if (sym == SYM_LPAREN)
			{
				getsym();
				if(sym == SYM_RPAREN)
				{
					gen(LIT, 0 , 0xffff);
					gen(STO, code[cx].l, code[cx].a);
					getsym();
				}
				else
					{
						while(sym == ID_VARIABLE || sym == SYM_NUMBER)
						{
							assign_expr(fsys);
							gen(STO, code[cx].l, code[cx].a);
							if(sym == SYM_COMMA ||sym == SYM_RPAREN)
								getsym();
								else
								error(22);
					}
				}
				}
			}
			else if(sym == SYM_CALLSTACK)
			{
				getsym();
				if (sym == SYM_LPAREN)
					getsym();
					else
					 	error(27);
				if(sym == SYM_RPAREN)
					getsym();
					else
						error(22);
				gen(CAS, 0, 0);
			}
		//test(fsys, createset(SYM_LPAREN, SYM_NULL), 23);
	} // if (inset(sym, facbegsys))
	else
		error(23);
	destroyset(set);
 } // factor

void postfix_expr(symset fsys)
{
	int postfixop;
	int i;
	symset set;
	set = uniteset(fsys, createset(SYM_INC, SYM_DEC,  SYM_NULL));
	factor(set);
//	mask* mk;
//	i = position(id);
//	mk = (mask*) &table[i];
	while (sym == SYM_INC || sym == SYM_DEC )
	{
		int c = cx -1 ;
		postfixop = sym;
		if(postfixop == SYM_INC)	//i++
		{
			if (code[c].f == LOD||code[c].f == LOA)//l-value check
			{
				if(code[c].f == LOD)
				 gen(LOD, code[c].l, code[c].a);//save the previous value of i
				else
				{
					code[c].f = LAD;
					gen(CPY,0,0);
					//gen(LOD, level - mk->level, mk->address);//save the previous value of i
				}
				gen(LIT, 0, 1);
				gen(OPR, 0, OPR_ADD);
				if(code[c].f == LOD)
				  gen(STO, code[c].l, code[c].a);
				else
				  gen(STA, code[c].l, 1);
				gen(POP, 0, 0);
			}
			else
				error(26);
	   	}
		else if(postfixop == SYM_DEC)// i--
		{
			if (code[c].f == LOD||code[c].f == LOA)//l-value check
			{
				if(code[c].f == LOD)
				 gen(LOD, code[c].l, code[c].a);//save the previous value of i
				else
				{
					code[c].f = LAD;
					gen(CPY,0,0);
					//gen(LOD, level - mk->level, mk->address);
				}
				gen(LIT, 0, 1);
				gen(OPR, 0, OPR_MIN);
				if(code[c].f == LOD)
				  gen(STO, code[c].l, code[c].a);
				else
				  gen(STA, code[c].l, 1);
				gen(POP, 0, 0);
			}
			else
				error(26);
		}
		getsym();
	} // while
	destroyset(set);
} // postfix_expr

void term(symset fsys)
{
	int mulop;
	symset set;

	set = uniteset(fsys, createset(SYM_TIMES, SYM_SLASH, SYM_MOD, SYM_NULL));
	postfix_expr(set);
	while (sym == SYM_TIMES || sym == SYM_SLASH || sym == SYM_MOD)
	{
		mulop = sym;
		getsym();
		postfix_expr(set);
		if (mulop == SYM_TIMES)
		{
			gen(OPR, 0, OPR_MUL);
		}
		else if(mulop == SYM_SLASH)
		{
			gen(OPR, 0, OPR_DIV);
		}
		else
		{
			gen(OPR, 0, OPR_MOD);
		}
	} // while
	destroyset(set);
} // term

//////////////////////////////////////////////////////////////////////
void expression(symset fsys)
{
	int addop;
	symset set;

	set = uniteset(fsys, createset(SYM_PLUS, SYM_MINUS, SYM_NULL));

	term(set);
	while (sym == SYM_PLUS || sym == SYM_MINUS)
	{
		addop = sym;
		getsym();
		term(set);
		if (addop == SYM_PLUS)
		{
			gen(OPR, 0, OPR_ADD);
		}
		else
		{
			gen(OPR, 0, OPR_MIN);
		}
	} // while

	destroyset(set);
} // expression

void shift_expr(symset fsys)
{
	int shiftop;
	symset set;

	set = uniteset(fsys, createset(SYM_SHL, SYM_SHR, SYM_NULL));

	expression(set);
	while (sym == SYM_SHL || sym == SYM_SHR)
	{
		shiftop = sym;
		getsym();
		expression(set);
		if (shiftop == SYM_SHL)
			gen(OPR, 0, OPR_SHL);
		else
			gen(OPR, 0, OPR_SHR);
	} // while
	destroyset(set);
} // shift_expr

void rel_expr(symset fsys) //condition
{
	int relop;
	symset set;
	{
		set = uniteset(relset, fsys);
		shift_expr(set);
		//destroyset(set);
		if (!inset(sym, relset))
		{
			//error(20);//2017-09-24
		}
		else
		{
			relop = sym;
			getsym();
			shift_expr(fsys);
			switch (relop)
			{
			case SYM_EQU:
				gen(OPR, 0, OPR_EQU);
				break;
			case SYM_NEQ:
				gen(OPR, 0, OPR_NEQ);
				break;
			case SYM_LES:
				gen(OPR, 0, OPR_LES);
				break;
			case SYM_GEQ:
				gen(OPR, 0, OPR_GEQ);
				break;
			case SYM_GTR:
				gen(OPR, 0, OPR_GTR);
				break;
			case SYM_LEQ:
				gen(OPR, 0, OPR_LEQ);
				break;
			} // switch
		} // else
		destroyset(set);
	} // else
} // rel_expr , condition



void and_bit_expr(symset fsys)// &
{
	symset set;

	set = uniteset(fsys, createset(SYM_BIT_AND, SYM_NULL));

	rel_expr(set);
	while (sym == SYM_BIT_AND)
	{
		getsym();
		rel_expr(set);
		gen(OPR, 0, OPR_BIT_AND);

	} // while

	destroyset(set);
}//and_bit_expr

void xor_bit_expr(symset fsys)// ^
{
	symset set;

	set = uniteset(fsys, createset(SYM_BIT_XOR, SYM_NULL));

	and_bit_expr(set);
	while (sym == SYM_BIT_XOR)
	{
		getsym();
		and_bit_expr(set);
		gen(OPR, 0, OPR_BIT_XOR);

	} // while

	destroyset(set);
}//xor_bit_expr

void or_bit_expr(symset fsys)// |
{
	symset set;

	set = uniteset(fsys, createset(SYM_BIT_OR, SYM_NULL));

	xor_bit_expr(set);
	while (sym == SYM_BIT_OR)
	{
		getsym();
		xor_bit_expr(set);
		gen(OPR, 0, OPR_BIT_OR);

	} // while

	destroyset(set);
}//or_bit_expr        2017.10.9

void and_expr(symset fsys)
{
	symset set;

	set = uniteset(fsys, createset(SYM_AND, SYM_NULL));

	or_bit_expr(set);
	while (sym == SYM_AND)
	{
		getsym();
		or_bit_expr(set);
		gen(OPR, 0, OPR_AND);
	} // while

	destroyset(set);
} //and_expr

void or_expr(symset fsys)
{
	symset set;

	set = uniteset(fsys, createset(SYM_OR, SYM_NULL));

	and_expr(set);
	while (sym == SYM_OR)
	{
		getsym();
		and_expr(set);
		gen(OPR, 0, OPR_OR);
	}

	destroyset(set);
} // or_expr

void question_expr(symset fsys)
{
	int cx1, cx2;
	void assign_expr(symset fsys);
	symset set;
	set = uniteset(fsys, createset(SYM_QUESTION, SYM_COLON, SYM_NULL));
	or_expr(set);
	if (sym == SYM_QUESTION)
	{
		cx1 = cx;
		gen(JPC, cx1, 0);
		getsym();
		assign_expr(set);
		cx2 = cx;
		gen(JMP, cx2, cx + 1);
		code[cx1].a = cx;
		if (sym == SYM_COLON)
		{
			getsym();
			assign_expr(set);
			code[cx2].a = cx;
		}
		else
			error(29);
	}
	destroyset(set);
} //question_expr

void assign_expr(symset fsys)
{
	int assignop;
	int i , c;
	symset set, set1;
	set1 = createset(SYM_BECOMES, SYM_BIT_AND_BEC, SYM_BIT_OR_BEC, SYM_BIT_XOR_BEC, SYM_PLUS_BEC,
	SYM_MINUS_BEC, SYM_TIMES_BEC, SYM_SLASH_BEC, SYM_MOD_BEC, SYM_SHL_BEC, SYM_SHR_BEC, SYM_NULL);
	set = uniteset(fsys, set1);
    question_expr(set);
//	mask* mk;
//	i = position(id);
//	mk = (mask*) &table[i];
	if (inset(sym, set1))
	{
		if (code[cx - 1].f == LOD||code[cx - 1].f == LOA) //l-value check
		{
		    c = cx - 1;
		    if(code[c].f == LOA)
                {
                	code[c].f = LAD;
                	if(sym == SYM_BECOMES)
					gen(POP,0,0);
				}
			assignop = sym;
			getsym();
			assign_expr(set);
			switch (assignop)
			{
		 	case SYM_BECOMES:
		 	    if(code[c].f == LOD)
				gen(STO, code[c].l, code[c].a);
				if(code[c].f == LAD)
                gen(STA, code[c].l, 0);
				//gen(POP, 0, 0);
				break;
			case SYM_BIT_AND_BEC:
			{
				gen(OPR, 0, OPR_BIT_AND);
				if(code[c].f == LAD)
                gen(STA, code[c].l, 0);
                else
				gen(STO, code[c].l, code[c].a);
				//gen(POP, 0, 0);
				break;
			}
			case SYM_BIT_XOR_BEC:
			{
				gen(OPR, 0, OPR_BIT_XOR);
				if(code[c].f == LAD)
                gen(STA, code[c].l, 0);
                else
				gen(STO, code[c].l, code[c].a);
				//gen(POP, 0, 0);
				break;
			}
			case SYM_BIT_OR_BEC:
			{
				gen(OPR, 0, OPR_BIT_OR);
				if(code[c].f == LAD)
                gen(STA, code[c].l, 0);
                else
				gen(STO, code[c].l, code[c].a);
				//gen(POP, 0, 0);
				break;
			}
			case SYM_PLUS_BEC:
			{
				gen(OPR, 0, OPR_ADD);
				if(code[c].f == LAD)
                gen(STA, code[c].l, 0);
                else
				gen(STO, code[c].l, code[c].a);
				//gen(POP, 0, 0);
				break;
			}
			case SYM_MINUS_BEC:
			{
				gen(OPR, 0, OPR_MIN);
				if(code[c].f == LAD)
                gen(STA, code[c].l, 0);
                else
				gen(STO, code[c].l, code[c].a);
				//gen(POP, 0, 0);
				break;
			}
			case SYM_TIMES_BEC:
			{
				gen(OPR, 0, OPR_MUL);
				if(code[c].f == LAD)
                gen(STA, code[c].l, 0);
                else
				gen(STO, code[c].l, code[c].a);
				//gen(POP, 0, 0);
				break;
			}
			case SYM_SLASH_BEC:
			{
				gen(OPR, 0, OPR_DIV);
				if(code[c].f == LAD)
                gen(STA, code[c].l, 0);
                else
				gen(STO, code[c].l, code[c].a);
				//gen(POP, 0, 0);
				break;
			}
			case SYM_MOD_BEC:
			{
				gen(OPR, 0, OPR_MOD);
				if(code[c].f == LAD)
                gen(STA, code[c].l, 0);
                else
				gen(STO, code[c].l, code[c].a);
				//gen(POP, 0, 0);
				break;
			}
			case SYM_SHL_BEC:
			{
				gen(OPR, 0, OPR_SHL);
				if(code[c].f == LAD)
                gen(STA, code[c].l, 0);
                else
				gen(STO, code[c].l, code[c].a);
				//gen(POP, 0, 0);
				break;
			}
			case SYM_SHR_BEC:
			{
				gen(OPR, 0, OPR_SHR);
				if(code[c].f == LAD)
                gen(STA, code[c].l, 0);
                else
				gen(STO, code[c].l, code[c].a);
				//gen(POP, 0, 0);
				break;
			}
			default:
			error(26);break;
			}
		}
	}
	destroyset(set);
	destroyset(set1);
} //assign_expr   	2017.10.29

void statement(symset fsys)
{
	int i, cx1, cx2;
	symset set1, set;
	if (inset(sym, facbegsys))
	{
		assign_expr(fsys);
		gen(POP, 0, 0);
	}
	else if (sym == SYM_RETURN)
	{
		getsym();
		if (inset(sym, facbegsys))
		{
			assign_expr(fsys);
			if (sym == SYM_SEMICOLON)
				getsym();
			else
				error(10);
		}
		else if (sym == SYM_SEMICOLON)
		{
			gen(LIT, 0, 0);
			getsym();
		}
		else
			error(10);
		gen(OPR, 0, OPR_RET);
		cx1 = cx;
		gen(JMP, cx1, 0);
		statement(fsys);
		code[cx1].a = cx;
	}
	else if (sym == SYM_IF)
	{
		set1 = createset(SYM_ELSE, SYM_NULL);
		set = uniteset(set1, fsys);
		getsym();
		if (sym == SYM_LPAREN)
			assign_expr(fsys);
		else
			error(27);
		cx1 = cx;
		gen(JPC, cx1, 0);
		statement(set);
		cx2 = cx;
		gen(JMP, cx2, cx + 1);
		code[cx1].a = cx;
		if (sym == SYM_ELSE)
		{
			getsym();
			statement(set);
			code[cx2].a = cx;
		}
		else if (sym == SYM_ELIF)
		{
			sym =SYM_IF;
			statement(set);
			code[cx2].a = cx;
		}
		destroyset(set1);
		destroyset(set);
	}
	else if (sym == SYM_BEGIN)
	{
		// block
		getsym();
		set1 = createset(SYM_SEMICOLON, SYM_END, SYM_NULL);
		set = uniteset(set1, fsys);
		statement(set);
		while (sym == SYM_SEMICOLON || inset(sym, statbegsys))
		{
			if (sym == SYM_SEMICOLON)
				getsym();
			else
				error(10);
			statement(set);
		} // while
		destroyset(set1);
		destroyset(set);
		if (sym == SYM_END)
			getsym();
		else
			error(17); // ';' or 'end' expected.
	}
	else if (sym == SYM_WHILE)
	{
		// while statement
		lflag = 1;
		cx1 = cx;
		getsym();
		if (sym == SYM_LPAREN)
			rel_expr(fsys);
		else
			error(27);
		cx2 = cx;

		gen(JPC, 0, 0);
		statement(fsys);
		if(cflag == 1)
	 		code[cx4].a = cx1;
		gen(JMP, 0, cx1);
		code[cx2].a = cx;
		if(bflag == 1)
			code[cx3].a = cx;
		bflag = 0;
        cflag = 0;
	}
	else if(sym==SYM_FOR)//for
	{
		lflag=1;
		set1=createset(SYM_RPAREN);
		set=uniteset(set1,fsys);
		getsym();
		if(sym == SYM_LPAREN)
		{
			getsym();
			if(inset(sym, facbegsys))
			{
				assign_expr(set);
                while(sym==SYM_COMMA)
                {
                        getsym();
                        if(inset(sym, facbegsys))
                            {
                                assign_expr(set);
                            }
                }
				cx1=cx;
				getsym();
				if(inset(sym, facbegsys))
				{
					rel_expr(set);
					cx2=cx;
					gen(JPC, 0, 0);
                    getsym();
                    if(inset(sym, facbegsys))
                        {
                            assign_expr(set);
                            while(sym==SYM_COMMA)
                            {
                                getsym();
                                if(inset(sym, facbegsys))
                                    {
                                        assign_expr(set);
                                    }
                            }
                        }
                    else
                        error(27);
				}
				else
					error(27);
			}
			else
				error(27);
		}
		else
			error(27);
		getsym();
		statement(set);
		if(cflag==1)code[cx4].a = cx1;
		gen(JMP, 0, cx1);
		code[cx2].a = cx;
		if(bflag==1)code[cx3].a = cx;
		 bflag=0;
        cflag=0;
		destroyset(set1);
		destroyset(set);
	}
	else if(sym == SYM_DO)
    {
        lflag=1;
        cx1=cx;
        getsym();
        statement(fsys);
        if(sym==SYM_WHILE)
        {
            getsym();
            if(sym==SYM_LPAREN)
            {
                getsym();
                if(inset(sym, facbegsys))
                {
                    if(cflag==1)code[cx4].a = cx;
                    rel_expr(fsys);
                    cx2=cx;
                    gen(JPC, 0, 0);
                    gen(JMP, 0, cx1);
                    if(bflag==1)code[cx3].a = cx;
                    bflag=0;
                    cflag=0;
                    code[cx2].a = cx;
                    getsym();
                }
            }
        }
    }
    /*else if(sym==SYM_SWITCH)
    {
        getsym();
        if(sym==SYM_LPAREN)
        {
            assign_expr(fsys);

        }
    }*/
    else if(sym==SYM_BREAK)
    {
        if(lflag==0)
        {
             error(28);
             getsym();
             statement(fsys);
        }
        else{
        cx3=cx;
        bflag=1;
        gen(JMP,0,0);
        getsym();
        lflag=0;
        }
    }
    else if(sym==SYM_CONTINUE)
    {
         if(lflag==0)
        {
             error(28);
             getsym();
             statement(fsys);
        }
        else{
        cflag=1;
        cx4=cx;
        gen(JMP,0,0);
        getsym();
        lflag=0;}
    }
	else if (sym == SYM_EXIT)
	{
		getsym();
		if(sym == SYM_LPAREN)
		{
			getsym();
			if(sym == SYM_RPAREN)
				gen(OPR, 0 , OPR_LEAVE);
				else
					error(22);
			getsym();
			}
			else
				error(27);
	}
	else if(sym == SYM_GOTO)
	{
		getsym();
		if(sym == SYM_IDENTIFIER)
		{
			if((i = position(id))== 0)
				enter(ID_LABEL);
			i = position(id);
			mask* mk;
			mk = (mask*) &table[i];
			cx_0[i] = cx;
			gen(JMP, level - mk->level ,mk -> address+1);
			getsym();
		}
		else
			error(14);
	}
	else
	test(fsys, phi, 19);
} // statement


void block(symset fsys)
{
	int cx0; // initial code index
	mask* mk;
	int block_dx , block_tx;
	int block_dimx , block_fx ;
	int savedTx;
	symset set1, set;

	dx = 4;//2017.11.3
	block_dx = dx;
	mk = (mask*)&table[tx];
	mk->address = cx;
	gen(JMP, 0, 0);
	if (level > MAXLEVEL)
	{
		error(32); // There are too many levels.
	}
	do
	{
		if (sym == SYM_CONST)
		{ // constant declarations
			getsym();
			do
			{
				constdeclaration();
				while (sym == SYM_COMMA)
				{
					getsym();
					constdeclaration();
				}
				if (sym == SYM_SEMICOLON)
				{
					getsym();
				}
				else
				{
					error(5); // Missing ',' or ';'.
				}
			} while (sym == SYM_IDENTIFIER);
		} // if

		if (sym == SYM_VAR)
		{ // variable declarations
			getsym();
			do
			{
				vardeclaration();
				while (sym == SYM_COMMA)
				{
					getsym();
					vardeclaration();
				}
				if (sym == SYM_SEMICOLON)
				{
					getsym();
				}
				else
				{
					error(5); // Missing ',' or ';'.
				}
			} while (sym == SYM_IDENTIFIER);
		} // if
		block_dx = dx; //save dx before handling procedure call!
		block_tx = tx;
		block_dimx = dimx ;
		while (sym == SYM_FUNCTION)
		{ // function declarations
			getsym();
			if (sym == SYM_IDENTIFIER)
			{
                enter(ID_FUNCTION);
                //block_fx = fx ;
				getsym();
				if (sym == SYM_LPAREN)
					{
					    offset = 3 ;
						getsym();
						if (sym == SYM_RPAREN)
							getsym();
                        else if(sym == SYM_VAR || sym == SYM_FUNCTION){
                            int v = 1;
                            if(sym == SYM_VAR)
                            {
                                getsym();
                                strcpy(f[fx].var[v] , id);
                                pvardeclaration();
                                if(table[tx].kind == ID_PDIM)
                                    f[fx].kind[v++] = ID_PDIM;
                                else
                                    f[fx].kind[v++] = ID_PVARIABLE;
                             }
                             if(sym == SYM_FUNCTION)
                             {
                                getsym();
                                strcpy(f[fx].var[v] , id);
                                enter(ID_PFUNCTION);
                                f[fx].kind[v++] = ID_PFUNCTION;
                                getsym();
                                if(sym == SYM_LPAREN)
                                {
                                    int v = 1;
                                    getsym();
                                    if(sym == SYM_VAR)
                                    {
                                      getsym();
                                      strcpy(f[afx].var[v] , id);
                                      pvardeclaration();
                                      if(table[tx].kind == ID_PDIM)
                                         f[afx].kind[v++] = ID_PDIM;
                                      else
                                         f[afx].kind[v++] = ID_PVARIABLE;
                                    }
                                    while(sym != SYM_RPAREN)
                                    {
                                       if(sym == SYM_COMMA)
                                       {
                                            getsym();
                                            if(sym == SYM_VAR)
                                            {
                                               getsym();
                                               strcpy(f[afx].var[v] , id);
                                               pvardeclaration();
                                               if(table[tx].kind == ID_PDIM)
                                                  f[afx].kind[v++] = ID_PDIM;
                                               else
                                                  f[afx].kind[v++] = ID_PVARIABLE;
                                            }
                                       }
                                    }
                                    getsym();
                                    f[afx].c = v ;
                                }
                             }
                                while(sym != SYM_RPAREN){
                                if(sym == SYM_COMMA)
                                {
                                    getsym();
                                    if(sym == SYM_VAR || sym == SYM_FUNCTION)
                                       {
                                           if(sym == SYM_VAR)
                                           {
                                           getsym();
                                           strcpy(f[fx].var[v] , id);
                                           pvardeclaration();
                                           if(table[tx].kind == ID_PDIM)
                                                f[fx].kind[v++] = ID_PDIM;
                                           else
                                                f[fx].kind[v++] = ID_PVARIABLE;
                                            }
                                            if(sym == SYM_FUNCTION)
                                            {
                                                getsym();
                                                strcpy(f[fx].var[v] , id);
                                                enter(ID_PFUNCTION);
                                                f[fx].kind[v++] = ID_PFUNCTION;
                                                getsym();
                                                if(sym == SYM_LPAREN)
                                {
                                    int v = 1;
                                    getsym();
                                    if(sym == SYM_VAR)
                                    {
                                      getsym();
                                      strcpy(f[afx].var[v] , id);
                                      pvardeclaration();
                                      if(table[tx].kind == ID_PDIM)
                                         f[afx].kind[v++] = ID_PDIM;
                                      else
                                         f[afx].kind[v++] = ID_PVARIABLE;
                                    }
                                    while(sym != SYM_RPAREN)
                                    {
                                       if(sym == SYM_COMMA)
                                       {
                                            getsym();
                                            if(sym == SYM_VAR)
                                            {
                                               getsym();
                                               strcpy(f[afx].var[v] , id);
                                               pvardeclaration();
                                               if(table[tx].kind == ID_PDIM)
                                                  f[afx].kind[v++] = ID_PDIM;
                                               else
                                                  f[afx].kind[v++] = ID_PVARIABLE;
                                            }
                                       }
                                    }
                                    getsym();
                                    f[afx].c = v ;
                                }
                                             }
                                       }
                                    else error(32);
                                }
                                else  error(5);
                                }
                               f[fx].c = v - 1 ;
                               strcpy(id , "0000");
                               enter(ID_PVARIABLE);
                                getsym();
                        }
                        else
                            error(22);
					}
				else
					error(22);
			}
			else
					error(4); // There must be an identifier to follow 'const', 'var', or 'function'.
			level++;
			savedTx = tx;
			set1 = createset(SYM_SEMICOLON, SYM_NULL);
			set = uniteset(set1, fsys);
			block(set);
			destroyset(set1);
			destroyset(set);
			tx = savedTx;
			level--;

			if (sym == SYM_SEMICOLON)
			{
				getsym();
				set1 = createset(SYM_IDENTIFIER, SYM_FUNCTION, SYM_NULL);
				set = uniteset(statbegsys, set1);
				test(set, fsys, 6);
				destroyset(set1);
				destroyset(set);
			}
			else
			{
				error(5); // Missing ',' or ';'.
			}
		} // while
		dx = block_dx; //restore dx after handling function!
		//tx = block_tx;
		set1 = createset(SYM_IDENTIFIER, SYM_NULL);
		set = uniteset(statbegsys, set1);
		test(set, declbegsys, 7);
		destroyset(set1);
		destroyset(set);
	} while (inset(sym, declbegsys));

	code[mk->address].a = cx;
	mk->address = cx;
	cx0 = cx;
	gen(INT, 0, block_dx + f[fx].c);
	set1 = createset(SYM_SEMICOLON, SYM_END, SYM_NULL);
	set = uniteset(set1, fsys);
	statement(set);
	destroyset(set1);
	destroyset(set);
	gen(OPR, 0, OPR_LEAVE); // leave
	block_tx = tx;
	while(table[tx].kind == ID_PVARIABLE || table[tx].kind == ID_PDIM) tx--;
	afx = fx ;
	dimx = block_dimx ;
	//fx = block_fx ;
	test(fsys, phi, 8); // test for error: Follow the statement is an incorrect symbol.
	listcode(cx0, cx);
} // block

//////////////////////////////////////////////////////////////////////
int base(int stack[], int currentLevel, int levelDiff)
{   //stack -- rutime stack storing ARs
	//currentLevel -- base address for current procedure(caller) AR
	//levelDiff(non-negative) -- level difference between a name's reference point and definition point!
	//thus, when finished, we should just reach an  outside surrounding scope, getting its AR base address as return value
	//in further, if a name @levelDiff is a common variable, we could find its value within that AR.
	//if a name@levelDiff means a procedure call, we lastly stop at its parent procedure's AR, a static link could be set up!

	int b = currentLevel;

	while (levelDiff--)
		b = stack[b];
	return b;
} // base

//////////////////////////////////////////////////////////////////////
// interprets and executes codes.
void interpret()
{
	int pc;        // program counter
	int stack[STACKSIZE] = {0};
	int top;       // top of stack
	int b;         // program, base, and top-stack register
	instruction i; // instruction register
	srand((unsigned)time(NULL));

	printf("Begin executing PL/0 program.\n");

	pc = 0;
	b = 2;
	top = 4;
	stack[1] = stack[2] = stack[3] = stack[4] = 0;//2017.11.3
	do
	{
		i = code[pc++];
		switch (i.f)
		{
		case LIT:
			stack[++top] = i.a;
			break;
		case OPR:
			switch (i.a) // operator
			{
			case OPR_LEAVE:
				top = b - 1;//restore stack pointer, then free all 4 items allocated to callee's AR
				pc = stack[top + 3];//restore ip
				b = stack[top + 2];// restore base pointer for caller's AR
			//	top++;	/*modified 2017.11.3*/
				break;
			case OPR_RET:
				stack[b - 1] = stack[top];//return
				top--;
				break;  /*added 2017.11.3*/
			case OPR_NOT: //2017-09-24
				stack[top] = !stack[top];
				break;
			case OPR_AND:
				top--;
				if(stack[top]==1)
					stack[top] = stack[top] && stack[top + 1];
				break;
			case OPR_OR:
				top--;
				if(stack[top]==0)
					stack[top] = stack[top] || stack[top + 1];
				break;
			case OPR_NEG:
				stack[top] = -stack[top];
				break;
			case OPR_ADD:
				top--;
				stack[top] += stack[top + 1];
				break;
			case OPR_MIN:
				top--;
				stack[top] -= stack[top + 1];
				break;
			case OPR_MUL:
				top--;
				stack[top] *= stack[top + 1];
				break;
			case OPR_DIV:
				top--;
				if (stack[top + 1] == 0)
				{
					fprintf(stderr, "Runtime Error: Divided by zero.\n");
					fprintf(stderr, "Program terminated.\n");
					continue;
				}
				stack[top] /= stack[top + 1];
				break;
			case OPR_MOD:
				top--;
				if (stack[top + 1] == 0)
				{
					fprintf(stderr, "Runtime Error: Divided by zero.\n");
					fprintf(stderr, "Program terminated.\n");
					continue;
				}
				stack[top] %= stack[top + 1];
				break;
			case OPR_BIT_AND:
				top--;
				stack[top] = stack[top] & stack[top + 1];
				break;
			case OPR_BIT_OR:
				top--;
				stack[top] = stack[top] | stack[top + 1];
				break;
			case OPR_BIT_XOR:
				top--;
				stack[top] = stack[top] ^ stack[top + 1];
				break;//2017.10.9
			case OPR_SHL:
				top--;
				stack[top] = stack[top] << stack[top + 1];
				break;
			case OPR_SHR:
				top--;
				stack[top] = stack[top] >> stack[top + 1];
				break;//2017.10.19
			case OPR_ODD:
				stack[top] %= 2;
				break;
			case OPR_EQU:
				top--;
				stack[top] = stack[top] == stack[top + 1];
				break;
			case OPR_NEQ:
				top--;
				stack[top] = stack[top] != stack[top + 1];
				break;
			case OPR_LES:
				top--;
				stack[top] = stack[top] < stack[top + 1];
				break;
			case OPR_GEQ:
				top--;
				stack[top] = stack[top] >= stack[top + 1];
				break;
			case OPR_GTR:
				top--;
				stack[top] = stack[top] > stack[top + 1];
				break;
			case OPR_LEQ:
				top--;
				stack[top] = stack[top] <= stack[top + 1];
				break;
			} // switch
			break;
		case LOD:
			stack[++top] = stack[base(stack, b, i.l) + i.a];
			break;
        case LOA:
            stack[top] = stack[base(stack, b, i.l) + stack[top]];
           // top++;
			break;
		case LAD:
            stack[top+1] = stack[base(stack, b, i.l) + stack[top]];
            top++;
			break;
		case STO:
			if(stack[top] == 0xffff)
				printf("\n");
				else
				{
				stack[base(stack, b, i.l) + i.a] = stack[top];
				printf("%d\n", stack[top]);
			}
			//top--;deleted    2017.11.2
			break;
        case STA:
            stack[base(stack, b, i.l) + stack[top-i.a-1]] = stack[top];
			printf("%d\n", stack[top]);
			//top--;deleted    2017.11.2
			break;
		case CPY:
			stack[top+1] = stack[top];
			top++;
			break;
		case CAL://2017.11.3
			// generate callee's AR at top of runtime stack!
			stack[top + 1] = 0; //return value
			stack[top + 2] = base(stack, b, i.l); //set up SL, static link
			stack[top + 3] = b; //DL, dynamic link, saving base address for caller's AR
			stack[top + 4] = pc;//save return address,next instruction after CAL
			b = top + 2; // new base poniter points to SL
			//top += 4 ;
			pc = i.a;    // reset ip
			//notice, when CAL executing, the top of stack is not changed
			break;
        case CAP://2017.11.3
			// generate callee's AR at top of runtime stack!
			 //return value
			stack[top + 1] = base(stack, b, i.l); //set up SL, static link
			stack[top + 2] = b; //DL, dynamic link, saving base address for caller's AR
			stack[top + 3] = pc;//save return address,next instruction after CAL
			b = top + 1; // new base poniter points to SL
			//top += 4 ;
			pc = stack[base(stack, b, i.l) + stack[top]];
			stack[top] = 0;    // reset ip
			//notice, when CAL executing, the top of stack is not changed
			break;
		case INT:
			top += i.a;//in general, as the first instruction of a function
			//to allocate all AR space, including SL, DL, RA, variables
			break;
		case JMP:
			pc = i.a;
			break;
		case JPC:
			if (stack[top] == 0)
				pc = i.a;
			top--;
			break;
		case POP://added  	2017.11.2
			top--;
			break;
		case RAN:
			if(stack[top])
			stack[++top] = rand()%stack[top-1];
			else
			stack[++top] = rand();
			break;
		case CAS:
			int t = top;
			int b0 = b;
			printf("%5d TOP:  %d\n",t,stack[t]);
			--t;
			while(t != 0)
			{
				while(t != b0+2)
				{
					printf("%5d       %d\n",t,stack[t]);
					--t;
				}
				printf("%5d  PC:  %d\n",t,stack[t]);
				--t;
				printf("%5d  DL:  %d\n",t,stack[t]);
				b0 = stack[t];
				--t;
				printf("%5d  SL:  %d\n",t,stack[t]);
				--t;
				printf("%5d  RE:  %d\n",t,stack[t]);
				--t;
			}
		/*	printf("\n");
			t = 100;
			while(stack[t] == 0)
				t--;
			while(t != 0)
			{
				printf("%5d       %d\n",t,stack[t]);
				t--;
			}*/
			break;
		} // switch
	} while (pc);

	printf("End executing PL/0 program.\n");
} // interpret

//////////////////////////////////////////////////////////////////////
int main()
{
	FILE* hbin;
	char s[80];
	int i;
	symset set, set1, set2;

	printf("Please input source file name: "); // get file name to be compiled
	scanf("%s", s);
	if ((infile = fopen(s, "r")) == NULL)
	{
		printf("File %s can't be opened.\n", s);
		exit(1);
	}

	phi = createset(SYM_NULL);
	relset = createset(SYM_EQU, SYM_NEQ, SYM_LES, SYM_LEQ, SYM_GTR, SYM_GEQ, SYM_NULL);

	// create begin symbol sets
	declbegsys = createset(SYM_CONST, SYM_VAR, SYM_FUNCTION, SYM_NULL);

	statbegsys = createset(SYM_BEGIN, SYM_IF, SYM_WHILE, SYM_ELIF,SYM_EXIT,
	SYM_FOR, SYM_DO, SYM_BREAK, SYM_CONTINUE, SYM_NULL);

	facbegsys = createset(SYM_IDENTIFIER, SYM_NUMBER, SYM_LPAREN, SYM_MINUS,
	SYM_ODD, SYM_NOT,SYM_INC, SYM_DEC,SYM_RANDOM, SYM_PRINT, SYM_CALLSTACK, SYM_NULL);
	//¿ªÊ¼·ûºÅ±í
	err = cc = cx = ll = 0; // initialize global variables
	ch = ' ';
	kk = MAXIDLEN;

	getsym();

	set1 = createset(SYM_PERIOD, SYM_NULL);
	set2 = uniteset(declbegsys, statbegsys);
	set = uniteset(set1, set2);
	block(set);
	destroyset(set1);
	destroyset(set2);
	destroyset(set);
	destroyset(phi);
	destroyset(relset);
	destroyset(declbegsys);
	destroyset(statbegsys);
	destroyset(facbegsys);

	if (sym != SYM_PERIOD)
		error(9); // '.' expected.
	if (err == 0)
	{
		hbin = fopen("hbin.txt", "w");
		for (i = 0; i < cx; i++)
			fwrite(&code[i], sizeof(instruction), 1, hbin);
		fclose(hbin);
	}
	if (err == 0)
		interpret();
	else
		printf("There are %d error(s) in PL/0 program.\n", err);
	listcode(0, cx);
	return 0;
} // main

//////////////////////////////////////////////////////////////////////
// eof pl0.c
