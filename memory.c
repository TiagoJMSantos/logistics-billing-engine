/**
 * @file memory.c
 * @brief Memory allocation and deallocation utilities.
 * @author ist1117516 (Tiago Santos)
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"

/** 
 * Allocates size bytes; exits with error message if allocation fails.
 * @param size	number of bytes to allocate
 * @return	pointer to allocated memory
 */
void *mylloc(int size) {
	void *value = malloc(size);
	if (value == NULL) exit(1);
	return value;
}

/** 
 * Reallocates memory for an array of amount elements of given size.
 * @param value		pointer to existing memory
 * @param size		size of each element in bytes
 * @param amount	number of elements
 * @return		pointer to reallocated memory
 */
void *my_realloc(void *value, int size, int amount) {
	void *new_value = realloc(value, size * amount);
	if (new_value == NULL) exit(1);
	return new_value;
}

/** 
 * Frees all dynamically allocated system memory before exit.
 * @param user_b	basket array
 * @param SysB		invoice array
 * @param cBills	number of invoices
 */
void myFree(basket *user_b, SysB *SysB, long cBills) {
	for (long i = 0; i < cBills; i++)
		free(SysB[i].clients_name);
	free(SysB);
	free(user_b);
}