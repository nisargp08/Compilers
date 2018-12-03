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
		sys_printe();
		/*Advancing to next input token*/
		lookahead = malar_next_token();
		/*Incrementing error counter*/
		synerrno++;
		return;
	}
} /*Match ends*/

/*Error handling function*/
void syn_eh(int sync_token_code){
	/*First calling syn_printe() and incrementing the counter*/
	syn_printe();
	synerrno++;
	/*Implementing panic mode recovery*/
	/*Looping through until required token is matched*/
	while (sync_token_code != lookahead.code) {
		/*When end of file is reached*/
		if (lookahead.code == SEOF_T) {
			/*Exiting with error code*/
			exit(synerrno);
		}
		/*Getting next token*/
		lookahead = malar_next_token();
	}
	/*When end of file is reached*/
	if (lookahead.code == SEOF_T) {
		return;
	}
	/*Token matched moving on to next token*/
	lookahead = malar_next_token();
}
