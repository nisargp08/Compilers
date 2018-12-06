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
#define NO_ATTR		-1						/*For NULL attribute*/
#define ELSE		0						/*Assigned value for keyword ELSE*/ 
#define FALSE		1						/*Assigned value for keyword FALSE*/
#define IF			2						/*Assigned value for keyword IF*/ 
#define PLATYPUS	3						/*Assigned value for keyword PLATYPUS*/
#define READ		4						/*Assigned value for keyword READ*/
#define REPEAT		5						/*Assigned value for keyword REPEAT*/
#define THEN		6						/*Assigned value for keyword THEN*/ 
#define TRUE		7						/*Assigned value for keyword TRUE*/ 
#define WHILE		8						/*Assigned value for keyword WHILE*/
#define WRITE		9						/*Assigned value for keyword WRITE*/

										/*Function Prototypes*/
void parser(void);
void match(int pr_token_code, int pr_token_attribute);
void syn_eh(int sync_token_code);
void syn_printe();
void gen_incode(char* string);

/*Grammer Functions*/
void program(void);
void opt_statements(void);
void statements(void);
void statement(void);
void statement_dash(void);
void assignment_statement(void);
void assignment_expression(void);
void arithmetic_expression(void);
void unary_arithmetic_expression(void);
void primary_arithmetic_expression(void);
void additive_arithmetic_expression(void);
void additive_arithmetic_expression_dash(void);
void multiplicative_arithmetic_expression(void);
void multiplicative_arithmetic_expression_dash(void);
void string_expression(void);
void string_expression_dash(void);
void primary_string_expression(void);
void selection_statement(void);
void conditional_expression(void);
void logical_or_expression(void);
void logical_and_expression(void);
void logical_or_expression_dash(void);
void logical_and_expression_dash(void);
void relational_expression(void);
void primary_a_relational_expression(void);
void primary_a_relational_expression_dash(void);
void primary_s_relational_expression(void);
void primary_s_relational_expression_dash(void);
void iteration_statement(void);
void input_statement(void);
void variable_list(void);
void variable_identifier(void);
void variable_list_dash(void);
void output_statement(void);
void output_statement_dash(void);
