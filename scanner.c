/********************************************************************************************************************************

Filename				:	scanner.c
Compiler				:	MS Visual Studio 2015
Author / Student name	:	Nisarg Patel,040859993
							Divy Shah, 040859087
Course					:	CST 8152 - Compilers
Lab section				:	13(Nisarg) , 14(Divy)
Assignment				:	2
Date					:	2018/10/8
Professor				:	Sv. Ranev
Purpose					:	To create a functioning Scanner for the buffer and understand the concepts.
Functions list				:	scanner_init() , malar_next_token() , char_class() , get_next_state() ,
								aa_func02() , aa_func03() , aa_func05() , aa_func08() , aa_func10() ,
								aa_func11() , aa_func12()

*******************************************************************************************************************************/
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
#ifndef BUFFER_H_
#include "buffer.h"
#endif

#ifndef TOKEN_H_
#include "token.h"
#endif

#ifndef TABLE_H_
#include "table.h"
#endif

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
static int iskeyword(char * kw_lexeme);/*static int iskeyword(char * kw_lexeme); /*keywords lookup functuion */

/********************************************************************************************************************************
Purpose			:The purpose of this function is to initialize the scanner
Author			: Sv. Ranev
History / Versions	: 2018 / 10 / 07
Called Function		: b_isempty() -  To check if the buffer is empty
						b_rewind() - To rewind the buffer
						b_clear() - This function is used to re-initialize all the variable of the buffer to 0.This will not free the space
Parameters		: Buffer * psc_buf - The buffer
Return Value		: EXIT_SUCCESS - When everything is successful , return 0
Algorithm		: - Check to see if the buffer is empty
					- If not , Rewind the buffer
					- Clear the buffer company
					- set the line at 1
					- set the buffer and return zero if successful
* ******************************************************************************************************************************/
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

/********************************************************************************************************************************
Purpose				:The purpose of this function is to process each character token by token and process and
by calling the accepting functions if necessary
Author				:Nisarg Patel / Divy Shah
History/Versions		:2018/10/07
Called Function			:b_getc() -  To get the character from the buffer
						isspace() - to check for spaces
						b_retract() - To get the previous character in the buffer
						b_mark() - This function is used to return the markoffset of the buffer
						b_reset() - This function is used to set the value of markc_offset to getc_offset
						b_getcoffset() - This function is used to return the value of getc_offset
						b_addc() - Will be used to add characters to the buffer
Parameters			: void
Return Value			: Token - Token with appropriate attributes is returned
Algorithm			:- Scan for special characters and sets appropriate tokens
						- Increments a counter when a new line is encountered
						- Processes the Transition tables and calls the necessary accepting function
*******************************************************************************************************************************/
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
					/* Retracting the getcoffset back to marked position*/
					b_reset(sc_buf);
					/* Getting one character to store in err_lex array */
					c = b_getc(sc_buf);
					t.code = ERR_T;
					t.attribute.err_lex[0] = '!';
					t.attribute.err_lex[1] = c;
					t.attribute.err_lex[2] = BACKSLASHZERO;
					/* To iterate to the end of the line */
					while (c != '\n' && c != BACKSLASHZERO && c != (unsigned char)SEOF) {
						/* If space is encountered skip it*/
						if (isspace(c)) {
							c = b_getc(sc_buf);
							continue;
						}
						/*Else get another character*/
						c = b_getc(sc_buf);
					}
					/*Return token*/
					return t;
				}

			default:
				/* DFA Implementation */
				/* Setting the mark at the beginning of the character*/
				lexstart = b_mark(sc_buf, b_getcoffset(sc_buf) - 1);
				/* Getting the next state for the element*/
				state = get_next_state(state, c, &accept);
				/*Looping untill accepting state is found*/
				while (accept == NOAS) {
					c = b_getc(sc_buf);

					state = get_next_state(state, c, &accept);
				}
				/* If accepting state is with retract then performing a retract*/
				if (accept == ASWR) {
					b_retract(sc_buf);
				}
				/*Setting lexend at the end of the character*/
				lexend = b_getcoffset(sc_buf);
				/*Creating temp buffer to hold the string elements*/
				lex_buf = b_allocate((lexend - lexstart) + 1, 0, 'f');
				/*In case of a fail memory allocation it will return RUNTIME Token and store the string in err_lex array*/
				if (lex_buf == NULL) {
					char runtimeErrorString[] = "RUN TIME ERROR: ";
					t.code = RTE_T;
					int len = strlen(runtimeErrorString);
					for (i = 0; i < len; i++) {
						t.attribute.err_lex[i] = runtimeErrorString[i];
					}
					t.attribute.err_lex[i] = BACKSLASHZERO;
					/*Storing non negative number in scerrnum*/
					scerrnum = 1;
					/*Freeing up the buffer*/
					free(lex_buf);
					/*Returning Token*/
					return t;
				}
				/*Setting c at the beginning of the string meaming lexstart*/
				b_reset(sc_buf);
				/*Storing the string in lexbuffer*/
				for (i = 0; i < lexend - lexstart; i++) {
					c = b_getc(sc_buf);
					b_addc(lex_buf, c);
				}
				/*Including a backslashzero to make it C type string*/
				b_addc(lex_buf, BACKSLASHZERO);
				/*Calling appropriate aa functions according tostate and column*/
				t = aa_table[state](b_location(lex_buf, 0));
				/*Free up temp buffer*/
				b_free(lex_buf);
				/*Returning token t*/
				return t;
			}
			/* Switch case ends */
		}/* While ends*/
	}/* Second { ends here */
}/*Function ends*/

 /********************************************************************************************************************************
 Purpose				:The purpose of this function is to return the value of the column for a partucylar character c
 Author					:Nisarg Patel
 History/Versions		:2018/10/07
 Called Function		:isalpha() - If it is an alphabet
						 isdigit() - To check if it is a digit
 Parameters				:char c : The characters that is to be checked
 Return Value			: val - The value of the columnf for the character
	 Algorithm			:-If the char is alphabet we return zero
						 -If the Character is a zero digit we return one
						 -If the Character is a nonzero digit we return two
						 -If the character is a (.) Dot , We return three
						 -If the character is a ($) Dollar sign , We return Four
						 -If the character is a (") Double quote , We return six
						 - If the character is SEOF , we return seven
						 -For all others we return Five.
 *******************************************************************************************************************************/
static int char_class(char c) {
	int val;
	//define constatns
	/* Column Zero - [a-zA-z]*/
	if (isalpha(c)) {
		val = ZERO;
	}
	else if (isdigit(c)) {
		if (c == CHARZERO) {
			/*Column One - 0*/
			val = ONE;
		}
		else
			/*Column Two - [1-9]*/
			val = TWO;
	}
	else if (c == '.') {
		/*Column Three - .*/
		val = THREE;
	}
	else if (c == '$') {
		/*Column Four - $*/
		val = FOUR;
	}
	else if (c == '"') {
		/*Column Five - "*/
		val = SIX;
	}
	else if (c == SEOF || c == BACKSLASHZERO) {
		/*Column Seven - 255,/0*/
		val = SEVEN;
	}
	/* Others */
	else
		/*Column Five - Others*/
		val = FIVE;
	return val;
}

/********************************************************************************************************************************
Purpose				:The purpose of this function is to return the Suitable next state from the state table
Author				:Ranev Sv.
History/Versions	:	2018/10/07
Called Function		:	assert()
Parameters			:state: int - The current state
					c: char - The current character
					*accept - int - the value of the accepting state
Return Value		:	int : the value of the next suitable state is returned
Algorithm			: -First we get the value of col from the char_class
					- Then we get the value of the next state depending on the values of column
					and the state.
					- Now check if the state is Illegeal
					- If not , we change the value of the accepting state
					- Return the next value
*******************************************************************************************************************************/
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
Author				:	Divy Shah
Tested By			:	Nisarg Patel
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
	unsigned int i;

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
Author				:	Nisarg Patel
Tested By			:	Divy Shah
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
	unsigned int i;

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
Author				:	Divy Shah
Tested By			:	Nisarg Patel
History/Versions	:	2018/10/07
Called Function		:	atof() - Convert string to float
					:	aa_func11() : Error checking
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
	float a = (float)f;
	/*Check that the value of the lexeme converted to float is in range */
	if ((f > 0 && (f > FLT_MAX || f < FLT_MIN)) || (f < 0 && (f < -FLT_MAX || f > -FLT_MIN))) {
		t = aa_func11(lexeme);
	}
	else {
		t.code = FPL_T; /*The Error token value is set to the token code*/
		t.attribute.flt_value = a;	/*The attribute is the value of the lexeme */
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
/********************************************************************************************************************************
Purpose					:The purpose of this function is to process the Decimal Integer Literal . When ccalled , It converts
the lexeme into a Decimal integer value and set it as the sttribute.
Author   			:	Nisarg Patel
Tested By			:	Divy Shah
History/Versions		:2018/10/07
Called Function			:atol() -  converts the string argument str to a long integer
Parameters				:char lexeme[] : The characters that are to be checked
						: aa_func11() : Error checking
Return Value			:Token : Token with the code and the attribute set is returned
Algorithm			: - First we convert the lexeme into a Long Integer value
						- Check if the the calue is in the range of the 2Byte Integer in C
						- Check if the length of the is greater than ERR_LEN
						- If yes , Store the first 20 characters into the attribute including
						- the appended 3 dots (. , . , .) and the '\0' at the end of the string
						- Otherwise store the entire lexeme into the attribute
						and set the '\0' at the end.
*******************************************************************************************************************************/
Token aa_func05(char lexeme[]) {

	/*Varables used are declared.*/
	Token t;
	int l = 0;

	/*Now lets convert it to decimal constant*/
	l = atol(lexeme);

	if (l > SHRT_MAX || l < SHRT_MIN)
	{
		t = aa_func11(lexeme);
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

/********************************************************************************************************************************
Purpose				:The purpose of this function is to store the lexeme content into the String literal table .
It then sets the appropriate code and attribute for the token and returns it.
Author				:	Divy Shah
Tested By			:	Nisarg Patel
History/Versions		:2018/10/07
Called Function			:-
Parameters			:char lexeme[] : The characters that are to be checked
Return Value			:Token : Token with the code and the attribute set is returned
Algorithm			: - For processing the string literal , The offset from the beginning of the str_LTBL is stored
						as the offset for the token
						- the code is set to the String token code
						- Now we iterate through the length of the lexeme
						- For every lexeme we check to make sure it is not a " Quote
						- Then we add the characters to the str_LTBL by calling the b_addc method
						- Append the '\0' at the end to make it a ctype string
						- Return the token
*******************************************************************************************************************************/
Token aa_func10(char lexeme[]) {

	/*Varables used are declared.*/
	Token t;
	unsigned int i = 0;

	/*The attribute  of the string token is the offset from the beginning to the location*/
	t.attribute.str_offset = b_limit(str_LTBL);
	/*The code is set to the String token code*/
	t.code = STR_T;

	/*Loop to iteratre through the lexeme*/
	for (i = 0; i < strlen(lexeme); i++)
	{
		/*If a double quote is acquired , do not add it to the buffer*/
		if (lexeme[i] == '"')
		{
			continue;
		}
		/* NEw line character to increment the line counter*/
		if (lexeme[i] == '\n')
		{
			line++;
		}
		b_addc(str_LTBL, lexeme[i]);
	}
	b_addc(str_LTBL, '\0');
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

/********************************************************************************************************************************
Purpose				:The purpose of this function is to set the error token . The lexeme is the attibute of the
error token. The characters are stored into the err_lex accordingly
Author				:Nisarg Patel
Tested By			:	Divy Shah
History/Versions		:2018/10/07
Called Function			: -
Parameters			:char lexeme[] : The characters that are to be checked
Return Value			:Token : Token with the code and the attribute set is returned
Algorithm			: - First we set the token code to that of the error token
- Now check if the lenght of the lexme is greater than ERR_LEN
- If yes , Store the first 20 characters into the attribute including
- the appended 3 dots (. , . , .) and the '\0' at the end of the string
- Otherwise store the entire lexeme into the err_lex
and set the '\0' at the end.
-Return the token
*******************************************************************************************************************************/
Token aa_func11(char lexeme[]) {

	/*Varables used are declared.*/
	Token t;
	unsigned int i = 0;

	/*set the code to the error token*/
	t.code = ERR_T;

	/*Incrementing line counter if '\n' found in the lexeme*/
	for (i = 0; i < strlen(lexeme); i++) {
		if (lexeme[i] == '\n')
			line++;
	}
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
	}
	else
	{
		/* Otherwise store the error into err_lex*/
		for (i = 0; i < strlen(lexeme); i++)
		{
			t.attribute.err_lex[i] = lexeme[i];
		}
		t.attribute.err_lex[i] = '\0';/*Adds the line terminator*/
	}
	return t;
}

/********************************************************************************************************************************
Purpose				:The purpose of this function is tocheck if the provided parameter is a keyword or not
Author				:Nisarg Patel / Divy Shah
History/Versions		:2018/10/07
Called Function			:strcmp() - To compare the parameter to the keywords
Parameters			:char * kw_lexeme : The characters that are to be checked
Return Value			:i - The location of the keyword , if found , otherwise -1
Algorithm			: - we iterate through the loop of the given keywords
						- We compare the paramater to the keyword table to see it its the same
						- we return the comparision keyword position.
						- Otherwise we return -1
*******************************************************************************************************************************/
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
