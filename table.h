/********************************************************************************************************************************

Filename				:	table.h
Compiler				:	MS Visual Studio 2015
Author / Student name			:	Nisarg Patel,040859993
									Divy Shah, 040859087
Course					:	CST 8152 - Compilers
Lab section				:	13 , 14
Assignment				:	2
Date					:	2018/10/8
Professor				:	Sv. Ranev
Purpose					:	This header file is reposnible for implementing the Transition table

*******************************************************************************************************************************/


#ifndef  TABLE_H_
#define  TABLE_H_ 

#ifndef BUFFER_H_
#include "buffer.h"
#endif

#ifndef NULL
#include <_null.h> /* NULL pointer constant is defined there */
#endif

/*   Source end-of-file (SEOF) sentinel symbol
*    '\0' or one of 255,0xFF,EOF
*/
#define SEOF 255

/*  Special case tokens processed separately one by one
*  in the token-driven part of the scanner
*  '=' , ' ' , '(' , ')' , '{' , '}' , == , <> , '>' , '<' , ';',
*  white space
*  !!comment , ',' , ';' , '-' , '+' , '*' , '/', # ,
*  .AND., .OR. , SEOF, 'illegal symbol',
*/

#define ES  11				/* Error state  with no retract */
#define ER  12				/* Error state  with retract */
#define IS -1				/* Inavalid state */
#define BACKSLASHZERO '\0'	/*Constant for Backslash zero*/
#define ZERO 0				/* Used to return non-character 0 value in char_class function*/
#define CHARZERO '0'		/* Used to return 0th column from st_table*/
#define ONE 1				/* Used to return 1st column from st_table*/
#define TWO 2				/* Used to return 2nd column from st_table*/
#define THREE 3				/* Used to return 3rd column from st_table*/
#define FOUR 4				/* Used to return 4th column from st_table*/
#define FIVE 5				/* Used to return 5th column from st_table*/
#define SIX 6				/* Used to return 6th column from st_table*/
#define SEVEN 7				/* Used to return 7th column from st_table*/
/* State transition table definition */

/* Table column is assigned with value 8 as we have six input symbols  including " and SEOF */
#define TABLE_COLUMNS 8

/*transition table - type of states defined in separate table */
int  st_table[][TABLE_COLUMNS] = {
	/* State 0 */	{ 1 , 6 , 4 , ES , ES , ES , 9 , ES},
	/* State 1 */	{ 1 , 1 , 1 , 2 , 3 , 2 , 2 , 2 },
	/* State 2 */	{ IS , IS , IS , IS , IS , IS , IS , IS},
	/* State 3 */	{ IS , IS , IS , IS , IS , IS , IS , IS},
	/* State 4 */	{ ES , 4 , 4 , 7 , 5 , 5 , ER , ER},
	/* State 5 */	{ IS , IS , IS , IS , IS , IS , IS , IS},
	/* State 6 */	{ ES , 6 , ES , 7 , ES , 5 , ES , 5},
	/* State 7 */	{ 8 , 7 , 7 , 8 , 8 , 8 , ES , 8},
	/* State 8 */	{ IS , IS , IS , IS , IS , IS , IS , IS},
	/* State 9 */	{ 9 , 9 , 9 , 9 , 9 , 9 , 10 , ER }, 
	/* State 10 */	{ IS , IS , IS , IS , IS , IS , IS , IS},
	/* State 11 */	{ IS , IS , IS , IS , IS , IS , IS , IS},
	/* State 12 */	{ IS , IS , IS , IS , IS , IS , IS , IS},
};

/* Accepting state table definition */
#define ASWR     2  /* accepting state with retract */
#define ASNR     1  /* accepting state with no retract */
#define NOAS     0  /* not accepting state */

int as_table[] = {
	/* State 0 */	NOAS,
	/* State 1 */	NOAS,
	/* State 2 */	ASWR,
	/* State 3 */	ASNR,
	/* State 4 */	NOAS,
	/* State 5 */	ASWR,
	/* State 6 */	NOAS,
	/* State 7 */	NOAS,
	/* State 8 */	ASWR,
	/* State 9 */	NOAS,
	/* State 10 */	ASNR,
	/* State 11 */	ASNR,
	/* State 12 */	ASWR
};

/* Accepting action function declarations */

/* Each accepting states
	Argument	:	Takes a string representing a token lexeme
	Return type :	Token
*/
Token aa_func02(char *lexeme);	/* VID AVID/KW */
Token aa_func03(char *lexeme);	/* VID SVID */
Token aa_func05(char *lexeme);	/* DIL */
Token aa_func08(char *lexeme);	/* FPL */
Token aa_func10(char *lexeme);	/* SL */
Token aa_func11(char *lexeme);	/* ES */

/* defining a new type: pointer to function (of one char * argument)
returning Token
*/

typedef Token(*PTR_AAF)(char *lexeme);


/* Accepting function (action) callback table (array) definition */
/* If you do not want to use the typedef, the equvalent declaration is:
* Token (*aa_table[])(char lexeme[]) = {
*/

/* Array of pointers is initialized here */
PTR_AAF aa_table[] = {
	/* State 0 */	NULL,
	/* State 1 */	NULL,
	/* State 2 */	aa_func02,
	/* State 3 */	aa_func03,
	/* State 4 */	NULL,
	/* State 5 */	aa_func05,
	/* State 6 */	NULL,
	/* State 7 */	NULL,
	/* State 8 */	aa_func08,
	/* State 9 */	NULL,
	/* State 10 */	aa_func10,
	/* State 11 */	aa_func11,
	/* State 12 */	aa_func11,
};

/* Keyword lookup table (.AND. and .OR. are not keywords) */

#define KWT_SIZE  10

char * kw_table[] =
{
	"ELSE",
	"FALSE",
	"IF",
	"PLATYPUS",
	"READ",
	"REPEAT",
	"THEN",
	"TRUE",
	"WHILE",
	"WRITE"
};

#endif
