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
			if (c == BACKSLASHZERO) {
				t.code = SEOF_T;
				t.attribute.seof = SEOF1;
				return t;
			}
			if (c == (unsigned char)SEOF) {
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
						while ((c = b_getc(sc_buf)) != BACKSLASHZERO) {
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
					}
					/* For getting last '"' to mark the completion of the string*/
					b_getc(sc_buf);
					/* Adding '\0' at the end to make it a C type string*/
					b_addc(str_LTBL, '\0');
					/* Returning token */
					return t;
				}
			default:
				/* DFA Implementation */
				if (isalnum(c)) {
					lexstart = b_mark(sc_buf, b_getcoffset(sc_buf) - 1);
					state = get_next_state(state, c, &accept);

					while (accept == NOAS) {
						c = b_getc(sc_buf);
						state = get_next_state(state, c, &accept);
					}
					if (accept == ASWR) {
						b_retract(sc_buf);
					}
					lexend = b_getcoffset(sc_buf);
					b_retract(sc_buf);
					lex_buf = b_allocate((lexend - lexstart) + 1, 0, 'f');

					if (lex_buf == NULL) {
						char runtimeErrorString[] = "RUN TIME ERROR: ";
						t.code = RTE_T;
						for (i = 0; i < strlen(runtimeErrorString); i++) {
							t.attribute.err_lex[i] = runtimeErrorString[i];
						}
						t.attribute.err_lex[i] = BACKSLASHZERO;
						scerrnum = 1;
						free(lex_buf);
						return t;
					}
					b_reset(sc_buf);
					for (i = 0; i < lexend - lexstart; i++) {
						c = b_getc(sc_buf);
						b_addc(lex_buf, c);
					}
					b_addc(lex_buf,BACKSLASHZERO);

					t = aa_table[state](b_location(lex_buf,0));
					b_free(lex_buf);
					return t;
				}/*isalnum if ends*/

				/* if invalid character is found then error token is returned here*/
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
	int val;
	//define constatns
	/* Column One - [a-zA-z]*/
	if (isalpha(c)) {
		val = ZERO;
	}
	else if (isdigit(c)) {
		if (c == 'ZERO') {
			val = ONE;
		}
		else
			val = TWO;
	}
	else if (c == '.') {
		val = THREE;
	}
	else if (c == '$') {
		val = FOUR;
	}
	else if (c == '"') {
		val = SIX;
	}
	else if (c == SEOF) {
		val = SEVEN;
	}
	/* Others */
	else
		val = FIVE;
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

/********************************************************************************************************************************
Purpose				:	The purpose of this function is to firstly check when called if the lexeme is a keyword ,
						if the lexeme is a keyword , then the attribute is set and the lexeme is stored into the 
						Kwt_id field , If it is not a keyword , set the code to Arithmetic VID and then the lexeme is stored
						into the VID_lex attribute.
Author				:	Nisarg Patel / Divy Shah
History/Versions	:	2018/10/07
Called Function		:	iskeyword() 
Parameters			:	char lexeme[] : The characters that are to be checked
Return Value		:	Token : Token with the code and the attribute set is returned
Algorithm			:	- First we check to make sure if the lexeme is a keyword ,
						- If it is a keyword , Set the code and the attribute and return the token
						- Otherwise Set the code to AVID 
						- Check if the length of the is greater than VID_LEN 
						- If yes , Store the first 8 characters into the attribute
						- Otherwise store the entire lexeme into the attribute
*******************************************************************************************************************************/
Token aa_func02(char lexeme[])
{
	/*Varables used are declared.*/
	Token t;
	int i;

	/*Calls the iskeyword method to check if the given lexeme is a keyword*/
	if (iskeyword(lexeme) != RT_FAIL_1)
	{
		t.code = KW_T;	/*If it is a keyword , then set the code and the attrivutes for it*/
		t.attribute.kwt_idx = iskeyword(lexeme);
		return t;   /*Returns the token after setting values.*/
	}

	/*If it is not a keyword , set the AVID Token*/
	t.code = AVID_T;	/*Sets the token code for arithmetic VID*/
	if (strlen(lexeme) > VID_LEN)   /*Check if the lenght of the lexeme is longer than VID_LEN(8)*/
	{
		for (i = 0; i < VID_LEN; i++)	/*If yes , we only count the first 8 letters*/
		{
			t.attribute.vid_lex[i] = lexeme[i];
		}
		t.attribute.vid_lex[VID_LEN] = '\0';	/*Adds the line terminator*/
	}
	else
	{
		/*If the length of the lexeme is not longer than VID_LEN(8) */
		for (i = 0; i < strlen(lexeme); i++) /*Adds the characters and appends a line terminator at the end*/
		{
			t.attribute.vid_lex[i] = lexeme[i];
		}
		t.attribute.vid_lex[i] = '\0';
	}
	return t;
}
/*
ACCEPTING FUNCTION FOR THE string variable identifier(VID - SVID)
REPLACE XX WITH THE CORRESPONDING ACCEPTING STATE NUMBER

Token aa_funcXX(char lexeme[]) {

WHEN CALLED THE FUNCTION MUST
1. SET a SVID TOKEN.
IF THE lexeme IS 1500LONGER than VID_LEN characters,
ONLY FIRST VID_LEN - 1 CHARACTERS ARE STORED
INTO THE VARIABLE ATTRIBUTE ARRAY vid_lex[],
AND THEN THE $ CHARACTER IS APPENDED TO THE NAME.
ADD \0 AT THE END TO MAKE A C - type STRING.

return t;
}
*/
/********************************************************************************************************************************
Purpose				:	The purpose of this function is to set the code to SVID when called . Then the lexeme is store into 
						VID_LEX attribute.
Author				:	Nisarg Patel / Divy Shah
History/Versions	:	2018/10/07
Called Function		:	-
Parameters			:	char lexeme[] : The characters that are to be checked
Return Value		:	Token : Token with the code and the attribute set is returned
Algorithm			:	- First we set the code to the SVID for svid token
						- Check if the length of the is greater than VID_LEN 
						- If yes , Store the first 8 characters into the attribute
						- Append the $ and the '\0' at the end of the string 
						- Otherwise store the entire lexeme into the attribute
							and set the '\0' at the end.
*******************************************************************************************************************************/
Token aa_func03(char lexeme[]) {

	/*Varables used are declared.*/
	Token t;
	int i;

	/*Sets the Token code to SVID_T */
	t.code = SVID_T;

	if (strlen(lexeme) > VID_LEN)
	{
		for (i = 0; i < VID_LEN - 1; i++)	/*If yes , we only count the VID_LEN - 1 characters*/
		{
			t.attribute.vid_lex[i] = lexeme[i];
		}
		t.attribute.vid_lex[i] = '$';	/*Appends the $ symbol at the end.*/
		t.attribute.vid_lex[i + 1] = '\0';     /*Adds the line terminator*/
	}
	else
	{
		for (i = 0; i < strlen(lexeme); i++) /*Adds the characters and appends a line terminator at the end*/
		{
			t.attribute.vid_lex[i] = lexeme[i];
		}
		t.attribute.vid_lex[i] = '\0';     /*Adds the line terminator*/
	}
	return t; /*Returns the token*/
}


/*
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

*/
/********************************************************************************************************************************
Purpose				:	The purpose of this function is to process thre Floating point Literal , It converts the lexeme into 
						floating point values and set it as the attribute.
Author				:	Nisarg Patel / Divy Shah
History/Versions	:	2018/10/07
Called Function		:	atof()
Parameters			:	char lexeme[] : The characters that are to be checked
Return Value		:	Token : Token with the code and the attribute set is returned
Algorithm			:	- First we convert the lexeme into a floating point value
						- Check if the the calue is in the range of positive and negative floating point number
- Check if the length of the is greater than VID_LEN
- If yes , Store the first 8 characters into the attribute
- Append the $ and the '\0' at the end of the string
- Otherwise store the entire lexeme into the attribute
and set the '\0' at the end.
*******************************************************************************************************************************/
Token aa_func08(char lexeme[]) {

	/*Varables used are declared.*/
	Token t;
	double f = 0.0;

	/*Now convert the string to float*/
	f = atof(lexeme);

	/*Check that the value of the lexeme converted to float is in range */
	if ((f > 0 && (f > FLT_MAX || f < FLT_MIN)) || (f < 0 && (f < -FLT_MAX || f > -FLT_MIN))) {
		t = aa_func11(lexeme);
	}
	else {
		t.code = FPL_T; /*The Error token value is set to the token code*/
		t.attribute.flt_value = f;	/*The attribute is the value of the lexeme */
	}
	return t;
}
/*
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
*/
Token aa_func05(char lexeme[]) {

	/*Varables used are declared.*/
	Token t;
	int l = 0;
	long i = 0;

	/*Now lets convert it to decimal constant*/
	l = atol(lexeme);

	if (l > SHRT_MAX || l < SHRT_MIN)
	{
		t.code = ERR_T; /*The Error token value is set to the token code*/
		t.attribute.int_value = (short)l;	/*The attribute is the value of the lexeme */

		if (strlen(lexeme) > ERR_LEN)
		{
			for (i = 0; i < ERR_LEN - 3; i++)	/*If yes , we only count the ERR_LEN - 3 characters*/
			{
				t.attribute.err_lex[i] = lexeme[i];
			}
			t.attribute.err_lex[i] = '.';	/* Add 3 dots (.) at the end*/
			t.attribute.err_lex[i + 1] = '.';
			t.attribute.err_lex[i + 2] = '.';
			t.attribute.err_lex[i + 3] = '\0'; /*Adds the line terminator*/
		}
		else
		{	/* Otherwise store the error into err_lex*/
			for (i = 0; i < strlen(lexeme); i++)
			{
				t.attribute.err_lex[i] = lexeme[i];
			}
			t.attribute.err_lex[i] = '\0';/*Adds the line terminator*/
		}
	}
	else {
		t.code = INL_T;
		t.attribute.int_value = (short)l;
	}
	return t;
}
/*
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

*/
Token aa_func10(char lexeme[]) {

	/*Varables used are declared.*/
	Token t;
	int i = 0;


	/*The attribute  of the string token is the offset from the beginning to the location*/
	t.attribute.str_offset = b_getcoffset(str_LTBL);
	//printf("\n attribute : %d \n",b_getcoffset(str_LTBL));
	t.code = STR_T; /*The code is set to the String token code*/

					/*Loop to iteratre through the lexeme*/
	for (i = 0; i < strlen(lexeme); i++)
	{
		/*If a double quote is acquired , do not add it to the buffer*/
		if (lexeme[i] == '"')
		{
			continue;
		}
		if (lexeme[i] == '\0')
		{
			line++;
		}
		b_addc(str_LTBL, lexeme[i]);
	}
	b_addc(str_LTBL, '\0');
	line++;
	return t;
}


/*
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

*/
Token aa_func11(char lexeme[]) {

	/*Varables used are declared.*/
	Token t;
	int i = 0;

	/*set the code to the error token*/
	t.code = ERR_T;

	if (strlen(lexeme) > ERR_LEN)
	{
		for (i = 0; i < ERR_LEN - 3; i++)
		{
			t.attribute.err_lex[i] = lexeme[i];
		}
		t.attribute.err_lex[i] = '.';	/* Add 3 dots (.) at the end*/
		t.attribute.err_lex[i + 1] = '.';
		t.attribute.err_lex[i + 2] = '.';
		t.attribute.err_lex[i + 3] = '\0'; /*Adds the line terminator*/
		line++;
	}
	else
	{
		/* Otherwise store the error into err_lex*/
		for (i = 0; i < strlen(lexeme); i++)
		{
			t.attribute.err_lex[i] = lexeme[i];
		}
		t.attribute.err_lex[i] = '\0';/*Adds the line terminator*/
		line++;
	}
	return t;
}


Token aa_func12(char lexeme[]) {
	/*Varables used are declared.*/
	Token t;

	/*We call the */
	t = aa_func11(lexeme); 
	b_retract(sc_buf);
	return t;
}
int iskeyword(char * kw_lexeme) {
	int i;
	/*For loop to compare the lexeme to the Keyword*/
	for (i = 0; i < KWT_SIZE; i++)
	{
		if (strcmp(kw_lexeme, kw_table[i]) == 0)	/*If it is found , set the attributes and return the token */
		{
			return i;
		}
	}
	return RT_FAIL_1;
}
