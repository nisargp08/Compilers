/********************************************************************************************************************************

Filename				:	parser.c
Compiler				:	MS Visual Studio 2015
Author / Student name	:	Nisarg Patel,040859993
Divy Shah, 040859087
Course					:	CST 8152 - Compilers
Lab section				:	13(Nisarg) , 14(Divy)
Assignment				:	3
Date					:	2018/12/06
Professor				:	Sv. Ranev
Purpose					:	Implementing a Parser and more…
Functions list			:	parser(),malar_next_token(),program(),match(),gen_incode(),syn_eh(),syn_printe(),exit(),printf(),opt_statements();
statements(), statement(),statement_dash(),assignment_statement(),assignment_expression(),arithmetic_expression(),
unary_arithmetic_expression(),primary_arithmetic_expression(),additive_arithmetic_expression(),additive_arithmetic_expression_dash(),
multiplicative_arithmetic_expression(),multiplicative_arithmetic_expression_dash(),string_expression(),string_expression_dash(),
primary_string_expression(),selection_statement(),conditional_expression(),logical_or_expression(),logical_and_expression(),
logical_or_expression_dash(),logical_and_expression_dash(),relational_expression(),primary_a_relational_expression(),
primary_a_relational_expression_dash(),primary_s_relational_expression(),primary_s_relational_expression_dash(),
void iteration_statement(),input_statement(),variable_list(),variable_identifier(),variable_list_dash(),
output_statement(),output_statement_dash()

*******************************************************************************************************************************/
#include <stdlib.h>
#include "parser.h"
/********************************************************************************************************************************
Purpose					:The purpose of this function is to parse the entire written code
Author					: Sv. Ranev
History / Versions		: 2018/12/06
Called Function			: malar_next_token(),program(),match(),gen_incode();
Parameters				: None
Return Value			: None
Algorithm				: Calls program() to parse the entire code untill SEOF is reached
* ******************************************************************************************************************************/
void parser(void) {
	lookahead = malar_next_token();
	program();
	match(SEOF_T, NO_ATTR);
	gen_incode("PLATY: Source file parsed");
}
/********************************************************************************************************************************
Purpose					: The purpose of this function is to match the passed token via arguments
Author					: Nisarg Patel
History / Versions		: 2018/12/06
Called Function			: syn_eh(),malar_next_token(),syn_printe();
Parameters				: int pr_token_code - Holds token code for the passed token
int pr_token_attribute - Holds token attribute for the passed token
Return Value			: None
Algorithm				: Compares passed token code and attribute with the lookahead if they match then token is increemnted else
calls error handler or error function according to the condition
* ******************************************************************************************************************************/
void match(int pr_token_code, int pr_token_attribute) {
	/*When token code not equal to lookahead meaning match failed*/
	if (lookahead.code != pr_token_code) {
		/*Error occured calling error handler*/
		syn_eh(pr_token_code);
		return;
	}
	/*When lookahead is equal to end of file*/
	if (lookahead.code == SEOF_T) {
		/*Exit the match when end of file is matched*/
		return;
	}
	if (pr_token_code == KW_T || pr_token_code == LOG_OP_T || pr_token_code == ART_OP_T ||
		pr_token_code == REL_OP_T) {
		/*When token code is KW_T whichc is keyword*/
		if (pr_token_code == KW_T) {
			if (pr_token_attribute != lookahead.attribute.kwt_idx) {
				/*Error occured calling error handler*/
				syn_eh(pr_token_code);
				return;
			}
		}
		/*When token code is LOG | ART | REL */
		else {
			if (pr_token_attribute != lookahead.attribute.get_int) {
				/*Error occured calling error handler*/
				syn_eh(pr_token_code);
				return;
			}
		}
	}
	/*Here match is successful and token is not SEOF_T so moving on to next token*/
	lookahead = malar_next_token();
	/*When new lookahead token is ERR_T*/
	if (lookahead.code == ERR_T) {
		/*Calling error printing function*/
		syn_printe();
		/*Advancing to next input token*/
		lookahead = malar_next_token();
		/*Incrementing error counter*/
		synerrno++;
		return;
	}
} /*Match ends*/

  /*Error handling function*/
  /********************************************************************************************************************************
  Purpose					: The purpose of this function to print the errors in the program,increment error counter and increment
  the lookahead token variable untill passed token code is not matched
  Author					: Nisarg Patel
  History / Versions		: 2018/12/06
  Called Function			: exit(),malar_next_token(),syn_printe();
  Parameters				: int sync_token_code - Holds token code for the passed token
  Return Value				: None
  Algorithm					: None
  * ******************************************************************************************************************************/
void syn_eh(int sync_token_code) {
	/*First calling syn_printe() and incrementing the counter*/
	syn_printe();
	synerrno++;
	/*Implementing panic mode recovery*/
	/*Looping through until required token is matched*/
	do {
		/*When end of file is reached*/
		if (lookahead.code == SEOF_T) {
			/*Exiting with error code*/
			exit(synerrno);
			return;
		}
		/*Getting next token*/
		lookahead = malar_next_token();

		/*Token matched moving on to next token and return*/
		if (lookahead.code == sync_token_code) {
			lookahead = malar_next_token();
			return;
		}
	} while (sync_token_code != lookahead.code);
}

/*Error printing function*/
/********************************************************************************************************************************
Purpose					: The purpose of this function is to print encountered errors in the program
Author					: Sv.Ranev
History / Versions		: 2018/12/06
Called Function			: printf();
Parameters				: None
Return Value			: None
Algorithm				: None
* ******************************************************************************************************************************/
/* error printing function for Assignment 3 (Parser), F18 */
void syn_printe() {
	Token t = lookahead;

	printf("PLATY: Syntax error:  Line:%3d\n", line);
	printf("*****  Token code:%3d Attribute: ", t.code);
	switch (t.code) {
	case  ERR_T: /* ERR_T     0   Error token */
		printf("%s\n", t.attribute.err_lex);
		break;
	case  SEOF_T: /*SEOF_T    1   Source end-of-file token */
		printf("SEOF_T\t\t%d\t\n", t.attribute.seof);
		break;
	case  AVID_T: /* AVID_T    2   Arithmetic Variable identifier token */
	case  SVID_T:/* SVID_T    3  String Variable identifier token */
		printf("%s\n", t.attribute.vid_lex);
		break;
	case  FPL_T: /* FPL_T     4  Floating point literal token */
		printf("%5.1f\n", t.attribute.flt_value);
		break;
	case INL_T: /* INL_T      5   Integer literal token */
		printf("%d\n", t.attribute.get_int);
		break;
	case STR_T:/* STR_T     6   String literal token */
		printf("%s\n", b_location(str_LTBL, t.attribute.str_offset));
		break;

	case SCC_OP_T: /* 7   String concatenation operator token */
		printf("NA\n");
		break;

	case  ASS_OP_T:/* ASS_OP_T  8   Assignment operator token */
		printf("NA\n");
		break;
	case  ART_OP_T:/* ART_OP_T  9   Arithmetic operator token */
		printf("%d\n", t.attribute.get_int);
		break;
	case  REL_OP_T: /*REL_OP_T  10   Relational operator token */
		printf("%d\n", t.attribute.get_int);
		break;
	case  LOG_OP_T:/*LOG_OP_T 11  Logical operator token */
		printf("%d\n", t.attribute.get_int);
		break;

	case  LPR_T: /*LPR_T    12  Left parenthesis token */
		printf("NA\n");
		break;
	case  RPR_T: /*RPR_T    13  Right parenthesis token */
		printf("NA\n");
		break;
	case LBR_T: /*    14   Left brace token */
		printf("NA\n");
		break;
	case RBR_T: /*    15  Right brace token */
		printf("NA\n");
		break;

	case KW_T: /*     16   Keyword token */
		printf("%s\n", kw_table[t.attribute.get_int]);
		break;

	case COM_T: /* 17   Comma token */
		printf("NA\n");
		break;
	case EOS_T: /*    18  End of statement *(semi - colon) */
		printf("NA\n");
		break;
	default:
		printf("PLATY: Scanner error: invalid token code: %d\n", t.code);
	}/*end switch*/
}/* end syn_printe()*/

 /********************************************************************************************************************************
 Purpose					: The purpose of this function is to print the passed char*
 Author					: Nisarg Patel
 History / Versions		: 2018/12/06
 Called Function			: printf();
 Parameters				: char *string - Holds passed string
 Return Value			: None
 Algorithm				: None
 * ******************************************************************************************************************************/
void gen_incode(char *string) {
	/*Printing the passed string*/
	printf("%s\n", string);
	/*Later can be used to perform bonus 1*/
}
/*
Author						:	Sv.Ranev
Grammar Production			:	<program> -> PLATYPUS{<opt_statement>}SEOF
FIRST set of the Production :	FIRST(<program>) = {KW_T(PLATYPUS)}

*/
void program(void) {
	match(KW_T, PLATYPUS);
	match(LBR_T, NO_ATTR);
	opt_statements();
	match(RBR_T, NO_ATTR);
	gen_incode("PLATY: Program parsed");
}
/*
Author						:	Sv.Ranev
Grammar Production			:	<opt_statements> ->  <statements> | e
FIRST set of the Production :	FIRST(<opt_statements>) = { AVID_T , SVID_T , KW{IF} , KW{WHILE} , KW{READ} , KW{WRITE} , e }

*/
void opt_statements(void) {
	/* FIRST set: {AVID_T,SVID_T,KW_T(but not � see above),e} */
	switch (lookahead.code) {
	case AVID_T:
	case SVID_T: statements(); break;
	case KW_T:
		/* check for PLATYPUS, ELSE, THEN, REPEAT, TRUE, FALSE here
		and in statements_p()*/
		if (lookahead.attribute.get_int != PLATYPUS
			&& lookahead.attribute.get_int != ELSE
			&& lookahead.attribute.get_int != THEN
			&& lookahead.attribute.get_int != REPEAT
			&& lookahead.attribute.get_int != TRUE
			&& lookahead.attribute.get_int != FALSE) {
			statements();
			break;
		}
	default: /*empty string � optional statements*/;
		gen_incode("PLATY: Opt_statements parsed");
	}
}
/*
Author						:	Nisarg Patel
Grammar Production			:	<statements> ->  <statement><statement'>
FIRST set of the Production :	FIRST(<statements>) = { AVID_T , SVID_T , KW{IF} , KW{WHILE} , KW{READ} , KW{WRITE} }

*/
void statements(void) {
	statement();
	statement_dash();
}
/*
Author						:	Divy Shah
Grammar Production			:	<statement> -> <assignment statement> | < selection statement> | <iteration statement> | <input statement> | <output statement>
FIRST set of the Production :	FIRST(<statement>) = {AVID_T , SVID_T , KW{IF} , KW{WHILE} , KW{READ} , KW{WRITE} }

*/
void statement() {
	switch (lookahead.code) {
	case AVID_T:
	case SVID_T:
		assignment_statement();
		break;
	case KW_T:
		switch (lookahead.attribute.kwt_idx) {
		case IF:
			selection_statement();
			break;
		case WHILE:
			iteration_statement();
			break;
		case READ:
			input_statement();
			break;
		case WRITE:
			output_statement();
			break;
		}
		break;
	default:
		syn_printe();
		break;
	}
}
/*
Author						:	Nisarg Patel
Grammar Production			:	<statement'> -> <statement><statement'> | e
FIRST set of the Production :	FIRST(<statement'>) = { AVID_T , SVID_T , KW{IF} , KW{WHILE} , KW{READ} , KW{WRITE} , e }

*/
void statement_dash(void) {
	switch (lookahead.code) {
	case AVID_T:
	case SVID_T:
		statement();
		statement_dash();
		break;
	case KW_T:
		/* check for PLATYPUS, ELSE, THEN, REPEAT, TRUE, FALSE here*/
		if (lookahead.attribute.get_int != PLATYPUS
			&& lookahead.attribute.get_int != ELSE
			&& lookahead.attribute.get_int != THEN
			&& lookahead.attribute.get_int != REPEAT
			&& lookahead.attribute.get_int != TRUE
			&& lookahead.attribute.get_int != FALSE) {
			statements();
			break;
		}
	default:
		break;
	}
}
/*
Author						:	Divy Shah
Grammar Production			:	<assignment statement> -> <assignment expression>;
FIRST set of the Production :	FIRST(<assignment statement>) = { AVID_T , SVID_T }

*/
void assignment_statement(void) {
	assignment_expression();
	match(EOS_T, NO_ATTR);
	gen_incode("PLATY: Assignment statement parsed");
}
/*
Author						:	Nisarg Patel
Grammar Production			:	<assignment expression> -> AVID = <arithmetic expression> | SVID = <string expression>
FIRST set of the Production :	FIRST( <assignment expression> ) = { AVID_T , SVID_T }

*/
void assignment_expression(void) {
	switch (lookahead.code) {
	case AVID_T:
		match(AVID_T, NO_ATTR);
		match(ASS_OP_T, EQ);
		arithmetic_expression();
		gen_incode("PLATY: Assignment expression (arithmetic) parsed");
		break;
	case SVID_T:
		match(SVID_T, NO_ATTR);
		match(ASS_OP_T, EQ);
		string_expression();
		gen_incode("PLATY: Assignment expression (string) parsed");
		break;
	default:
		syn_printe();
		break;
	}
}
/*
Author						:	Divy Shah
Grammar Production			:	<arithmetic_expression> -> <unary arithmetic expression> | <addititve arithmetic expression>
FIRST set of the Production :	FIRST(<arithmetic_expression>) = { ART_OP_T(MINUS) , ART_OP_T(PLUS) , AVID_T , FPL_T , INL_T ,  ( }

*/
void arithmetic_expression(void) {
	switch (lookahead.code) {
		/*First Checking if operator is unary */
	case ART_OP_T:
		/*If its one of +,- then calling unary_arithmetic_expression*/
		if (lookahead.attribute.arr_op == PLUS || lookahead.attribute.arr_op == MINUS)
			unary_arithmetic_expression();
		else
			syn_printe();
		break;
		/*Checking for AVID,FPL,INL,LPR*/
	case AVID_T:
	case FPL_T:
	case INL_T:
	case LPR_T:
		/*Following code will be executed for the above mentioned four case(upto AVID_T)*/
		additive_arithmetic_expression();
		break;
	default:
		syn_printe();
		break;
	}
	gen_incode("PLATY: Arithmetic expression parsed");
}
/*
Author						:	Nisarg Patel
Grammar Production			:	<unary_arithmetic_expression> -> -<primary arithmetic expression> | +<primary arithmetic expression>
FIRST set of the Production :	FIRST(<unary_arithmetic_expression>) = { ART_OP_T(MINUS) , ART_OP_T(PLUS) }

*/
void unary_arithmetic_expression(void) {
	switch (lookahead.code) {
	case ART_OP_T:
		switch (lookahead.attribute.arr_op) {
		case PLUS:
			match(ART_OP_T, PLUS);
			primary_arithmetic_expression();
			break;
		case MINUS:
			match(ART_OP_T, MINUS);
			primary_arithmetic_expression();
			break;
		default:
			syn_printe();
			break;
		}
	}
	gen_incode("PLATY: Unary arithmetic expression parsed");
}
/*
Author						:	Divy Shah
Grammar Production			:	<primary_arithmetic_expression> -> AVID_T | FPL_T | INL_T | (<arithmetic expression)
FIRST set of the Production :	FIRST(<primary_arithmetic_expression>) = { AVID_T , FPL_T , INL_T , ( }

*/
void primary_arithmetic_expression(void) {
	switch (lookahead.code) {
	case AVID_T:
		match(AVID_T, NO_ATTR);
		break;
	case FPL_T:
		match(FPL_T, NO_ATTR);
		break;
	case INL_T:
		match(INL_T, NO_ATTR);
		break;
	case LPR_T:
		match(LPR_T, NO_ATTR);
		arithmetic_expression();
		match(RPR_T, NO_ATTR);
		break;
	default:
		syn_printe();
		break;
	}
	gen_incode("PLATY: Primary arithmetic expression parsed");
}
/*
Author						:	Nisarg Patel
Grammar Production			:	<additive arithmetic expression> -> <multiplicative arithmetic expression><addititive arithmetic expression'>
FIRST set of the Production :	FIRST(<additive arithmetic expression>) = { AVID_T , FPL_T , INL_T , ( }

*/
void additive_arithmetic_expression(void) {
	multiplicative_arithmetic_expression();
	additive_arithmetic_expression_dash();
}
/*
Author						:	Divy Shah
Grammar Production			:	<additive arithmetic expression'> -> +<multiplicative arithmetic expression><additive arithmetic expression'>
																									|
																	 -<multiplicative arithmetic expression><additive arithmetic expression'> | e
FIRST set of the Production :	FIRST(<additive arithmetic expression'>) = { ART_OP_T(PLUS) , ART_OP_T(MINUS) , e }

*/
void additive_arithmetic_expression_dash(void) {
	switch (lookahead.code) {
	case ART_OP_T:
		switch (lookahead.attribute.arr_op) {
		case PLUS:
			match(ART_OP_T, PLUS);
			multiplicative_arithmetic_expression();
			additive_arithmetic_expression_dash();
			gen_incode("PLATY: Additive arithmetic expression parsed");
			break;
		case MINUS:
			match(ART_OP_T, MINUS);
			multiplicative_arithmetic_expression();
			additive_arithmetic_expression_dash();
			gen_incode("PLATY: Additive arithmetic expression parsed");
			break;
		default:
			syn_printe();
			break;
		}
		break;
	}
}
/*
Author						:	Nisarg Patel
Grammar Production			:	<multiplicative_arithmetic_expression> -> <primary_arithmetic_expression><multiplicative arithmetic expression'>
FIRST set of the Production :	FIRST(<multiplicative_arithmetic_expression>) = { AVID_T , FPL_T , INL_T , ( }

*/
void multiplicative_arithmetic_expression(void) {
	primary_arithmetic_expression();
	multiplicative_arithmetic_expression_dash();
}
/*
Author						:	Divy Shah
Grammar Production			:	<multiplicative arithmetic expression dash> -> *<primary_arithmetic_expression><multiplicative arithmetic expression'>
																												|	
																			   /<primary_arithmetic_expression><multiplicative arithmetic expression'> | e
FIRST set of the Production :	FIRST(<multiplicative arithmetic expression dash>) = { ART_OP(MULTI),ART_OP(DIV),e }

*/
void multiplicative_arithmetic_expression_dash(void) {
	switch (lookahead.code) {
	case ART_OP_T:
		switch (lookahead.attribute.arr_op) {
		case MULT:
			match(ART_OP_T, MULT);
			primary_arithmetic_expression();
			multiplicative_arithmetic_expression_dash();
			gen_incode("PLATY: Multiplicative arithmetic expression parsed");
			break;
		case DIV:
			match(ART_OP_T, DIV);
			primary_arithmetic_expression();
			multiplicative_arithmetic_expression_dash();
			gen_incode("PLATY: Multiplicative arithmetic expression parsed");
			break;
		}
		break;
	}
}
/*
Author						:	Nisarg Patel
Grammar Production			:	<string expression> -> <primary string expression><string expression'>
FIRST set of the Production :	FIRST(<string expression>) = { SVID_T , STR_T }

*/
void string_expression(void) {
	primary_string_expression();
	string_expression_dash();
	gen_incode("PLATY: String expression parsed");
}
/*
Author						:	Divy Shah
Grammar Production			:	<string_expression'> -> #<primary string expression><string expression'> | e
FIRST set of the Production :	FIRST(<string_expression'>) = { SCC_OP_T , e }

*/
void string_expression_dash(void) {
	switch (lookahead.code) {
	case SCC_OP_T:
		match(SCC_OP_T, NO_ATTR);
		primary_string_expression();
		string_expression_dash();
		break;
	}
}
/*
Author						:	Nisarg Patel
Grammar Production			:	<primary string expression> -> SVID_T | STR_T
FIRST set of the Production :	FIRST(<primary string expression>) = { SVID_T , STR_T }

*/
void primary_string_expression(void) {
	switch (lookahead.code) {
	case SVID_T:
	case STR_T:
		match(lookahead.code, NO_ATTR);
		break;
	}
	gen_incode("PLATY: Primary string expression parsed");
}
/*
Author						:	Divy Shah
Grammar Production			:	<selection statement> -> IF<pre-condition>(<condition expression>) THEN {<opt_statements>}ELSE{<opt_statements>};
								- Same as Original production
FIRST set of the Production :	FIRST(<selection statement>) = {KW_T(IF);}

*/
void selection_statement(void) {
	match(KW_T, IF);
	if (lookahead.code == KW_T) {
		if (lookahead.attribute.kwt_idx == TRUE || lookahead.attribute.kwt_idx == FALSE) {
			match(KW_T, lookahead.attribute.kwt_idx);
		}
	}
	match(LPR_T, NO_ATTR);
	conditional_expression();
	match(RPR_T, NO_ATTR);
	match(KW_T, THEN);
	match(LBR_T, NO_ATTR);
	opt_statements();
	match(RBR_T, NO_ATTR);
	match(KW_T, ELSE);
	match(LBR_T, NO_ATTR);
	opt_statements();
	match(RBR_T, NO_ATTR);
	match(EOS_T, NO_ATTR);
	gen_incode("PLATY: Selection statement parsed");
}
/*
Author						:	Nisarg Patel
Grammar Production			:	<conditional expression> -> <logical or expression>
FIRST set of the Production :	FIRST(<conditional expression> = { AVID_T, SVID_T , STR_T , FPL_T , INL_T }

*/
void conditional_expression(void) {
	logical_or_expression();
	gen_incode("PLATY: Conditional expression parsed");
}
/*
Author						:	Divy Shah
Grammar Production			:	<logical or expression> -> <logical and expression><logical or expression'>
FIRST set of the Production :	FIRST(<logical or expression>) = {  AVID_T, SVID_T , STR_T , FPL_T , INL_T }

*/
void logical_or_expression(void) {
	logical_and_expression();
	logical_or_expression_dash();
}
/*
Author						:	Nisarg Patel
Grammar Production			:	<logical or expression'> -> .OR.<logical AND expression><logical OR expression'> | e
FIRST set of the Production :	FIRST<logical or expression'> = { LOG_OP_T(OR) , e }

*/
void logical_or_expression_dash(void) {
	switch (lookahead.code)
	{
	case LOG_OP_T:
		switch (lookahead.attribute.log_op)
		{
		case OR:
			match(LOG_OP_T, OR);
			logical_and_expression();
			logical_or_expression_dash();
			gen_incode("PLATY: Logical OR expression parsed");
			break;
		}
		break;
	default:
		break;
	}
}
/*
Author						:	Divy Shah
Grammar Production			:	<logical and expression> = <relational expression><logical and expression'>
FIRST set of the Production :	FIRST(<logical and expression>) = { AVID_T , SVID_T , STR_T , FPL_T , INL_T }

*/
void logical_and_expression(void) {
	relational_expression();
	logical_and_expression_dash();
}
/*
Author						:	Nisarg Patel
Grammar Production			:	<logical and expression> = .AND.<relational expression><logical and expression'> | e
FIRST set of the Production :	FIRST(<logical and expression>) = { LOG_OP_T(AND) , e }

*/
void logical_and_expression_dash(void) {
	switch (lookahead.code)
	{
	case LOG_OP_T:
		switch (lookahead.attribute.log_op)
		{
		case AND:
			match(LOG_OP_T, AND);
			relational_expression();
			logical_and_expression_dash();
			gen_incode("PLATY: Logical AND expression parsed");
			break;
		}
		break;
	default:
		break;
	}
}
/*
Author						:	Divy Shah
Grammar Production			:	<relational expression> -> <primary a relational expression><primary a relational expression'> |
															<primary s relational expression><primary s relational expression'>
FIRST set of the Production :	FIRST(<relational expression>) = { AVID_T , SVID_T , FPL_T , INL_T , STR_T }

*/
void relational_expression(void) {
	switch (lookahead.code)
	{
	case AVID_T:
	case FPL_T:
	case INL_T:
		primary_a_relational_expression();
		primary_a_relational_expression_dash();
		break;
	case SVID_T:
	case STR_T:
		primary_s_relational_expression();
		primary_s_relational_expression_dash();
		break;
	default:
		syn_printe();
		break;
	}
	gen_incode("PLATY: Relational expression parsed");
}
/*
Author						:	Nisarg Patel
Grammar Production			:	<primary a relational expression> -> AVID_T | FPL_T | INL_T 
FIRST set of the Production :	FIRST(<primary a relational expression>) = { AVID_T , FPL_T , INL_T }

*/
void primary_a_relational_expression(void) {
	switch (lookahead.code)
	{
	case AVID_T:
	case FPL_T:
	case INL_LEN:
		match(lookahead.code, NO_ATTR);
		gen_incode("PLATY: Primary a_relational expression parsed");
		break;
	default:
		syn_printe();
		gen_incode("PLATY: Primary a_relational expression parsed");
		break;
	}
}
/*
Author						:	Divy Shah
Grammar Production			:	<primary a relational expression'> -> ==<primary a relational expression> | <><primary a relational expression>
																		| > <primary a relational expression> | < <primary a relational expression>
FIRST set of the Production :	FIRST(<primary a relational expression'>) = { == , <> , > , <  }

*/
void primary_a_relational_expression_dash(void) {
	switch (lookahead.code)
	{
	case REL_OP_T:
		switch (lookahead.attribute.rel_op)
		{
		case EQ:
		case NE:
		case GT:
		case LT:
			match(REL_OP_T, lookahead.attribute.rel_op);
			primary_a_relational_expression();
			break;
		default:
			syn_printe();
			break;
		}
		break;
	default:
		syn_printe();
		break;
	}
}
/*
Author						:	Nisarg Patel
Grammar Production			:	<primary_s_relational_expression> -> <primary string expression>
FIRST set of the Production :	FIRST(<primary_s_relational_expression>) = { SVID_T , STR_T }

*/
void primary_s_relational_expression(void) {
	switch (lookahead.code)
	{
	case SVID_T:
	case STR_T:
		primary_string_expression();
		gen_incode("PLATY: Primary s_relational expression parsed");
		break;
	default:
		syn_printe();
		break;
	}
}
/*
Author						:	Divy Shah
Grammar Production			:	<primary_s_relational_expression'> -> ==<primary_s_relational_expression> | <><primary_s_relational_expression> |
																		< <primary_s_relational_expression> | > <primary_s_relational_expression>
FIRST set of the Production :	FIRST(<primary_s_relational_expression'>) = { == , <> , < , > }

*/
void primary_s_relational_expression_dash(void) {
	switch (lookahead.code)
	{
	case REL_OP_T:
		switch (lookahead.attribute.rel_op)
		{
		case EQ:
		case NE:
		case GT:
		case LT:
			match(REL_OP_T, lookahead.attribute.rel_op);
			primary_s_relational_expression();
			break;
		default:
			syn_printe();
			break;
		}
		break;
	}
}
/*
Author						:	Nisarg Patel
Grammar Production			:	<iteration_statement> -> WHILE<pre-condition>(<conditional expression>) REPEAT { <statatements> };
								<pre-condition> -> TRUE | FALSE
FIRST set of the Production :	FIRST(<iteration_statement>) = { KW_T(WHILE); }
								FIRST(<pre-condition>) = { KW_T(TRUE) , KW_T(FALSE) }

*/
void iteration_statement(void) {
	match(KW_T, WHILE);
	if (lookahead.code == KW_T) {
		if (lookahead.attribute.kwt_idx == TRUE || lookahead.attribute.kwt_idx == FALSE) {
			match(KW_T, lookahead.attribute.kwt_idx);
		}
	}
	match(LPR_T, NO_ATTR);
	conditional_expression();
	match(RPR_T, NO_ATTR);
	match(KW_T, REPEAT);
	match(LBR_T, NO_ATTR);
	statements();
	match(RBR_T, NO_ATTR);
	match(EOS_T, NO_ATTR);
	gen_incode("PLATY: Iteration statement parsed");
}
/*
Author						:	Divy Shah
Grammar Production			:	<input_statement> -> READ(<variable list>);
FIRST set of the Production :	FIRST(<input_statement>) = { KW_T(READ) }

*/
void input_statement(void) {
	match(KW_T, READ);
	match(LPR_T, NO_ATTR);
	variable_list();
	match(RPR_T, NO_ATTR);
	match(EOS_T, NO_ATTR);
	gen_incode("PLATY: Input statement parsed");
}
/*
Author						:	Nisarg Patel
Grammar Production			:	<variable list> -> <variable identifier><variable list'>
FIRST set of the Production :	FIRST(<variable list>) = { AVID_T , SVID_T }

*/
void variable_list(void) {
	variable_identifier();
	variable_list_dash();
	gen_incode("PLATY: Variable list parsed");
}
/*
Author						:	Divy Shah
Grammar Production			:	<variable identifier> -> { AVID_T | SVID_T }
FIRST set of the Production :	FIRST(<variable identifier>) = { AVID_T , SVID_T }

*/
void variable_identifier(void) {
	switch (lookahead.code) {
	case AVID_T:
	case SVID_T:
		match(lookahead.code, NO_ATTR);
		break;
	default:
		syn_printe();
		break;
	}
}
/*
Author						:	Nisarg Patel
Grammar Production			:	<variable_list'> -> { ,<variable identifier><variable list'> | e}
FIRST set of the Production :	FIRST(<variable_list'>) = { COM_T , e }

*/
void variable_list_dash(void) {
	if (lookahead.code == COM_T) {
		match(COM_T, NO_ATTR);
		variable_identifier();
		variable_list_dash();
	}
}
/*
Author						:	Divy Shah
Grammar Production			:	<output_statement> -> WRITE(<output statement'>);
FIRST set of the Production :	FIRST(<output_statement>) = { KW_T(WRITE) }

*/
void output_statement(void) {
	match(KW_T, WRITE);
	match(LPR_T, NO_ATTR);
	output_statement_dash();
	match(RPR_T, NO_ATTR);
	match(EOS_T, NO_ATTR);
	gen_incode("PLATY: Output statement parsed");
}
/*
Author						:	Nisarg Patel
Grammar Production			:	<output statement'> -> <variable list> | STR_T | e
FIRST set of the Production :	FIRST(<output statement'>) = { AVID_T , SVID_T , STR_T , e }

*/
void output_statement_dash(void) {
	switch (lookahead.code) {
	case AVID_T:
	case SVID_T:
		variable_list();
		break;
	case STR_T:
		match(STR_T, NO_ATTR);
		gen_incode("PLATY: Output list (string literal) parsed");
		break;
	default:
		gen_incode("PLATY: Output list (empty) parsed");
		break;
	}
}
