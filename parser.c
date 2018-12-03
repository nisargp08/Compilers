#include "parser.h"

void parser(void) {
	lookahead = malar_next_token();
	program();
	match(SEOF_T, NO_ATTR);
	gen_incode("PLATY: Source file parsed");
}

void match(int pr_token_code, int pr_token_attribute) {

}
