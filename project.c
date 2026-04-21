/**
 * Billing and Logistics Management program made in C
 * @file project.c
 * @author ist1117516 (Tiago Santos)
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main_data_struct.h"
#include "memory.h"
#include "aux_functions.h"

/**
 * Adds a new product or updates an existing one in the system.
 * @param input Raw input line from stdin.
 * @param SysP Array of registered system products.
 * @param c Pointer to the total number of products.
 * @param user_b Array representing the current shopping basket.
 * @param IVA Array of valid IVA classes.
 * @param iiva Number of registered IVA classes.
 * @param pcB Pointer to the number of items in the basket.
 * @return The updated stock quantity, or -1 on error.
 */
int addProduct(char *input, SysP *SysP, int *c, basket *user_b, IVA *IVA,
                int iiva, long *pcB) {
    prod newProd;
    prod *pnewProd = &newProd;
    memset(pnewProd, 0, sizeof(prod));
    int d = processInput_P(input, pnewProd);
    if (d == 0) return -1;
    int ivaID = searchForIVA(pnewProd, IVA, iiva);
    int prodID = searchProductID(pnewProd->EAN, SysP, c);
    long basketID = searchProductBasketID(pnewProd, user_b, pcB);
    int v = checkErrorsP(pnewProd, SysP, c, basketID, ivaID, prodID);
    if (v == 0) return 0;
    if (prodID != -1) {
        SysP[prodID].Product.qty += pnewProd->qty;
        SysP[prodID].Product.price = pnewProd->price;
        SysP[prodID].Product.IVA = pnewProd->IVA;
        strcpy(SysP[prodID].Product.desc, pnewProd->desc);
        printf("%d\n", SysP[prodID].Product.qty);
        return SysP[prodID].Product.qty;
    } else {
        SysP[*c].Product = *pnewProd;
        (*c)++;
        printf("%d\n", pnewProd->qty);
        return pnewProd->qty;
    }
    return -1;
}

/**
 * Lists available system products, optionally filtering by wildcards.
 * @param SysP Array of registered system products.
 * @param c Pointer to the total number of products.
 * @param input Raw input line containing potential wildcards.
 */
void listProducts(SysP *SysP, int *c, char *input) {
    char input_copy[MAXBUFF];
    strncpy(input_copy, input, MAXBUFF - 1);
    input_copy[strcspn(input_copy, "\r\n")] = '\0';
    strtok(input_copy, " ");
    char *token = strtok(NULL, " ");
    if (token == NULL || strcmp(token, "*") == 0) {
        listByWildcard(SysP, c, "*");
        return;
    }
    while (token != NULL) {
        listByWildcard(SysP, c, token);
        token = strtok(NULL, " ");
    }
}

/**
 * Modifies the shopping basket by adding or removing product quantities.
 * @param SysP Array of registered system products.
 * @param input Raw input line specifying the EAN and quantity.
 * @param IVA Array of valid IVA classes.
 * @param iiva Number of registered IVA classes.
 * @param user_b Pointer to the shopping basket array pointer.
 * @param pcB Pointer to the number of items in the basket.
 * @param pC Pointer to the total number of products.
 * @param max_b Pointer to the maximum capacity of the basket array.
 */
void addBasket(SysP *SysP, char *input, IVA *IVA, int iiva, basket **user_b,
                long *pcB, int *pC, int *max_b) {
    char ean[EAN_DIGITS];
    int qty = 1;
    long bid = -1;
    if (!processInput_A(input, ean, &qty)) {
        for (long i = 0; i < *pcB; i++) {
            if ((*user_b)[i].Product.qty == 0) continue;
            int ivaID = searchForIVA(&(*user_b)[i].Product, IVA, iiva);
            long itemCents = calculateTotalCents((*user_b)[i].Product.price, (*user_b)[i].Product.qty, IVA[ivaID].IVA_PERCENTAGE);
            printBasket(*user_b, (double)itemCents / 100.0, i);
        }
        return;
    }
    int pid = checkErrorsA(ean, qty, SysP, pC, *user_b, pcB, &bid);
    if (pid == -1) return;
    if (bid == -1) {
        insertProductInBasket(user_b, pcB, SysP, pid, qty, max_b);
        for (long i=0;i<*pcB;i++)
            if (strcmp((*user_b)[i].Product.EAN, SysP[pid].Product.EAN) == 0) {
                bid = i;
                break;
            }
    } else updateProductInBasket(*user_b, bid, SysP, pid, qty);
    SysP[pid].Product.qty -= qty;
    SysP[pid].sold += qty;
    int ivaID = searchForIVA(&(*user_b)[bid].Product, IVA, iiva);
    long itemCents = calculateTotalCents((*user_b)[bid].Product.price, (*user_b)[bid].Product.qty, IVA[ivaID].IVA_PERCENTAGE);
    printBasket(*user_b, (double)itemCents / 100.0, bid);
    if ((*user_b)[bid].Product.qty == 0) {
        int numToMove = (*pcB) - bid - 1;
        if (numToMove > 0) {
            memmove(&(*user_b)[bid], &(*user_b)[bid + 1], numToMove * sizeof(basket));
        }
        (*pcB)--;
    }
    return;
}

/**
 * Processes the basket checkout, generates an invoice, and clears the basket.
 * @param input Raw input line specifying optional NIF and client name.
 * @param SysB Pointer to the dynamic array of invoices.
 * @param SysP Array of registered system products.
 * @param pcBills Pointer to the total number of invoices.
 * @param pcB Pointer to the number of items in the basket.
 * @param user_b Array representing the current shopping basket.
 * @param pC Pointer to the total number of products.
 * @param IVA Array of valid IVA classes.
 * @param iiva Number of registered IVA classes.
 * @param totalBills Pointer to the cumulative count of emitted invoices.
 */
void billBasket(char *input, SysB **SysB, SysP *SysP, long *pcBills,
        long *pcB, basket *user_b, int *pC, IVA *IVA, int iiva, long *totalBills) {
    char nif[NIF_LENGTH], *name = NULL;
    processInput_F(input, nif, &name);
    if (checkErrorsF(nif, name) == -1) { free(name); return; }
    if (strcmp(name, "error") == 0) {
        for (long i = 0; i < *pcB; i++) {
            SysP[searchProductID(user_b[i].Product.EAN, SysP, pC)].Product.qty
                    += user_b[i].Product.qty;
            int pid = searchProductID(user_b[i].Product.EAN, SysP, pC);
            SysP[pid].Product.qty += user_b[i].Product.qty;
            SysP[pid].sold -= user_b[i].Product.qty;
        }
        *pcB = 0;
        free(name);
        return;
    }
    long totalCentsSum = 0;
    for (long i = 0; i < *pcB; i++) {
        int iva_id = searchForIVA(&user_b[i].Product, IVA, iiva);
        totalCentsSum += calculateTotalCents(user_b[i].Product.price, user_b[i].Product.qty, IVA[iva_id].IVA_PERCENTAGE);
    }
    double totalDouble = (double)totalCentsSum / 100.0;
    int total_qty = 0;
    for (long i = 0; i < *pcB; i++) total_qty += user_b[i].Product.qty;
    long newID = *pcBills + 1;
    insertBill(SysB, pcBills, nif, name, pcB, totalDouble, total_qty);
    printf("%d %.2f %ld\n", total_qty, totalDouble, newID);
    *pcB = 0;
    (*totalBills)++;
    free(name);
}

/** 
 * Prints a billing summary, either system-wide or for a specific product.
 * @param input Raw input line specifying an optional product EAN.
 * @param SysP Array of registered system products.
 * @param SysB Array of registered invoices.
 * @param pcBills Total number of invoices.
 * @param pC Total number of products.
 * @param IVA Array of valid IVA classes.
 * @param iiva Number of registered IVA classes.
 * @param totalBills Cumulative count of emitted invoices.
 */
void billsSummary(char *input, SysP *SysP, SysB *SysB, 
            long *pcBills, int *pC, IVA *IVA, int iiva, long *totalBills) {
    char EAN[EAN_DIGITS];
    char *pEAN = EAN;
    int res = processInput_R(input, pEAN);
    if (!res) {
        long total_s = getSoldProd(SysB, pcBills);
        double total_b = getBillsTotal(SysB, pcBills);
        printf("%ld %ld %.2f\n", total_s, *totalBills, total_b);
        for (int i=0;i<iiva;i++) {
            printf("%c %d%%\n", IVA[i].IVA_LETTER, IVA[i].IVA_PERCENTAGE);
        }
        return;
    }
    int pid = checkErrorsR(pEAN, SysP, pC);
    if (pid == -1) return; 
    printf("%d %d %s\n", SysP[pid].Product.qty, SysP[pid].sold, SysP[pid].Product.desc);
    return; 
} 

/**
 * Lists registered invoices, either all of them or filtered by a client's name.
 * @param input Raw input line specifying an optional client name.
 * @param SysB Array of registered invoices.
 * @param pcBills Total number of invoices.
 */
void printBills(char *input, SysB *SysB, long *pcBills) {
    char *name = NULL;
    int a = processInput_C(input, &name);
    if (!a) {
        for (long i = 0; i < *pcBills; i++) {
            double finalPrice = (double)roundToCents(SysB[i].value) / 100.0;
            printf("%ld %.2f %s\n", SysB[i].id, finalPrice, SysB[i].clients_name);
        }
        return;
    }
    if (checkErrorsC(name) == -1) { 
        free(name); 
        return; 
    }
    int found = 0;
    size_t name_len = strlen(name);
    for (long i = 0; i < *pcBills; i++) {
        if (strncmp(name, SysB[i].clients_name, name_len) == 0) {
            double finalPrice = (double)roundToCents(SysB[i].value) / 100.0;
            printf("%ld %.2f %s\n", SysB[i].id, finalPrice, SysB[i].clients_name);
            found = 1; 
        }
    }
    if (!found) {
        printf("%s: %s\n", name, NO_CLIENT); 
    }
    free(name);
    return;
}

/**
 * Removes an invoice or reduces the stock/removes a system product.
 * @param input Raw input line specifying the ID or EAN and quantity.
 * @param SysB Pointer to the dynamic array of invoices.
 * @param pcBills Pointer to the total number of invoices.
 * @param SysP Array of registered system products.
 * @param pC Pointer to the total number of products.
 * @param user_b Array representing the current shopping basket.
 * @param pcB Pointer to the number of items in the basket.
 */
void removeProductOrBill(char *input, SysB **SysB, long *pcBills, 
                         SysP *SysP, int *pC, basket *user_b, long *pcB) {
    char arg1[EAN_DIGITS];
    int qty = -1;
    int argsAmount = processInput_D(input, arg1, &qty);
    if (argsAmount == 0) return;
    long pbid = checkErrorsD(argsAmount, arg1, qty, *SysB, SysP, pcBills, pC, user_b, pcB);
    if (pbid == -1) return;
    if (argsAmount == 1) {
        double val = (double)roundToCents((*SysB)[pbid].value) / 100.0;
        printf("%.2f %s %s\n", val, (*SysB)[pbid].NIF, (*SysB)[pbid].clients_name);
        removeBill(*SysB, pcBills, pbid);
        return;
    }
    int leftStock = SysP[pbid].Product.qty - qty;
    if (leftStock > 0) {
        SysP[pbid].Product.qty -= qty;
        printf("%d %s\n", leftStock, SysP[pbid].Product.desc);
        return;
    }
    printf("0 %s\n", SysP[pbid].Product.desc);
    removeProduct(SysP, pC, pbid);
    return;
}

// 1. Função auxiliar para o qsort organizar as faturas
int compareBills(const void *a, const void *b) {
    SysB *billA = (SysB *)a;
    SysB *billB = (SysB *)b;
    
    // Primeiro: Ordenar alfabeticamente pelo nome
    int cmp = strcmp(billA->clients_name, billB->clients_name);
    if (cmp != 0) {
        return cmp;
    }
    // Segundo: Se for o mesmo cliente, ordenar por ID (ordem cronológica)
    if (billA->id < billB->id) return -1;
    if (billA->id > billB->id) return 1;
    return 0;
}


void changeName(char *input, SysB **SysB, long *pcBills) {
    char oldName[50], newName[50];
    if (sscanf(input, "%*s %s %s", oldName, newName) != 2) {
        return;
    }
    long c = searchName(*SysB, oldName, pcBills);
    if (c == -1) { 
        printf("%s: %s\n", oldName, NO_CLIENT); 
        return; 
    }
    for (long i = 0; i < *pcBills; i++) {
        if (strcmp((*SysB)[i].clients_name, oldName) == 0) {
            free((*SysB)[i].clients_name);
            (*SysB)[i].clients_name = strdup(newName);
        }
    }
    printf("%s\n", newName);
}

/**
 * Main application entry point handling the command loop.
 * @param argc Argument count.
 * @param argv Argument vector (optional IVA file path).
 * @return 0 on standard completion.
 */
int main(int argc, char *argv[]) {
    char input[MAXBUFF];
    // Static array holding all registered system products.
    static SysP SysP[MAXPRODUCTS];
    IVA IVA_S[26]; IVA *IVA = IVA_S;
    basket *user_b = mylloc(sizeof(basket));
    memset(user_b, 0, sizeof(basket));
    SysB *PSysB = mylloc(sizeof(SysB));
    int c = 0, iiva = 0, max_basket = 1;
    long cBills = 0, cB = 0, totalBillsEmited = 0;
    int *pC = &c, *pmax_b = &max_basket;
    long *pcBills = &cBills, *pcB = &cB, *pTotalBills = &totalBillsEmited;
    iiva = createIVATable(IVA, argc);
    for (int i = 1; i < argc; i++) createIVATableFile(IVA, &iiva, argv[i]);
    while (fgets(input, MAXBUFF, stdin)) {
        switch (input[0]) {
        case 'q': myFree(user_b, PSysB, cBills); return 0;
        case 'p': addProduct(input, SysP, pC, user_b, IVA, iiva, pcB); break;
        case 'l': listProducts(SysP, pC, input); break;
        case 'a': addBasket(SysP, input, IVA, iiva, &user_b, pcB, pC, pmax_b); break;
        case 'r': billsSummary(input, SysP, PSysB, pcBills, pC, IVA, iiva, pTotalBills); break;
        case 'f': billBasket(input, &PSysB, SysP, pcBills, pcB, user_b, pC, IVA, iiva, pTotalBills); break;
        case 'c': printBills(input, PSysB, pcBills); break;
        case 'd': removeProductOrBill(input, &PSysB, pcBills, SysP, pC, user_b, pcB); break;
        case 'u': changeName(input, &PSysB, pcBills); break;
        default: puts(EINVALID); break;
        }
    }
    myFree(user_b, PSysB, cBills);
    return 0;
}