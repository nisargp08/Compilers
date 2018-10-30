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
static int iskeyword(char * kw_lexeme); /*keywords lookup functuion */


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

			/* End of file check*/
			if (c == (unsigned char)SEOF || c == '\0') {
				t.code = SEOF_T;
				/*t.attribute.seof = SEOF1;
				t.attribute.seof = SEOF1;*/
				break;
			}

			/* isspace() will return true if ' ' , '\t' , '\n' , '\f' , '\v' , '\r' is encountered */
			if (isspace(c)) {
				/* Incrementing line counter if '\n' is detected */
				if (c == '\n') {
					line++;
				}
				continue;
			}

			/* Switch case to check all the special cases */
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
				if (c == '==') {
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
				/* Checking for relational operator '.OR.' here */
				else if (b_getc(sc_buf) == 'O' && b_getc(sc_buf) == 'R' && b_getc(sc_buf) == '.') {
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
				/* Checking if the element matches the comment pattern meaning '!!' */
				if (b_getc(sc_buf) == '!') {
					/* Looping untill new line is found*/
					while (b_getc(sc_buf) != '\n') {
						/* If file ends before new line than it returns Source EOF token*/
						if (c == (unsigned char)SEOF) {
							t.code = SEOF_T;
							return t;
						}
					}
					line++;
					continue;
				}
				/* When pattern is not matching meaning only one '!' was found.So returning error token and storing the elemennt in err_lex array*/
				else {
					t.code = ERR_T;
					t.attribute.err_lex[0] = '!';
					t.attribute.err_lex[1] = c;
					t.attribute.err_lex[2] = '\0';
					/* While loop to reach the end of line so that we can increment the line counter*/
					while (c != '\n') {
						c = b_getc(sc_buf);
						if (c == (unsigned char)SEOF) {
							t.code = SEOF_T;
							return t;
						}
					}
					line++;
					return t;
				}
				/* String token */
			case '"':
				/* Saving the starting position of the of the string : Character after '"' in lexstart*/
				/*lexstart = b_getcoffset(sc_buf);*/
				lexstart = b_mark(sc_buf, b_getcoffset(sc_buf));
				/* Looping untill next '"' is found */
				while (c = b_getc(sc_buf) != '"') {
					if (c == '\n') {
						line++;
					}
					else if (c == '\0' || c == (unsigned char)SEOF) {
						t.code = ERR_T;
						lexend = b_getcoffset(sc_buf);
						b_reset(sc_buf);
						lexstart = b_retract(sc_buf);
						if ((lexend - lexstart) > 20) {
							for (i = 0; i < 17; i++) {
								t.attribute.err_lex[i] = b_getc(sc_buf);
							}
							t.attribute.err_lex[i] = '\0';
							t.attribute.err_lex[i++] = '.';
							t.attribute.err_lex[i++] = '.';
							t.attribute.err_lex[i++] = '.';
						}
						else {
							int j = 0;
							for (i = 0; i < 20; i++) {
								c = b_getc(sc_buf);
								if (c == '\n')
									continue;
								t.attribute.err_lex[j++] = c;
							}
							t.attribute.err_lex[j] = '\n';
						}
						if (c = (unsigned char)SEOF) {
							b_retract(sc_buf);
						}
						else {
							while (c != (unsigned char)SEOF) {
								c = b_getc(sc_buf);
							}
							b_retract(sc_buf);
						}
						return t;
					} /* SEOF Else if ends*/
					else if (c == '"') {
						/* Second '"' found here so retracting one element as we dont want last '"' in our string*/
						b_retract(sc_buf);
						/* Saving the ending position of the of the string : Character before '"' in lexstart*/
						lexend = b_getcoffset(sc_buf);
						/* Now setting getcoffset to the marked position i.e lexstart so that we can loop through the string to store each character*/
						b_mark(sc_buf, b_getcoffset(sc_buf));
						/* setting token to STR_T*/
						t.code = STR_T;
						t.attribute.str_offset = b_limit(str_LTBL);
						/* Looping from beginning to end of string and storing it inside str_LTBL buffer)*/
						for (i = lexstart; i < lexend; i++) {
							b_addc(str_LTBL, b_getc(sc_buf));
						}
						/* Adding '\0' at the end to make it a C type string*/
						b_addc(str_LTBL, '\0');
						b_getc(sc_buf);
						return t;
					}
				}

			default:
				t.code = ERR_T;
				t.attribute.err_lex[0] = c;
				t.attribute.err_lex[1] = '\0';
				return t;
			}

			/*********************************************************************/
			/* Part - 2 : Implemenation of DFA or Transition table driven scanner*/
			/*********************************************************************/
			if (isdigit(c) || isalnum(c)) {
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
		}/* While ends*/
		return t;
	}
}/*Function ends*/
				/*SET THE MARK AT THE BEGINING OF THE LEXEME AND SAVE IT IN lexstart
					lexstart = b_mark(sc_buf, ...);
				....
					CODE YOUR FINATE STATE MACHINE HERE(FSM or DFA)
					IT IMPLEMENTS THE FOLLOWING ALGORITHM :

				FSM0.Begin with state = 0 and the input character c
					FSM1.Get the next state from the transition table calling
					state = get_next_state(state, c, &accept);
				FSM2.Get the next character
					FSM3.If the state is not accepting(accept == NOAS), go to step FSM1
					If the step is accepting, token is found, leave the machine and
					call an accepting function as described below.


					RETRACT  getc_offset IF THE FINAL STATE IS A RETRACTING FINAL STATE

					SET lexend TO getc_offset USING AN APPROPRIATE BUFFER FUNCTION

					CREATE  A TEMPORRARY LEXEME BUFFER HERE;
				lex_buf = b_allocate(...);
				.RETRACT getc_offset to the MARK SET PREVIOUSLY AT THE BEGINNING OF THE LEXEME AND
					.USING b_getc() COPY THE LEXEME BETWEEN lexstart AND lexend FROM THE INPUT BUFFER INTO lex_buf USING b_addc(...),
					.WHEN VID(KEYWORDS INCLUDED), FPL OR IL IS RECOGNIZED
					.YOU MUST CALL THE ACCEPTING FUNCTION USING THE ARRAY aa_table, WHICH
					.CONTAINS POINTERS TO FUNCTIONS.THE ARRAY INDEX OF THE FUNCTION TO BE
					.CALLED IS STORED IN THE VARIABLE state.
					.YOU ARE NOT ALLOWED TO CALL ANY OF THE ACCEPTING FUNCTIONS BY NAME.
					.THE ARGUMENT TO THE FUNCTION IS THE STRING STORED IN lex_buf.
					....
					b_free(lex_buf);
				return t;
			}//end while(1)
			IN A CASE OF RUNTIME ERROR, THE FUNCTION MUST STORE
				A NON- NEGATIVE NUMBER INTO THE GLOBAL VARIABLE scerrnum
				AND RETURN A RUN TIME ERROR TOKEN.THE RUN TIME ERROR TOKEN ATTRIBUTE
				MUST BE THE STRING "RUN TIME ERROR: "
		}


		DO NOT MODIFY THE CODE OF THIS FUNCTION
			YOU CAN REMOVE THE COMMENTS
*/
			int get_next_state(int state, char c, int *accept)
			{
				int col;
				int next;
				col = char_class(c);
				next = st_table[state][col];
#ifdef DEBUG
				printf("Input symbol: %c Row: %d Column: %d Next: %d \n", c, state, col, next);
#endif
				/*
				The assert(int test) macro can be used to add run-time diagnostic to programs
				and to "defend" from producing unexpected results.
				assert() is a macro that expands to an if statement;
				if test evaluates to false (zero) , assert aborts the program
				(by calling abort()) and sends the following message on stderr:

				Assertion failed: test, file filename, line linenum

				The filename and linenum listed in the message are the source file name
				and line number where the assert macro appears.
				If you place the #define NDEBUG directive ("no debugging")
				in the source code before the #include <assert.h> directive,
				the effect is to comment out the assert statement.
				*/
				assert(next != IS);

				/*
				The other way to include diagnostics in a program is to use
				conditional preprocessing as shown bellow. It allows the programmer
				to send more details describing the run-time problem.
				Once the program is tested thoroughly #define DEBUG is commented out
				or #undef DEBUF is used - see the top of the file.
				*/
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

			int char_class(char c)
			{
				int val;

				THIS FUNCTION RETURNS THE COLUMN NUMBER IN THE TRANSITION
					TABLE st_table FOR THE INPUT CHARACTER c.
					SOME COLUMNS MAY REPRESENT A CHARACTER CLASS .
					FOR EXAMPLE IF COLUMN 2 REPRESENTS[A - Za - z]
					THE FUNCTION RETURNS 2 EVERY TIME c IS ONE
					OF THE LETTERS A, B, ..., Z, a, b...z.
					PAY ATTENTION THAT THE FIRST COLOMN IN THE TT IS 0 (has index 0)

					return val;
			}



			HERE YOU WRITE THE DEFINITIONS FOR YOUR ACCEPTING FUNCTIONS.
				************************************************************

				ACCEPTING FUNCTION FOR THE arithmentic variable identifier AND keywords(VID - AVID / KW)
				REPLACE XX WITH THE CORRESPONDING ACCEPTING STATE NUMBER

				Token aa_funcXX(char lexeme[]) {

				WHEN CALLED THE FUNCTION MUST
					1. CHECK IF THE LEXEME IS A KEYWORD.
					IF YES, IT MUST RETURN A TOKEN WITH THE CORRESPONDING ATTRIBUTE
					FOR THE KEYWORD.THE ATTRIBUTE CODE FOR THE KEYWORD
					IS ITS INDEX IN THE KEYWORD LOOKUP TABLE(kw_table in table.h).
					IF THE LEXEME IS NOT A KEYWORD, GO TO STEP 2.

					2. SET a AVID TOKEN.
					IF THE lexeme IS LONGER than VID_LEN(see token.h) CHARACTERS,
					ONLY FIRST VID_LEN CHARACTERS ARE STORED
					INTO THE VARIABLE ATTRIBUTE ARRAY vid_lex[](see token.h) .
					ADD \0 AT THE END TO MAKE A C - type STRING.
					return t;
			}

			ACCEPTING FUNCTION FOR THE string variable identifier(VID - SVID)
				REPLACE XX WITH THE CORRESPONDING ACCEPTING STATE NUMBER

				Token aa_funcXX(char lexeme[]) {

				WHEN CALLED THE FUNCTION MUST
					1. SET a SVID TOKEN.
					IF THE lexeme IS LONGER than VID_LEN characters,
					ONLY FIRST VID_LEN - 1 CHARACTERS ARE STORED
					INTO THE VARIABLE ATTRIBUTE ARRAY vid_lex[],
					AND THEN THE $ CHARACTER IS APPENDED TO THE NAME.
					ADD \0 AT THE END TO MAKE A C - type STRING.

					return t;
			}

			ACCEPTING FUNCTION FOR THE floating - point literal(FPL)

				Token aa_funcXX(char lexeme[]) {

				THE FUNCTION MUST CONVERT THE LEXEME TO A FLOATING POINT VALUE,
					WHICH IS THE ATTRIBUTE FOR THE TOKEN.
					THE VALUE MUST BE IN THE SAME RANGE AS the value of 4 - byte float in C.
					IN CASE OF ERROR(OUT OF RANGE) THE FUNCTION MUST RETURN ERROR TOKEN
					THE ERROR TOKEN ATTRIBUTE IS  lexeme.IF THE ERROR lexeme IS LONGER
					than ERR_LEN characters, ONLY THE FIRST ERR_LEN - 3 characters ARE
					STORED IN err_lex.THEN THREE DOTS ... ARE ADDED TO THE END OF THE
					err_lex C - type string.
					BEFORE RETURNING THE FUNCTION MUST SET THE APROPRIATE TOKEN CODE
					return t;
			}

			ACCEPTING FUNCTION FOR THE integer literal(IL) - decimal constant(DIL)

				Token aa_funcXX(char lexeme[]) {

				THE FUNCTION MUST CONVERT THE LEXEME REPRESENTING A DECIMAL CONSTANT
					TO A DECIMAL INTEGER VALUE, WHICH IS THE ATTRIBUTE FOR THE TOKEN.
					THE VALUE MUST BE IN THE SAME RANGE AS the value of 2 - byte integer in C.
					IN CASE OF ERROR(OUT OF RANGE) THE FUNCTION MUST RETURN ERROR TOKEN
					THE ERROR TOKEN ATTRIBUTE IS  lexeme.IF THE ERROR lexeme IS LONGER
					than ERR_LEN characters, ONLY THE FIRST ERR_LEN - 3 characters ARE
					STORED IN err_lex.THEN THREE DOTS ... ARE ADDED TO THE END OF THE
					err_lex C - type string.
					BEFORE RETURNING THE FUNCTION MUST SET THE APROPRIATE TOKEN CODE
					return t;
			}

			ACCEPTING FUNCTION FOR THE string literal(SL)

				Token aa_funcXX(char lexeme[]) {

				THE FUNCTION MUST STORE THE lexeme PARAMETER CONTENT INTO THE STRING LITERAL TABLE(str_LTBL)
					FIRST THE ATTRIBUTE FOR THE TOKEN MUST BE SET.
					THE ATTRIBUTE OF THE STRING TOKEN IS THE OFFSET FROM
					THE BEGINNING OF THE str_LTBL char buffer TO THE LOCATION
					WHERE THE FIRST CHAR OF THE lexeme CONTENT WILL BE ADDED TO THE BUFFER.
					USING b_addc(..)COPY THE lexeme content INTO str_LTBL.
					THE OPENING AND CLOSING " MUST BE IGNORED DURING THE COPING PROCESS. 
					ADD '\0' AT THE END MAKE THE STRING C - type string
					IF THE STING lexeme CONTAINS line terminators THE line COUNTER MUST BE INCTREMENTED.
					SET THE STRING TOKEN CODE.
					return t;
			}

			ACCEPTING FUNCTION FOR THE ERROR TOKEN

				Token aa_funcXX(char lexeme[]) {

				THE FUNCTION SETS THE ERROR TOKEN.lexeme[] CONTAINS THE ERROR
					THE ATTRIBUTE OF THE ERROR TOKEN IS THE lexeme CONTENT ITSELF
					AND IT MUST BE STORED in err_lex.IF THE ERROR lexeme IS LONGER
					than ERR_LEN characters, ONLY THE FIRST ERR_LEN - 3 characters ARE
					STORED IN err_lex.THEN THREE DOTS ... ARE ADDED TO THE END OF THE
					err_lex C - type string.
					IF THE ERROR lexeme CONTAINS line terminators THE line COUNTER MUST BE INCTREMENTED.
					BEFORE RETURNING THE FUNCTION MUST SET THE APROPRIATE TOKEN CODE
					return t;
			}

			HERE YOU WRITE YOUR ADDITIONAL FUNCTIONS(IF ANY).
				FOR EXAMPLE

				int iskeyword(char * kw_lexeme) {}
