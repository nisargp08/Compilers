#include "buffer.h"
#include "table.h"
#include "token.h"
#include "scanner.c"

/*Static Global variables*/
static Token lookahead;
static int synerrno;

/*Variable Declaration*/
#define NO_ATTR -1


/*Function Prototypes*/
void parser(void);
void match(int pr_token_code, int pr_token_attribute);
