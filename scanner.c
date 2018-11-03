/* Filename: scanner.c
/* PURPOSE:
*    SCANNER.C: Functions implementing a Lexical Analyzer (Scanner)
*    as required for CST8152, Assignment #2
*    scanner_init() must be called before using the scanner.
*    The file is incomplete;
*    Provided by: Svillen Ranev
*    Version: 1.18.2
*    Date: 1 October 2018
*******************************************************************
*    REPLACE THIS HEADER WITH YOUR HEADER
*******************************************************************
*/

/* The #define _CRT_SECURE_NO_WARNINGS should be used in MS Visual Studio projects
* to suppress the warnings about using "unsafe" functions like fopen()
* and standard sting library functions defined in string.h.
* The define does not have any effect in Borland compiler projects.
*/
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>   /* standard input / output */
#include <ctype.h>   /* conversion functions */
#include <stdlib.h>  /* standard library functions and constants */
#include <string.h>  /* string functions */
#include <limits.h>  /* integer types constants */
#include <float.h>   /* floating-point types constants */

/*#define NDEBUG        to suppress assert() call */
#include <assert.h>  /* assert() prototype */

/* project header files */
#include "buffer.h"
#include "token.h"
#include "table.h"

#define DEBUG  /* for conditional processing */
#undef  DEBUG

/* Global objects - variables */
/* This buffer is used as a repository for string literals.
It is defined in platy_st.c */
extern Buffer * str_LTBL; /*String literal table */
int line; /* current line number of the source code */
extern int scerrnum;     /* defined in platy_st.c - run-time error number */

						 /* Local(file) global objects - variables */
static Buffer *lex_buf;/*pointer to temporary lexeme buffer*/
static pBuffer sc_buf; /*pointer to input source buffer*/
					   /* No other global variable declarations/definitiond are allowed */

					   /* scanner.c static(local) function  prototypes */
static int char_class(char c); /* character class function */
static int get_next_state(int, char, int *); /* state machine function */
											 /*static int iskeyword(char * kw_lexeme); /*keywords lookup functuion */


											 /*Initializes scanner */
int scanner_init(Buffer * psc_buf) {
	if (b_isempty(psc_buf)) return EXIT_FAILURE;/*1*/
												/* in case the buffer has been read previously  */
	b_rewind(psc_buf);
	b_clear(str_LTBL);
	line = 1;
	sc_buf = psc_buf;
	return EXIT_SUCCESS;/*0*/
						/*   scerrnum = 0;  *//*no need - global ANSI C */
}

Token malar_next_token(void) {
	{
		Token t = { 0 }; /* token to return after pattern recognition. Set all structure members to 0 */
		unsigned char c; /* input symbol */
		int state = 0; /* initial state of the FSM */
		short lexstart;  /*start offset of a lexeme in the input char buffer (array) */
		short lexend;    /*end   offset of a lexeme in the input char buffer (array)*/
		int accept = NOAS; /* type of state - initially not accepting */

						   /*DECLARE YOUR LOCAL VARIABLES HERE IF NEEDED*/
						   /* Variable used in loops*/
		int i = 0;
		while (1) { /* endless loop broken by token returns it will generate a warning */
			c = b_getc(sc_buf);

			/* Part 1: Implementation of token driven scanner */
			if (c == (unsigned char)SEOF) {
				t.code = SEOF_T;
				t.attribute.seof = SEOF1;
				return t;
			}
			if (c == BACKSLASHZERO) {
				t.code = SEOF_T;
				t.attribute.seof = SEOF2;
				return t;
			}
			/* isspace() will return true if ' ' , '\t' , '\n' , '\f' , '\v' , '\r' is encountered */
			if (isspace(c)) {
				/* Incrementing line counter if '\n' is detected */
				if (c == '\n') {
					line++;
				}
				continue;
			}
			/* DFA Implementation - When the character is an alphabet or a number */
			if (isalnum(c)) {
				lexstart = b_mark(sc_buf, b_getcoffset(sc_buf));
				c = b_getc(sc_buf);
				state = get_next_state(state, c, &accept);
				while (accept == NOAS) {
					state = get_next_state(state, c, &accept);
					if (accept != NOAS) {
						break;
					}
					c = b_getc(sc_buf);
				}

				if (accept == ASWR) {
					b_retract(sc_buf);
				}
				lexend = b_getcoffset(sc_buf);
				lex_buf = b_allocate((lexend - lexstart), 0, 'f');

				if (lex_buf == NULL) {
					char runtimeErrorString[] = "RUN TIME ERROR: ";
					t.code = RTE_T;
					for (i = 0; i < strlen(runtimeErrorString); i++) {
						t.attribute.err_lex[i] = runtimeErrorString[i];
					}
					t.attribute.err_lex[i] = '\0';
					scerrnum = 1;
					free(lex_buf);
				}
				b_reset(sc_buf);
				for (i = lexstart; i < lexend; i++) {
					c = b_getc(sc_buf);
					b_addc(lex_buf, c);
				}
				b_addc(lex_buf, '\0');
				aa_table[state]((lex_buf));
				b_free(lex_buf);
				return t;
			}
			t.code = ERR_T;
			t.attribute.err_lex[0] = c;
			t.attribute.err_lex[1] = '\0';
			return t;
			}

			/* Switch case begins */
			switch (c) {
				/* Left parenthesis token */
			case '(':
				t.code = LPR_T;
				return t;
				/* Right parenthesis token */
			case ')':
				t.code = RPR_T;
				return t;
				/* Left brace token */
			case '{':
				t.code = LBR_T;
				return t;
				/* Right brace token */
			case '}':
				t.code = RBR_T;
				return t;
				/* End of statement token*/
			case ';':
				t.code = EOS_T;
				return t;
				/* Comma token*/
			case ',':
				t.code = COM_T;
				return t;
				/* String concatenation token */
			case '#':
				t.code = SCC_OP_T;
				return t;

				/* Arithmetic Operators Token */
			case '+':
				t.code = ART_OP_T;
				t.attribute.arr_op = PLUS;
				return t;
			case '-':
				t.code = ART_OP_T;
				t.attribute.arr_op = MINUS;
				return t;
			case '*':
				t.code = ART_OP_T;
				t.attribute.arr_op = MULT;
				return t;
			case '/':
				t.code = ART_OP_T;
				t.attribute.arr_op = DIV;
				return t;

				/* Relational Operators Token */
			case '=':
				/*	Fetching one more character from the buffer to check if the next character is another '=' or not.
				If it is then relational operator token with attribute 'EQ' will be returned */
				c = b_getc(sc_buf);
				if (c == '=') {
					t.code = REL_OP_T;
					t.attribute.rel_op = EQ;
					return t;
				}
				/* If not then assignment operator token will be returned */
				b_retract(sc_buf);
				t.code = ASS_OP_T;
				return t;

			case '<':
				/*	Fetching one more character from the buffer to check if the next character is '>' or not.
				If it is then relational operator token with attribute 'NE' will be returned */
				c = b_getc(sc_buf);
				if (c == '>') {
					t.code = REL_OP_T;
					t.attribute.rel_op = NE;
					return t;
				}
				/* If not then relational operator token with attribute 'LT' will be returned */
				b_retract(sc_buf);
				t.code = REL_OP_T;
				t.attribute.rel_op = LT;
				return t;
				/* Greater than '>' token */
			case '>':
				t.code = REL_OP_T;
				t.attribute.rel_op = GT;
				return t;

				/* Logical Operators Token */
			case '.':
				/* Setting the mark to '.' position here so we can retract back to this position in case of an error or pattern mismatch */
				/* Ask sir if it necessary to store the return value of b_mark if not using it */
				b_mark(sc_buf, b_getcoffset(sc_buf));
				/* Checking for relational operator '.AND.' here */
				if (b_getc(sc_buf) == 'A' && b_getc(sc_buf) == 'N' && b_getc(sc_buf) == 'D' && b_getc(sc_buf) == '.') {
					t.code = LOG_OP_T;
					t.attribute.rel_op = AND;
					return t;
				}
				else {
					b_reset(sc_buf);
				}
				/* Checking for relational operator '.OR.' here */
				if (b_getc(sc_buf) == 'O' && b_getc(sc_buf) == 'R' && b_getc(sc_buf) == '.') {
					t.code = LOG_OP_T;
					t.attribute.rel_op = OR;
					return t;
				}
				/* If none of the above pattern matches then resetting the getc_offset to the marked position meaning '.' */
				b_reset(sc_buf);
				/* Returning error token as illegal character is found and storing it in error lex array */
				t.code = ERR_T;
				t.attribute.err_lex[0] = c;
				t.attribute.err_lex[1] = '\0';
				return t;

				/* Comment token '!!' */
			case '!':
				/* Setting mark so that we can retract back*/
				b_mark(sc_buf, b_getcoffset(sc_buf));
				/* Checking if the element matches the comment pattern meaning '!!' */
				if (b_getc(sc_buf) == '!') {
					/* Looping untill new line is found*/
					while ((c = b_getc(sc_buf)) != '\n') {
						if (c == BACKSLASHZERO) {
							t.code = SEOF_T;
							t.attribute.seof = SEOF2;
							return t;
						}
						continue;
					}
					line++;
					continue;
				}
				/* When pattern is not matching meaning only one '!' was found.So returning error token and storing the elemennt in err_lex array*/
				else {
					b_reset(sc_buf);
					t.code = ERR_T;
					t.attribute.err_lex[0] = c;
					t.attribute.err_lex[1] = BACKSLASHZERO;
					return t;
				}

			case '"':
				/* Marking the beginning of the string*/
				lexstart = b_mark(sc_buf, b_getcoffset(sc_buf));
				c = b_getc(sc_buf);
				while (c != '"') {
					/* Incrementing line counter everytime '\n' is found*/
					if (c == '\n') {
						line++;
					}
					/* When file ends without finding another '"' */
					else if (c == BACKSLASHZERO || c == (unsigned char)SEOF) {
						t.code = ERR_T;
						/* String end stored in lexend*/
						lexend = b_getcoffset(sc_buf);
						/* Reseting the getcoffset at the beginning of the string*/
						b_reset(sc_buf);
						/* When illegal string is greater than 20 in lenght then array will only hold the first 17 elements and ... for the rest 3 elements*/
						if ((lexend - lexstart) > 20) {
							for (i = 0; i < 17; i++) {
								t.attribute.err_lex[i] = b_getc(sc_buf);
							}
							t.attribute.err_lex[i] = '\0';
							t.attribute.err_lex[i++] = '.';
							t.attribute.err_lex[i++] = '.';
							t.attribute.err_lex[i++] = '.';
						}
						/* When illegal string is smaller than 20 in length then it will be stored in array*/
						else {
							int j = 0;
							for (i = lexstart; i < lexend; i++) {
								c = b_getc(sc_buf);
								if (c == '\n')
									continue;
								t.attribute.err_lex[j++] = c;
							}
						}
						/* This is for setting the getcoffset to the element prior to '\0' */
						while (c = b_getc(sc_buf) != BACKSLASHZERO) {
							c = b_getc(sc_buf);
						}
						b_retract(sc_buf);
						/*Token returned*/
						return t;
					} /* SEOF Else if ends*/
					/* Incrementing to next character in the string*/
					c = b_getc(sc_buf);
				}

				/* Second '"' found here */
				if (c == '"') {
					/* setting token to STR_T*/
					t.code = STR_T;
					t.attribute.str_offset = b_limit(str_LTBL);
					/* so retracting one element as we dont want last '"' in our string and setting it to lexend*/
					lexend = b_retract(sc_buf);
					/* When the string is not empty*/
					if (lexstart != lexend) {
						/* Moving the getcoffset to beginning of the string meanign lexstart*/
						b_reset(sc_buf);
						/* Looping from beginning to end of string and storing it inside str_LTBL buffer)*/
						for (i = lexstart; i < lexend; i++) {
							b_addc(str_LTBL, b_getc(sc_buf));
						}
						/* For getting last '"' to mark the completion of the string*/
						b_getc(sc_buf);
					}
					/* Adding '\0' at the end to make it a C type string*/
					b_addc(str_LTBL, '\0');
					return t;
				}
			default :
				t.code = ERR_T;
				t.attribute.err_lex[0] = c;
				t.attribute.err_lex[1] = BACKSLASHZERO;
				return t;
			}
			/* Switch case ends */
		}/* While ends*/
	}/* Second { ends here */
}/*Function ends*/

static int char_class(char c) {
	int val = 0;
	/* State 2 */
	if (isalpha(c)) {
		val = 2;
	}
	/* State 5 */
	else if (isdigit(c)) {
		val = 5;
	}

	/*THIS FUNCTION RETURNS THE COLUMN NUMBER IN THE TRANSITION
	TABLE st_table FOR THE INPUT CHARACTER c.
	SOME COLUMNS MAY REPRESENT A CHARACTER CLASS .
	FOR EXAMPLE IF COLUMN 2 REPRESENTS[A - Za - z]
	THE FUNCTION RETURNS 2 EVERY TIME c IS ONE
	OF THE LETTERS A, B, ..., Z, a, b...z.
	PAY ATTENTION THAT THE FIRST COLOMN IN THE TT IS 0 (has index 0)
	*/
	return val;
}


int get_next_state(int state, char c, int *accept)
{
	int col;
	int next;
	col = char_class(c);
	next = st_table[state][col];
#ifdef DEBUG
	printf("Input symbol: %c Row: %d Column: %d Next: %d \n", c, state, col, next);
#endif
	assert(next != IS);
#ifdef DEBUG
	if (next == IS) {
		printf("Scanner Error: Illegal state:\n");
		printf("Input symbol: %c Row: %d Column: %d\n", c, state, col);
		exit(1);
	}
#endif
	*accept = as_table[next];
	return next;
}

Token aa_func02(char *lexeme) {}	/* VID AVID/KW */
Token aa_func03(char *lexeme) {}	/* VID SVID */
Token aa_func05(char *lexeme) {}	/* DIL */
Token aa_func08(char *lexeme) {}	/* FPL */
Token aa_func11(char *lexeme) {}	/* ES */
Token aa_func12(char *lexeme) {}	/* ER */