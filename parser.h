#include "buffer.h"
#include "token.h"
#include "table.h"
#include "scanner.c"

/*Static Global variables*/
static Token lookahead;
static int synerrno;

/*Variable Declaration*/
#define NO_ATTR -1


/*Function Prototypes*/
void parser(void);
void match(int pr_token_code, int pr_token_attribute);
void syn_eh(int sync_token_code);
void syn_printe();
void gen_incode(char* string);
