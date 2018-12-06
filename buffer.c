/********************************************************************************************************************************

Filename				:	buffer.c
Compiler				:	MS Visual Studio 2015
Author / Student name	:	Nisarg Patel,040859993
Course					:	CST 8152 - Compilers
Lab section				:	13
Assignment				:	1
Date					:	2018/09/26
Professor				:	Sv. Ranev
Purpose					:	To create a functioning buffer and get familiar with its concept
Functions list			:	b_allocate(),b_addc(),b_clear(),b_free(),b_full(),b_limit(),b_capacity(),b_mark(),b_mode(),
b_incfactor(),b_load(),b_isempty(),b_getc(),b_eob(),b_print(),b_compact(),b_rflag(),b_retract(),
b_reset(),b_getoffset(),b_rewind(),b_location(),malloc(),calloc(),realloc().
*******************************************************************************************************************************/
#ifndef BUFFER_H_
#include "buffer.h"
#endif
/********************************************************************************************************************************
Purpose				:	Used to allocated memory dyncamically to the buffer structure and manipulate certain buffer variable
according to user's input
Author				:	Nisarg Patel
History/Versions	:	2018/09/26
Called Function		:	calloc(),malloc()
Parameters			:	short init_capapacity : Between 0 and SHRT_MAX - ONE
char inc_factor : Greater than zero
char o_mode : 'f' , 'a' , 'm'
Return Value		:	bufferPointer - pointer to buffer structure
Algorithm			:	Using parameters as value it will assign the value inc_factor and mode according to users input and will
allocate the memory to the pointer of the buffer.
*******************************************************************************************************************************/
Buffer *b_allocate(short init_capacity, char inc_factor, char o_mode) {
	Buffer *bufferPointer = NULL;
	/* init_capacity will be validated here and will return NULL if not valid */
	if (init_capacity < ZERO || init_capacity >= (SHRT_MAX)) {
		return NULL;
	}
	/* As fixed mode buffer cannot be created with init capacity of zero*/
	else if (init_capacity <= ZERO && o_mode == 'f') {
		return NULL;
	}
	/* bufferPointer will be allocated memory for one buffer structure*/
	bufferPointer = (Buffer *)calloc(1, sizeof(Buffer));
	/* Will return NULL if memory allocation fails */
	if (bufferPointer == NULL) {
		return NULL;
	}
	/* memory is dynamically allocated to cb_head based on init_capacity */
	bufferPointer->cb_head = (char *)malloc(sizeof(char) * init_capacity);
	/* Will return NULL if memory allocation fails */
	if (bufferPointer->cb_head == NULL) {
		return NULL;
	}

	/* Value of structure variable mode and inc_factor is set here based on the operational mode and value of inc_factor */
	if (o_mode == 'f' || (unsigned char)inc_factor == ZERO) {
		bufferPointer->mode = 0;
		bufferPointer->inc_factor = 0;
	}
	
	else if (o_mode == 'a' && (unsigned char)inc_factor >= ONE && (unsigned char)inc_factor <= TWO_FIFTY_FIVE) {
		bufferPointer->mode = 1;
		bufferPointer->inc_factor = inc_factor;
	}
	else if (o_mode == 'm' && inc_factor >= ONE && inc_factor <= ONE_HUNDRED) {
		bufferPointer->mode = -1;
		bufferPointer->inc_factor = inc_factor;
	}
	/* If none of the condition is satisfied then memory allocated to bufferpointer will be freed up and assinged NULL*/
	else {
		free(bufferPointer);
		bufferPointer = NULL;
		return NULL;
	}
	/*
	If values of structure variable mode and inc_factor is set then value of init_capacity will be assigned to structures capapcity &
	flag variables are set to default and pointer to the buffer is returned
	*/
	bufferPointer->capacity = init_capacity;
	bufferPointer->flags = DEFAULT_FALGS;
	return bufferPointer;
}

/********************************************************************************************************************************
Purpose				:	Will be used to add characters to the buffer
Author				:	Nisarg Patel
History/Versions	:	2018/09/26
Called Function		:	b_isfull(),realloc()
Parameters			:	const pBD - should not be NULL
char symbol
Return Value		:	pBD - pointer to the buffer
Algorithm			:	Function will keep on adding the symbol to the buffer untill it is full.Once it is full it will check
for the operational mode and check if it can increase the current capacity to the required capacity if
it can then it will increase the capacity using realloc function else returns NULL;
*******************************************************************************************************************************/

pBuffer b_addc(pBuffer const pBD, char symbol) {
	/* Variable declaration */
	short new_capacity, available_space, new_increment;
	/* If buffer needs increment and is operational then required memory will be allocated to newCapArray */
	char *newCapArray = NULL;
	if (pBD == NULL) {
		return NULL;
	}
	/* Given symbol will be added to buffer if buffer is not full*/
	if (b_isfull(pBD) == FALSE) {
		/* r_flag bit of flags variable is set to ZERO here */
		pBD->flags = pBD->flags &~(ONE << RESET_R_FLAG);
		/*Symbol will be added at the current offset position and then it is incremented */
		*(pBD->cb_head + pBD->addc_offset) = symbol;
		if (pBD->capacity > pBD->addc_offset) {
			pBD->addc_offset++;
		}
		return pBD;
	}
	/* Buffer is full so it is incremented if the mode allows*/
	else {
		/* When buffer is fixed mode buffer*/
		if (pBD->mode == FIX_BUFFER_FLAG) {
			return NULL;
		}
		/*When buffer is additive*/
		else if (pBD->mode == ADD_BUFFER_FLAG) {
			/* required capacity to increase the buffer capacity is stored in new_capacity*/
			new_capacity = (pBD->capacity) + (unsigned char)(pBD->inc_factor);
			/*When new_capacity is greated than zero*/
			if (new_capacity > ZERO) {
				if (new_capacity <= (SHRT_MAX - ONE)) {
					pBD->capacity = new_capacity;
				}
				else {
					new_capacity = (SHRT_MAX - ONE);
					pBD->capacity = new_capacity;

				}
			}
			/* When new_capacity is less than zero(i.e Negative) */
			else {
				return NULL;
			}
		}
		/* When buffer is multiplicative */
		else if (pBD->mode == MULT_BUFFER_FLAG) {
			/* When capacity is already equal to maximum allowed positive value - 1*/
			if (pBD->capacity == SHRT_MAX - ONE) {
				return NULL;
			}
			/* required capacity to increase the buffer capacity is stored in new_capacity */
			available_space = (SHRT_MAX - ONE) - pBD->capacity;
			new_increment = (available_space * (unsigned char)pBD->inc_factor) / ONE_HUNDRED;
			new_capacity = pBD->capacity + new_increment;
			/* When there is an error incrementing the capacity */
			if (new_increment == ZERO) {
				if (new_capacity < (SHRT_MAX - ONE)) {
					new_capacity = (SHRT_MAX - ONE);
					pBD->capacity = new_capacity;
				}
				else {
					return NULL;
				}
			}
			/* new_Capacity is assigned to buffer's capacity when increment is successfull without any errors */
			pBD->capacity = new_capacity;
		}
	}
	/*When increment is successfull in additive or multiplicative mode*/
	if (pBD->mode == ADD_BUFFER_FLAG || pBD->mode == MULT_BUFFER_FLAG) {
		newCapArray = realloc(pBD->cb_head, sizeof(char) * pBD->capacity);
		if (newCapArray == NULL) {
			return NULL;
		}
		if (pBD->cb_head == newCapArray)
		{/* r_flag bit of flags variable is set to ONE here */
			pBD->flags = pBD->flags | (ONE << SET_R_FLAG);
		}
		else
		{/* r_flag bit of flags variable is reset to ZERO here */
			pBD->flags = pBD->flags & ~(ONE << RESET_R_FLAG);
		}
		/*Pointing cb_head to newly created array with updated capacity*/
		pBD->cb_head = newCapArray;
		/*Symbol will be added at the current offset position*/
		//pBD->cb_head[pBD->addc_offset = symbol];
		*(pBD->cb_head + pBD->addc_offset) = symbol;
		/*add_offset will be incremented(Checking so that offset index does not go beyond bounds of capacity*/
		if (pBD->capacity > pBD->addc_offset) {
			pBD->addc_offset++;
		}
	}
	return pBD;
}

/********************************************************************************************************************************
Purpose				:	This function is used to re-initialize all the variable of the buffer to 0.This will not free the space
currently held by the buffer
Author				:	Nisarg Patel
History/Versions	:	2018/09/26
Called Function		:	-
Parameters			:	const pBD - should not be NULL
Return Value		:	TRUE(1) - int
Algorithm			:	-
*******************************************************************************************************************************/

int b_clear(Buffer * const pBD) {
	if (pBD == NULL) {
		return RT_FAIL_1;
	}
	pBD->addc_offset = 0;
	pBD->getc_offset = 0;
	pBD->markc_offset = 0;
	pBD->flags = pBD->flags & ~(ONE << RESET_R_FLAG);
	pBD->flags = pBD->flags & ~(ONE << RESET_EOB);
	return TRUE;
}
/********************************************************************************************************************************
Purpose				:	This function is used to free the memory occupied by the buffer
Author				:	Nisarg Patel
History/Versions	:	2018/09/26
Called Function		:	Free()
Parameters			:	const pBD - should not be NULL
Return Value		:	-
Algorithm			:	-
*******************************************************************************************************************************/
void b_free(Buffer * const pBD) {
	if (pBD != NULL) {
		free(pBD->cb_head);
		free(pBD);
	}
}
/********************************************************************************************************************************
Purpose				:	This function is used to check if the buffer is full or not.
Author				:	Nisarg Patel
History/Versions	:	2018/09/26
Called Function		:		-
Parameters			:	const pBD - should not be NULL
Return Value		:	TRUE(1) - if buffer is full
FALSE(0) - if buffer is not full
Algorithm			:		-
*******************************************************************************************************************************/
int b_isfull(Buffer * const pBD) {
	if (pBD == NULL) {
		return RT_FAIL_1;
	}
	else if (pBD->addc_offset == pBD->capacity) {
		return TRUE;
	}
	return FALSE;
}
/********************************************************************************************************************************
Purpose				:	This function is used to get current value of the addc_offset
Author				:	Nisarg Patel
History/Versions	:	2018/09/26
Called Function		:		-
Parameters			:	const pBD - should not be NULL
Return Value		:	pBD->add_offset - Short
Algorithm			:		-
*******************************************************************************************************************************/
short b_limit(Buffer * const pBD) {
	if (pBD == NULL) {
		return RT_FAIL_1;
	}
	return pBD->addc_offset;
}
/********************************************************************************************************************************
Purpose				:	This function is used to get the current value of the capacity
Author				:	Nisarg Patel
History/Versions	:	2018/09/26
Called Function		:		-
Parameters			:	const pBD - should not be NULL
Return Value		:	pBD->capacity - Short
Algorithm			:		-
*******************************************************************************************************************************/
short b_capacity(Buffer * const pBD) {
	if (pBD == NULL) {
		return RT_FAIL_1;
	}
	return pBD->capacity;
}
/********************************************************************************************************************************
Purpose				:	This function is used to return the markoffset of the buffer
Author				:	Nisarg Patel
History/Versions	:	2018/09/26
Called Function		:		-
Parameters			:	const pBD - should not be NULL
short mark - Between 0 and pBD->addc_offset(inclusive)
Return Value		:	Short : pBD->markc_offset if successfull else
RT_FAIL_1(-1)
Algorithm			:		-
*******************************************************************************************************************************/
short b_mark(pBuffer const pBD, short mark) {
	if (mark >= ZERO && mark <= pBD->addc_offset) {
		pBD->markc_offset = mark;
		return pBD->markc_offset;
	}
	return RT_FAIL_1;
}
/********************************************************************************************************************************
Purpose				:	This function is used to return the mode of the buffer
Author				:	Nisarg Patel
History/Versions	:	2018/09/26
Called Function		:		-
Parameters			:	const pBD - should not be NULL
Return Value		:	Int : pBD->mode
Algorithm			:		-
*******************************************************************************************************************************/
int b_mode(Buffer * const pBD) {
	if (pBD == NULL) {
		return RT_FAIL_2;
	}
	return pBD->mode;
}
/********************************************************************************************************************************
Purpose				:	This function is used to return the inc_factor of the buffer
Author				:	Nisarg Patel
History/Versions	:	2018/09/26
Called Function		:		-
Parameters			:	const pBD - should not be NULL
Return Value		:	size_t : (unsigned char)pBD->inc_factor
Algorithm			:		-
*******************************************************************************************************************************/
size_t b_incfactor(Buffer * const pBD) {
	if (pBD == NULL) {
		return HEX_ONE_HUNDRED;
	}
	return (unsigned char)pBD->inc_factor;
}
/********************************************************************************************************************************
Purpose				:	This function is used to load the content from the input file to the buffer
Author				:	Nisarg Patel
History/Versions	:	2018/09/26
Called Function		:	feof(),fgetc(),b_addc(),ungetc()
Parameters			:	const pBD - should not be NULL
const fi - Filestream variable
Return Value		:	Int : charCounter
Algorithm			:	Opens up the filestream using feof() to load the contents from the input file to the buffer using gfetc()
Function addc() is used to add the character on the buffer. ungetc() is used to return the character which
were not added to the filestream.Successfull character count is returned on completion
*******************************************************************************************************************************/
int b_load(FILE * const fi, Buffer * const pBD) {
	if (fi == NULL || pBD == NULL) {
		return RT_FAIL_1;
	}
	/* Stores value of character */
	char charStore;
	/* Counter to keep track of no of characters added to buffer */
	int charCounter = 0;
	/* Untill the end of file is reached */
	while (!feof(fi)) {
		/* character is stored into charStore one by one from the file*/
		charStore = (char)fgetc(fi);
		/* ignoring end of file character here */
		if (charStore != EOF) {
			/* if adding character to the buffer is not successfull*/
			if (!b_addc(pBD, charStore)) {
				ungetc(charStore, fi);
				printf("The last character read from the file is: %c %d\n", charStore, charStore);
				return LOAD_FAIL;
			}
			/* Character counter will be incremented if it is successfull */
			charCounter++;
		}
	}
	return charCounter;
}
/********************************************************************************************************************************
Purpose				:	This function is used to check if the buffer is empty or not
Author				:	Nisarg Patel
History/Versions	:	2018/09/26
Called Function		:		-
Parameters			:	const pBD - should not be NULL
Return Value		:	Int : TRUE(1),FALSE(0)
Algorithm			:		-
*******************************************************************************************************************************/
int b_isempty(Buffer* const pBD) {
	if (pBD == NULL) {
		return RT_FAIL_1;
	}
	else if (pBD->addc_offset == ZERO)
		return ONE;
	else
		return ZERO;
}
/********************************************************************************************************************************
Purpose				:	This function is used to get character one by one from the file stream
Author				:	Nisarg Patel
History/Versions	:	2018/09/26
Called Function		:		-
Parameters			:	const pBD - should not be NULL
Return Value		:	Char - getC_Char
Algorithm			:	Will set the eob bit to 1 if the buffer is full else it will be set to 0.After that it will store the
read character to getC_Char and is incremented before returning it.
*******************************************************************************************************************************/
char b_getc(Buffer * const pBD) {
	char getC_Char;
	/* Setting the end of buffer flag as 1 if reached end of buffer */
	if (pBD != NULL) {
		if (pBD->getc_offset == pBD->addc_offset) {
			/* Setting EOB flag 1 here as buffer is full */
			pBD->flags = pBD->flags | (ONE << SET_EOB);
			return ZERO;
		}
		/* If not end of buffer then value of eob bit in flags variable is set to 0 */
		pBD->flags = pBD->flags &~(ONE << RESET_EOB);
		//getC_Char = pBD->cb_head[pBD->getc_offset];
		getC_Char = *(pBD->cb_head + pBD->getc_offset);
		pBD->getc_offset++;
		return getC_Char;
	}
	else
		return RT_FAIL_2;
}
/********************************************************************************************************************************
Purpose				:	This function is used to return the current bit of the pBD->flags
Author				:	Nisarg Patel
History/Versions	:	2018/09/26
Called Function		:		-
Parameters			:	const pBD - should not be NULL
Return Value		:	Int : ONE - Bit is 1
ZERO - Bit is 0
Algorithm			:		-
*******************************************************************************************************************************/
int b_eob(Buffer * const pBD) {
	if (pBD == NULL) {
		return RT_FAIL_1;
	}
	int eobBit;
	/* Value of eob bit in flags variable is returned here */
	eobBit = pBD->flags & (ONE << CHECK_EOB);
	return eobBit;
}
/********************************************************************************************************************************
Purpose				:	This function is used to print the content of the buffer
Author				:	Nisarg Patel
History/Versions	:	2018/09/26
Called Function		:	b_isempty(),b_getc()
Parameters			:	const pBD - should not be NULL
Return Value		:	Int : elementCounter
Algorithm			:	It will check if the buffer is full or not if not then it will print the content of the buffer using
getc()
*******************************************************************************************************************************/
int b_print(Buffer * const pBD) {
	char elementValue;
	int elementCounter = 0;
	if (pBD == NULL) {
		return RT_FAIL_1;
	}
	else if (b_isempty(pBD)) {
		printf("Empty buffer!\n");
		return RT_FAIL_1;
	}
	else {
		/* Looping through the entire buffer to get character one by one and return it to the file untill it is full*/
		while (b_eob(pBD) != ONE) {
			elementValue = b_getc(pBD);
			/* So that it does not add the EOB character of the stream */
			if (b_eob(pBD)) { break; }
			printf("%c", elementValue);
			elementCounter++;
		}
		printf("\n");
	}
	return elementCounter;
}
/********************************************************************************************************************************
Purpose				:	This function is used to shrink or expand the buffer depending on the situation
Author				:	Nisarg Patel
History/Versions	:	2018/09/26
Called Function		:	realloc()
Parameters			:	const pBD - should not be NULL
symbol - Char
Return Value		:	Buffer pointer - pBD
Algorithm			:	memory for new_capacity is assigned(addc_offset+1) is variables of the buffer are set accordingly.
*******************************************************************************************************************************/
Buffer * b_compact(Buffer * const pBD, char symbol) {
	if (pBD == NULL) {
		return NULL;
	}
	short new_capacity = 0;
	/* The amount of memory needed for new_capacity will be stored in newCapArray */
	char *newCapArray = NULL;
	/* Assigning comparePointer = pBD->cb_head so that we can compare this pointer with newCapArray to check if the
	memory is expanded or shrinked	*/
	char *comparePointer = pBD->cb_head;
	new_capacity = (pBD->addc_offset) + 1;
	if (new_capacity < 0 || new_capacity >(SHRT_MAX)) {
		return NULL;
	}
	newCapArray = (char*)realloc(pBD->cb_head, sizeof(char) * new_capacity);
	if (newCapArray == NULL) {
		return NULL;
	}
	/*Both capacities are not equal meaning reallocation has happened,So we are setting the Rflag bit to 1 here */
	if (newCapArray != comparePointer) {
		pBD->flags = pBD->flags | (ONE << SET_R_FLAG);
	}
	else {
		/*Both capacities are equal meaning reallocation has not happened,So we are setting the Rflag bit to 0 here */
		pBD->flags = pBD->flags &~(ONE << SET_R_FLAG);
	}
	/* Assigning cb_head to newly created newCapArray*/
	pBD->cb_head = newCapArray;
	pBD->capacity = new_capacity;
	/*Storing the symbol at the current addc_offset position and then increments the same */
	*(pBD->cb_head + pBD->addc_offset) = symbol;
	pBD->addc_offset++;
	return pBD;
}
/********************************************************************************************************************************
Purpose				:	This function is used to return the bit value of the rflag bit in pBD->flags
Author				:	Nisarg Patel
History/Versions	:	2018/09/26
Called Function		:	-
Parameters			:	const pBD - should not be NULL
Return Value		:	Char
Algorithm			:	-
*******************************************************************************************************************************/
char b_rflag(Buffer * const pBD) {
	if (pBD == NULL) {
		return RT_FAIL_1;
	}
	/* rBit will hold the value of rflag bit (i.e zero or one) */
	char rBit;
	/* Value of r_flag bit in flags variable is returned here */
	rBit = (pBD->flags & (ONE << CHECK_R_FLAG));
	/* Above mentioned function will return '2' if the bit is one */
	if (rBit == TWO) {
		rBit = ONE;
		return rBit;
	}
	return rBit;
}
/********************************************************************************************************************************
Purpose				:	This function is used to decrement the value of getc_offset
Author				:	Nisarg Patel
History/Versions	:	2018/09/26
Called Function		:	-
Parameters			:	const pBD - should not be NULL
Return Value		:	Short - pBD->getc_offset
Algorithm			:	-
*******************************************************************************************************************************/
short b_retract(Buffer * const pBD) {
	if (pBD == NULL) {
		return RT_FAIL_1;
	}
	/* Value of get_offset is decremented and returned */
	if (pBD->getc_offset > 0)
		pBD->getc_offset--;
	return pBD->getc_offset;
}
/********************************************************************************************************************************
Purpose				:	This function is used to set the value of markc_offset to getc_offset
Author				:	Nisarg Patel
History/Versions	:	2018/09/26
Called Function		:	-
Parameters			:	const pBD - should not be NULL
Return Value		:	Short - pBD->getc_offset
Algorithm			:	-
*******************************************************************************************************************************/
short b_reset(Buffer * const pBD) {
	if (pBD == NULL) {
		return RT_FAIL_1;
	}
	pBD->getc_offset = pBD->markc_offset;
	return pBD->getc_offset;
}
/********************************************************************************************************************************
Purpose				:	This function is used to return the pBD->getc_offset
Author				:	Nisarg Patel
History/Versions	:	2018/09/26
Called Function		:	-
Parameters			:	const pBD - should not be NULL
Return Value		:	Short - pBD->getc_offset
Algorithm			:	-
*******************************************************************************************************************************/
short b_getcoffset(Buffer * const pBD) {
	if (pBD == NULL) {
		return RT_FAIL_1;
	}
	return pBD->getc_offset;
}
/********************************************************************************************************************************
Purpose				:	This function is used to set the getc_offset and markc_offset to 0
Author				:	Nisarg Patel
History/Versions	:	2018/09/26
Called Function		:	-
Parameters			:	const pBD - should not be NULL
Return Value		:	Int - Zero
Algorithm			:	-
*******************************************************************************************************************************/
int b_rewind(Buffer * const pBD) {
	if (pBD == NULL) {
		return RT_FAIL_1;
	}
	/* eBit will hold the value of eob flag(i.e zero or one)*/

	pBD->getc_offset = 0;
	pBD->markc_offset = 0;
	
	return ZERO;
}
/********************************************************************************************************************************
Purpose				:	This function is used to return the value of cb_head + loc_offset
Author				:	Nisarg Patel
History/Versions	:	2018/09/26
Called Function		:	-
Parameters			:	const pBD - should not be NULL
Short loc_offset
Return Value		:	Char - pBD->cb_head+loc_offset
Algorithm			:	-
*******************************************************************************************************************************/
char * b_location(Buffer * const pBD, short loc_offset) {
	if (pBD == NULL) {
		return NULL;
	}
	/*else if (loc_offset <= b_getcoffset(pBD)) {
		return NULL;
	}*/
	return pBD->cb_head + loc_offset;
}

