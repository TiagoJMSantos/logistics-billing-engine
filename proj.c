/**
 * Billing and Logistics Management program made in C
 * @file proj.c
 * @author ist1117516 (Tiago Santos)
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "main_data_struct.h"
#include "aux_functions.c"

int add_product (char *u_input, SysP *SysP, int *c, basket *user_b, IVA *IStr, int iiva, int *pcB) {
    prod adding_prod;
    prod *pAdding_prod = &adding_prod;
    memset(pAdding_prod, 0, sizeof(prod));
    int d = process_args_p(u_input, pAdding_prod);
    if (d==0) return -1;
    int iva_id = checkForIVA(pAdding_prod, IStr, iiva);
    int product_id = detect_product_id(pAdding_prod, SysP, c);
    int basket_id = check_basket_for_product(pAdding_prod, user_b, pcB);
    int v = err_check(pAdding_prod, SysP, c, basket_id, iva_id, product_id);
    if (v==0) return 0;
    if (product_id != -1) {
        SysP[product_id].Product.stock += pAdding_prod->stock;
        SysP[*c].sold = 0;
        printf("%d\n", SysP[product_id].Product.stock);
        return SysP[product_id].Product.stock;
    } else {
        SysP[*c].Product = *pAdding_prod;
        (*c)++;
        printf("%d\n", pAdding_prod->stock);
        return pAdding_prod->stock;
    }
    return -1;
}

void list_product(SysP *SysP, int *c, char *u_input) {
    char input_copy[MAXBUFF];
    strncpy(input_copy, u_input, MAXBUFF - 1);
    input_copy[strcspn(input_copy, "\r\n")] = '\0';
    strtok(input_copy, " ");
    char *token = strtok(NULL, " ");
    if (token == NULL || strcmp(token, "*") == 0) {
        list_by_wildcard(SysP, c, "*");
        return;
    }
    while (token != NULL) {
        list_by_wildcard(SysP, c, token);
        token = strtok(NULL, " ");
    }
}

void add_to_basket(char *user_input, basket *user_b, int *pcB) {
    
    return;
}

int main(int argc, char *argv[]) {
    char input[MAXBUFF];
    int i, cB=0, cBills=0, c = 0;
    int *pC = &c, *pcB = &cB, *pcBills=&cBills;
    static SysP SysP[MAXPRODUCTS]; 
    for (i = 1; i < argc; i++) add_IVA_through_files(IStr, &iiva, argv[i]);
    while (fgets(input, MAXBUFF, stdin)) {
        switch(input[0]) {
            case 'q': return 0;
            case 'p': add_product(input, SysP, pC, user_b, IStr, iiva, pcB); break;
            case 'l': list_product(SysP, pC, input); break;
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