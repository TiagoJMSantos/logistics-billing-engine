/**
 * @file memory.h
 * @brief Memory allocation and deallocation utilities.
 * @author ist1117516 (Tiago Santos)
 */
#ifndef MEMORY_H
#define MEMORY_H
#include "main_data_struct.h"

void *mylloc(int size);
void *my_realloc(void *value, int size, int amount);
void myFree(basket *user_b, SysB *SysB, long cBills);

#endif