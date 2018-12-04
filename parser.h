#ifndef PARSER_H_
#define PARSER_H_

#ifndef BUFFER_H_
#include "buffer.h"
#endif

#ifndef TOKEN_H_
#include "token.h"
#endif

/*Static Global variables*/
static Token lookahead;					/*Used to store fetched token from malar function*/
int synerrno;							/*Total error counter*/

										/*External variables and functions*/
extern Token malar_next_token(void);	/*Getting a token function*/
extern int line;						/* source code line number - defined in scanner.c */
extern char *kw_table[];				/*Keyword table array*/
extern Buffer * str_LTBL;				/*String literal table */

										/*Variable Declaration*/
#define NO_ATTR -1		/*For NULL attribute*/
#define PLATYPUS 3	/*Assigned value for keyword platypus*/ 

										/*Function Prototypes*/
void parser(void);
void match(int pr_token_code, int pr_token_attribute);
void syn_eh(int sync_token_code);
void syn_printe();
void gen_incode(char* string);

/*Grammer Functions*/
void program(void);
void opt_statements(void);
void statement(void);
void statement_dash(void);
void assignment_statement(void);
void assignment_expression(void);
void arithmetic_expression(void);
void unary_arithmetic_expression(void);
void primary_arithmetic_expression(void);
void additive_arithmetic_expression(void);
#endif
