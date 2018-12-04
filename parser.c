#include "parser.h"

void parser(void) {
	lookahead = malar_next_token();
	program();
	match(SEOF_T, NO_ATTR);
	gen_incode("PLATY: Source file parsed");
}

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
void syn_eh(int sync_token_code) {
	/*First calling syn_printe() and incrementing the counter*/
	syn_printe();
	synerrno++;
	/*Implementing panic mode recovery*/
	/*Looping through until required token is matched*/

	//while (sync_token_code != lookahead.code) {
	//	/*When end of file is reached*/
	//	if (lookahead.code == SEOF_T) {
	//		/*Exiting with error code*/
	//		exit(synerrno);
	//	}
	//	/*Getting next token*/
	//	lookahead = malar_next_token();
	//}
	///*When end of file is reached*/
	//if (lookahead.code == SEOF_T) {
	//	return;
	//}
	///*Token matched moving on to next token*/
	//lookahead = malar_next_token();

	do {
		/*Getting next token*/
		lookahead = malar_next_token();

		/*When end of file is reached*/
		if (lookahead.code == SEOF_T) {
			/*Exiting with error code*/
			exit(synerrno);
			return;
		}
		/*Token matched moving on to next token and return*/
		if (lookahead.code == sync_token_code) {
			lookahead = malar_next_token();
			return;
		}
	} while (sync_token_code != lookahead.code);
}

/*Error printing function*/
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

void gen_incode(char *string) {
	/*Printing the passed string*/
	printf("%s\n", string);
	/*Can be used to print either PLATY : <something> parsed*/
	/*Later can be used to perform bonus 1*/
}




