/**
 * Billing and Logistics Management program made in C
 * @file: main.c
 * @author: ist1117516 (Tiago Santos)
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "main_data_struct.h"
#include "functions_aux.c"

int add_product (char *user_input) {
    product adding_prod;
    product *pAdding_prod=NULL;
    pAdding_prod = &adding_prod;
    process_args(user_input, pAdding_prod);
    validate_errors(pAdding_prod);
    return pAdding_prod->stock;
}

int main() {
    char input[MAXBUFF];
    while (fgets(input, MAXBUFF, stdin)) {
        switch(input[0]) {
            case 'q': break;
            case 'p': add_product(input); break;
            case 'l': break;
            case 'a': break;
            case 'r': break;
            case 'f': break;
            case 'c': break;
            case 'd': break;
            default : puts(EINVALID); break;
        };
    }
    return 0;
}