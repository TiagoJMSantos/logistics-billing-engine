/**
 * @file aux_functions.c
 * @brief Auxiliary functions for the billing and logistics system.
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
 * Parses product command input into a prod struct.
 * @param user_input	Raw input line from stdin
 * @param proc_prod	    Product struct to populate
 * @return		        1 on success, 0 if description is too long
 */
int processInput_P(char *user_input, prod *proc_prod) {
    user_input[strcspn(user_input, "\r\n")] = '\0';
    char temp_desc[MAXBUFF];
    temp_desc[0] = '\0'; 
    sscanf(user_input, "%*s %s %c %lf %d %65535[^\n]", proc_prod->EAN,
            &proc_prod->IVA, &proc_prod->price, &proc_prod->qty, temp_desc);
    proc_prod->EAN[EAN_DIGITS - 1] = '\0';
    if (strlen(temp_desc) >= desc_max_char) {
        puts(INVALID_DESCRIPTION);
        return 0;
    }
    strncpy(proc_prod->desc, temp_desc, desc_max_char - 1);
    proc_prod->desc[desc_max_char - 1] = '\0';
    return 1;
}


/** 
 * Calculates the EAN check digit and sets ean_size.
 * @param EAN		EAN string to process
 * @param ean_size	Output: length of the EAN string
 * @return		    Expected check digit
 */
int calculateEAN(char *EAN, int *ean_size) {
    int i, sum = 0;
    *ean_size = strlen(EAN);
    for (i = 0; i < *ean_size - 1; i++) {
        int digit = EAN[i] - '0';
        if (i % 2 == 0) sum += digit;
        else sum += (digit * 3);
    }
    int verification_digit = (10 - (sum % 10)) % 10;
    return verification_digit;
}

/** 
 * Loads IVA rates from a file into the IVA table in sorted order.
 * @param PIVA		IVA array to populate
 * @param counter	Current number of IVA entries
 * @param filename	Path to the IVA file
 */
void createIVATableFile(IVA *PIVA, int *counter, const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) return;
    while (fscanf(fp, " %c %d", &PIVA[*counter].IVA_LETTER,
                  &PIVA[*counter].IVA_PERCENTAGE) == 2) {
        int i = *counter;
        while (i > 0 && PIVA[i-1].IVA_LETTER > PIVA[i].IVA_LETTER) {
            IVA tmp = PIVA[i-1];
            PIVA[i-1] = PIVA[i];
            PIVA[i] = tmp;
            i--;
        }
        (*counter)++;
    }
    fclose(fp);
}

/** 
 * Initialises default IVA table (A=0, B=6, C=13, D=23) if no file given.
 * @param IVA	IVA array to initialise
 * @param argc	Argument count from main
 * @return	    Number of IVA entries initialised
 */
int createIVATable(IVA *IVA, int argc) {
    int counter=0;
    if (argc>1) return 0;
    IVA[0].IVA_LETTER = 'A'; IVA[0].IVA_PERCENTAGE = 0;
    IVA[1].IVA_LETTER = 'B'; IVA[1].IVA_PERCENTAGE = 6;
    IVA[2].IVA_LETTER = 'C'; IVA[2].IVA_PERCENTAGE = 13;
    IVA[3].IVA_LETTER = 'D'; IVA[3].IVA_PERCENTAGE = 23;
    counter = 4;
    return counter;
}

/** 
 * Returns the index of a product by EAN, or -1 if not found.
 * @param EAN		    EAN string to search
 * @param Products_List	System product array
 * @param counter	    Number of registered products
 * @return		        Product index, or -1 if not found
 */
int searchProductID(char *EAN, SysP *Products_List, int *counter) {
    if (*counter == 0) return -1;
    for (int i = 0; i < *counter; i++) {
        if (strcmp(Products_List[i].Product.EAN, EAN) == 0) {
            return i;
        }
    }
    return -1;
}

/** 
 * Returns the IVA index for a product using binary search.
 * @param product	Product whose IVA letter to look up
 * @param iva_list	Sorted IVA array
 * @param ivaLEN	Number of IVA entries
 * @return		    IVA index, or -1 if not found
 */
int searchForIVA(prod *product, IVA *iva_list, int ivaLEN) {
    int left = 0;
    int right = ivaLEN - 1;
    char target = product->IVA;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (iva_list[mid].IVA_LETTER == target) {
            return mid;
        } else if (iva_list[mid].IVA_LETTER < target) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return -1;
}

/**
 * Searches for a product's index within the shopping basket using binary search.
 * @param prod Pointer to the product to search for.
 * @param bask Array representing the current shopping basket.
 * @param pcB Pointer to the number of items in the basket.
 * @return The index of the product in the basket, or -1 if not found.
 */
long searchProductBasketID(prod *prod, basket *bask, long *pcB) {
    long left = 0;
    long right = (*pcB) - 1;
    while (left <= right) {
        long mid = left + (right - left) / 2;
        int cmp = strcmp(bask[mid].Product.EAN, prod->EAN);
        if (cmp == 0) {
            return mid;
        } else if (cmp < 0) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return -1;
}

/**
 * Validates a product before it is added or updated in the system.
 * @param prod Pointer to the product to validate.
 * @param SysP Array of registered system products.
 * @param c Pointer to the total number of products.
 * @param b_id The product's index in the basket, or -1.
 * @param iva_id The product's IVA index, or -1.
 * @param pID The product's index in the system, or -1.
 * @return 1 if the product is valid, 0 if an error is found and printed.
 */
int checkErrorsP(prod *prod, SysP *SysP, int *c, long b_id, int iva_id, int pID) {
    int ean_size = 0;
    int *pean_s = &ean_size;
    int ean_digit = calculateEAN(prod->EAN, pean_s);
    char *error = NULL;
    if (*c >= MAXPRODUCTS) error = INVALID_PRODUCT;
    else if (prod->price <= 0) error = INVALID_PRICE;
    else if (prod->qty < 0) error = INVALID_QUANTITY;
    else if (iva_id == -1) error = INVALID_IVA;
    else if ((prod->EAN[ean_size - 1] - '0') != ean_digit)
        error = INVALID_EAN;
    else if (islower((unsigned char)prod->desc[0])) {
        error = INVALID_DESCRIPTION;
    } else if ((b_id != -1) && (pID != -1) &&
                (prod->price != SysP[pID].Product.price))
        error = PRODUCT_IN_USE;
    unsigned int desc_size = strlen(prod->desc);
    for (unsigned int i=0; i<desc_size; i++) {
        if (prod->desc[i] == '('|| 
            prod->desc[i] == ')' || 
            prod->desc[i] == '{' ||
            prod->desc[i] == '}' ||
            prod->desc[i] == '[' ||
            prod->desc[i] == ']'
        ) {
            puts("invalid character in description");
            return 0;
        }
    }
    if (error != NULL) {
        puts(error);
        return 0;
    }
    return 1;
}

/**
 * Checks if a string matches a given pattern containing wildcards (*, ?).
 * @param pattern The wildcard pattern to match against.
 * @param str The string to evaluate.
 * @return 1 if the string matches the pattern, 0 otherwise.
 */
int wildcardMatch(const char *pattern, const char *str) {
    const char *last_star = NULL, *last_str = str;
    while (*str) {
        if (*pattern == '*') {
            last_star = ++pattern;
            last_str = str;
        } else if (*pattern == '?' || *pattern == *str) {
            pattern++;
            str++;
        } else if (last_star) {
            pattern = last_star;
            str = ++last_str;
        } else return 0;
    }
    while (*pattern == '*') pattern++;
    return *pattern == '\0';
}

/**
 * Prints the details of a single product.
 * @param SysP Array of registered system products.
 * @param i Index of the product to print.
 */
void printProduct(SysP *SysP, int i) {
    printf("%s %c %.2f %d %d %s\n", 
        SysP[i].Product.EAN, 
        SysP[i].Product.IVA,
        SysP[i].Product.price, 
        SysP[i].sold, SysP[i].Product.qty,
        SysP[i].Product.desc);
}

/**
 * Prints the details of a single item in the shopping basket.
 * @param user_b Array representing the current shopping basket.
 * @param totalIVA The calculated total value including IVA.
 * @param i Index of the item in the basket.
 */
void printBasket(basket *user_b, double totalIVA, int i) {
    printf("%c %.2f %d %.2f %s\n", 
        user_b[i].Product.IVA, 
        user_b[i].Product.price,    
        user_b[i].Product.qty, 
        totalIVA, 
        user_b[i].Product.desc);
}

/**
 * Lists all system products that match a wildcard pattern.
 * @param SysP Array of registered system products.
 * @param c Pointer to the total number of products.
 * @param pattern The wildcard pattern to evaluate.
 * @return 1 if at least one product was found, 0 otherwise.
 */
int listByWildcard(SysP *SysP, int *c, char *pattern) {
    int i, found = 0;
    for (i=0;i<*c;i++) {
    if (SysP[i].Product.qty > 0 &&
        wildcardMatch(pattern, SysP[i].Product.EAN)) {
        printProduct(SysP, i);
        found = 1;
    }
    }
    if (!found) printf("%s: no such product\n", pattern);
    return found;
}

/**
 * Parses the basket addition input to extract the EAN and quantity.
 * @param user_input Raw input line from stdin.
 * @param out_ean Buffer to store the extracted EAN.
 * @param out_qty Pointer to store the extracted quantity.
 * @return 1 on successful parse, 0 if arguments are missing.
 */
int processInput_A(char *user_input, char *out_ean, int *out_qty) {
    char input_copy[MAXBUFF];
    strncpy(input_copy, user_input, MAXBUFF - 1);
    input_copy[strcspn(input_copy, "\r\n")] = '\0';
    strtok(input_copy, " ");
    char *t1 = strtok(NULL, " ");
    char *t2 = strtok(NULL, " ");
    if (!t1) return 0;
    if (!t2) *out_qty = 1;
    else {
        *out_qty = atoi(t1);
        t1 = t2;
    }
    strncpy(out_ean, t1, EAN_DIGITS - 1);
    out_ean[EAN_DIGITS - 1] = '\0';
    return 1;
}

/**
 * Validates basket addition requests and checks for adequate stock.
 * @param ean The EAN of the product to add.
 * @param qty The quantity to add or remove.
 * @param SysP Array of registered system products.
 * @param c Pointer to the total number of products.
 * @param user_b Array representing the current shopping basket.
 * @param pcB Pointer to the number of items in the basket.
 * @param bid Pointer to store the product's index in the basket if it exists.
 * @return The product's index in the system, or -1 if an error occurred.
 */
int checkErrorsA(char *ean, int qty, SysP *SysP, int *c, basket *user_b,
            long *pcB, long *bid) {
    int eanSize = 0;
    prod temp;
    memset(&temp, 0, sizeof(prod));
    strncpy(temp.EAN, ean, EAN_DIGITS - 1);
    temp.EAN[EAN_DIGITS - 1] = '\0';
    int ean_digit = calculateEAN(ean, &eanSize);
    char *error = NULL;
    if ((temp.EAN[eanSize-1]-'0') != ean_digit) error = INVALID_EAN;
    else {
        int pid = searchProductID(ean, SysP, c);
        if (pid == -1) {
            printf("%s: %s\n", ean, NO_PRODUCT);
            return -1;
        }
        *bid = searchProductBasketID(&temp, user_b, pcB);
        int basket_qty = (*bid != -1) ? user_b[*bid].Product.qty : 0;
        if (qty > 0 && SysP[pid].Product.qty <= 0) error = NO_STOCK;
        else if (qty < 0 && basket_qty + qty < 0)
            error = INVALID_QUANTITY;
        if (error != NULL) {
            puts(error);
            return -1;
        }
        return pid;
    }
    puts(error);
    return -1;
}

/**
 * Calculates the total cost in cents, including IVA, with proper rounding.
 * @param price The base unit price of the product.
 * @param qty The quantity of the product.
 * @param ivaPercent The IVA percentage to apply.
 * @return The calculated total cost in cents.
 */
long calculateTotalCents(double price, int qty, int ivaPercent) {
    double ivaFactor = 1.0 + (ivaPercent / 100.0);
    double totalValue = price * qty * ivaFactor;
    long totalCents = (long)(totalValue * 100.0 + 0.500001);
    return totalCents;
}

/**
 * Rounds a floating-point value to the nearest whole cent.
 * @param value The value to round.
 * @return The rounded value in cents.
 */
long roundToCents(double value) {
    if (value >= 0)
        return (long)(value * 100.0 + 0.500001);
    else
        return (long)(value * 100.0 - 0.500001);
}

/**
 * Updates the quantity of an existing product in the shopping basket.
 * @param user_b Array representing the current shopping basket.
 * @param bid Index of the product in the basket.
 * @param SysP Array of registered system products.
 * @param pid Index of the product in the system array.
 * @param qty Quantity to add or remove.
 */
void updateProductInBasket(basket *user_b, int bid, SysP *SysP, int pid,
                           int qty) {
    user_b[bid].Product.qty += qty;
    user_b[bid].Product.price = SysP[pid].Product.price;
    user_b[bid].Product.IVA = SysP[pid].Product.IVA;
    memcpy(user_b[bid].Product.EAN, SysP[pid].Product.EAN, EAN_DIGITS - 1);
    memcpy(user_b[bid].Product.desc, SysP[pid].Product.desc, desc_max_char - 1);
}

/**
 * Inserts a new product into the shopping basket, maintaining EAN order.
 * @param user_b Pointer to the shopping basket array pointer.
 * @param pcB Pointer to the number of items in the basket.
 * @param SysP Array of registered system products.
 * @param pid Index of the product in the system array.
 * @param qty Quantity of the product to add.
 * @param max_b Pointer to the current capacity of the basket array.
 */
void insertProductInBasket(basket **user_b, long *pcB, SysP *SysP, int pid,
                           int qty, int *max_b) {
    if (*pcB >= *max_b) {
        *max_b *= 2;
        *user_b = my_realloc(*user_b, sizeof(basket), *max_b);
        memset(*user_b + *pcB, 0, sizeof(basket) * (*max_b - *pcB));
    }
    int i = *pcB;
    while (i > 0 && strcmp((*user_b)[i-1].Product.EAN, SysP[pid].Product.EAN) > 0) {
        (*user_b)[i] = (*user_b)[i-1];
        i--;
    }
    (*user_b)[i].Product.qty = qty;
    (*user_b)[i].Product.price = SysP[pid].Product.price;
    (*user_b)[i].Product.IVA = SysP[pid].Product.IVA;
    memcpy((*user_b)[i].Product.EAN, SysP[pid].Product.EAN, EAN_DIGITS);
    memcpy((*user_b)[i].Product.desc, SysP[pid].Product.desc, desc_max_char);
    (*pcB)++;
}

/**
 * Extracts a client name from tokens, handling optional quotes.
 * @param token The first part of the client name.
 * @param rest The remaining parts of the client name.
 * @param name Pointer to dynamically allocate and store the full name.
 */
void extract_name(char *token, char *rest, char **name) {
    char tmp[MAXBUFF];
    if (token[0] != '"') { *name = strdup(token); return; }
    if (rest) snprintf(tmp, MAXBUFF, "%s %s", token + 1, rest);
    else snprintf(tmp, MAXBUFF, "%s", token + 1);
    int len = strlen(tmp);
    if (len == 0 || tmp[len-1] != '"') { *name = strdup(""); return; }
    tmp[len-1] = '\0';
    *name = strdup(tmp);
}
/**
 * Parses the billing input to extract the NIF and client name.
 * @param input Raw input line from stdin.
 * @param nif Buffer to store the extracted NIF.
 * @param name Pointer to dynamically allocate and store the client name.
 */
void processInput_F(char *input, char *nif, char **name) {
    char input_copy[MAXBUFF];
    strncpy(input_copy, input, MAXBUFF - 1);
    input_copy[MAXBUFF - 1] = '\0';
    input_copy[strcspn(input_copy, "\r\n")] = '\0';
    strtok(input_copy, " ");
    char *t1 = strtok(NULL, " ");
    if (!t1) {
        memcpy(nif, "999999999", NIF_LENGTH);
        *name = strdup("Cliente final");
        return;
    }
    if (t1[0] == '"') {
        memcpy(nif, "999999999", NIF_LENGTH);
        extract_name(t1, strtok(NULL, ""), name);
        return;
    }
    char *t2 = strtok(NULL, "");
    if (!t2) {
        memcpy(nif, "999999999", NIF_LENGTH);
        *name = strdup(t1);
        return;
    }
    strncpy(nif, t1, NIF_LENGTH - 1);
    nif[NIF_LENGTH - 1] = '\0';
    extract_name(t2, NULL, name);
}

/**
 * Validates the extracted client name and NIF structure.
 * @param nif The NIF string to validate.
 * @param name The client name to validate.
 * @return 0 on success, -1 if validation fails.
 */
int checkErrorsF(char *nif, char *name) {
    int i, len = strlen(nif);
    if (strlen(name) == 0 || !isalpha((unsigned char)name[0])) {
        puts(INVALID_NAME);
        return -1;
    }
    if (len != 9 || nif[0] == '0') {
        printf("%s: %s\n", nif, INVALID_NIF);
        return -1;
    }
    for (i = 0; i < len; i++) {
        if (!isdigit((unsigned char)nif[i])) {
            printf("%s: %s\n", nif, INVALID_NIF);
            return -1;
        }
    }
    return 0;
}
/**
 * Restores product stock from an aborted basket and resets the basket.
 * @param pcB Pointer to the number of items in the basket.
 * @param user_b Array representing the current shopping basket.
 * @param SysP Array of registered system products.
 * @param pC Pointer to the total number of products.
 */
void cancelBasket(long *pcB, basket *user_b, SysP *SysP, int *pC) {
    for (int i = 0; i < *pcB; i++) {
        int prodID = searchProductID(user_b[i].Product.EAN, SysP, pC);
        SysP[prodID].Product.qty += user_b[i].Product.qty;
        SysP[prodID].sold -= user_b[i].Product.qty;
    }
    *pcB = 0;
}

/**
 * Creates and stores a new invoice, maintaining alphabetical order by client.
 * @param ptr_SysB Pointer to the dynamic array of invoices.
 * @param pcBills Pointer to the total number of invoices.
 * @param nif The client's NIF.
 * @param name The client's name.
 * @param pcB Pointer to the number of distinct items in the basket.
 * @param value The total calculated value of the invoice.
 * @param total_qty The total unit quantity of all products.
 */
void insertBill(SysB **ptr_SysB, long *pcBills, char *nif, char *name,
                 long *pcB, double value, long total_qty) {
    *ptr_SysB = my_realloc(*ptr_SysB, sizeof(SysB), *pcBills + 1);
    long i = *pcBills;
    SysB *sb = *ptr_SysB;
    long newId = *pcBills + 1;
    while (i > 0 && strcmp(sb[i-1].clients_name, name) > 0) {
        sb[i] = sb[i-1];
        i--;
    }
    memcpy(sb[i].NIF, nif, NIF_LENGTH - 1);
    sb[i].NIF[NIF_LENGTH - 1] = '\0';
    sb[i].clients_name = strdup(name);
    sb[i].id = newId;
    sb[i].qty = *pcB;
    sb[i].value = value;
    sb[i].total_qty = total_qty;
    (*pcBills)++;
}

/**
 * Parses the reporting input to extract an optional EAN.
 * @param input Raw input line from stdin.
 * @param ean Buffer to store the extracted EAN.
 * @return 1 if an EAN was provided, 0 if it is a general report request.
 */
int processInput_R(char *input, char *ean) {
    char input_copy[MAXBUFF];
    strncpy(input_copy, input, MAXBUFF - 1);
    input_copy[strcspn(input_copy, "\r\n")] = '\0';
    strtok(input_copy, " ");
    char *t1 = strtok(NULL, " ");
    if (!t1) return 0;
    strncpy(ean, t1, EAN_DIGITS - 1);
    ean[EAN_DIGITS - 1] = '\0';
    return 1;
}

/**
 * Validates the EAN provided for a specific product report.
 * @param EAN The EAN to search for.
 * @param SysP Array of registered system products.
 * @param pC Pointer to the total number of products.
 * @return The product's index in the system, or -1 if errors occur.
 */
int checkErrorsR(char *EAN, SysP *SysP, int *pC) {
    int pid = searchProductID(EAN, SysP, pC);
    int ean_size = strlen(EAN);
    int *pean_size = &ean_size;
    int ean_digit = calculateEAN(EAN, pean_size);
    if ((EAN[ean_size-1]-'0') != ean_digit) {
        puts(INVALID_EAN);
        return -1;
    }
    else if (pid == -1) {
        printf("%s: %s\n", EAN, NO_PRODUCT);
        return -1;
    }
    return pid;
}

/**
 * Sums up the total quantity of products sold across all invoices.
 * @param SysB Array of registered invoices.
 * @param pcBills Pointer to the total number of invoices.
 * @return The overall total quantity of items sold.
 */
long getSoldProd(SysB *SysB, long *pcBills) {
    long total = 0;
    for (long i = 0; i < *pcBills; i++)
        total += SysB[i].total_qty;
    return total;
}

/**
 * Sums up the aggregate monetary value of all system invoices.
 * @param SysB Array of registered invoices.
 * @param pcBills Pointer to the total number of invoices.
 * @return The total billed amount across all invoices.
 */
double getBillsTotal(SysB *SysB, long *pcBills) {
    long totalCents = 0;
    for (long i = 0; i < *pcBills; i++)
        totalCents += roundToCents(SysB[i].value);
    return totalCents / 100.0;
}

/**
 * Parses client listing input to extract an optional client name.
 * @param input Raw input line from stdin.
 * @param name Pointer to dynamically allocate and store the client name.
 * @return 1 if a name was provided, 0 otherwise.
 */
int processInput_C(char *input, char **name) {
    char input_copy[MAXBUFF];
    strncpy(input_copy, input, MAXBUFF - 1);
    input_copy[MAXBUFF - 1] = '\0';
    input_copy[strcspn(input_copy, "\r\n")] = '\0';
    strtok(input_copy, " ");
    char *t1 = strtok(NULL, " ");
    if (!t1) { *name = NULL; return 0; }
    if (t1[0] == '"') {
        extract_name(t1, strtok(NULL, ""), name);
        return 1;
    }
    char *t2 = strtok(NULL, "");
    extract_name(t1, t2, name);
    return 1;
}

/**
 * Validates a client name string for listing requests.
 * @param name The client name to validate.
 * @return 0 on success, -1 if the name is invalid.
 */
int checkErrorsC(char *name) {
    char *error = NULL;
    if (strlen(name) == 0 || !isalpha((unsigned char)name[0])) error = INVALID_NAME;
    if(error!=NULL){
        puts(error);
        return -1;
    }
    return 0;
}

/**
 * Finds the first invoice associated with a specific client using binary search.
 * @param SysB Array of registered invoices.
 * @param name The client name to search for.
 * @param pcBills Pointer to the total number of invoices.
 * @return The index of the first matched invoice, or -1 if not found.
 */
long searchName(SysB *SysB, char *name, long *pcBills) {
    long left = 0;
    long right = (*pcBills) - 1;
    long first_found = -1;
    while (left <= right) {
        long mid = left + (right - left) / 2;
        int cmp = strcmp(SysB[mid].clients_name, name);
        if (cmp == 0) {
            first_found = mid; 
            right = mid - 1; 
        } else if (cmp < 0) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return first_found;
}

/**
 * Locates an invoice by its sequential ID.
 * @param SysB Array of registered invoices.
 * @param billID The invoice ID to search for.
 * @param pcBills Pointer to the total number of invoices.
 * @return The index of the invoice, or -1 if not found.
 */
long searchForBill(SysB *SysB, long billID, long *pcBills) {
    for (long i=0; i<*pcBills;i++) {
        if (SysB[i].id == billID) {
            return i;
        }
    }
    return -1;
}

/**
 * Parses deletion input to determine if an invoice or product is targeted.
 * @param input Raw input line from stdin.
 * @param arg1 Buffer to store the invoice ID or EAN.
 * @param qty Pointer to store the quantity (if a product is targeted).
 * @return 1 for invoice deletion, 2 for product deletion, 0 on failure.
 */
int processInput_D(char *input, char *arg1, int *qty) {
    int n = sscanf(input, "%*s %13s %d", arg1, qty);
    if (n == 0) return 0;
    if (n == 1) { *qty = -1; return 1; }
    return 2;
}

/**
 * Validates deletion requests for either invoices or products.
 * @param argsAmount The number of arguments provided (1 or 2).
 * @param arg1 The target invoice ID or EAN.
 * @param qty The target quantity (if applicable).
 * @param SysB Array of registered invoices.
 * @param SysP Array of registered system products.
 * @param pcBills Pointer to the total number of invoices.
 * @param pC Pointer to the total number of products.
 * @param user_b Array representing the current shopping basket.
 * @param pcB Pointer to the number of items in the basket.
 * @return The index of the target in its respective array, or -1 on error.
 */
long checkErrorsD(int argsAmount, char *arg1, int qty, SysB *SysB, 
                   SysP *SysP, long *pcBills, int *pC, basket *user_b, long *pcB) {
    char *error = NULL;
    if (argsAmount == 1) {
        long billID = atol(arg1), systemBillID = searchForBill(SysB, billID, pcBills);
        if (systemBillID == -1) { 
            printf("%ld: %s\n", billID, INVALID_INVOICE); 
            return -1; 
        }
        return systemBillID;
    }
    int ean_size = 0;
    int ean_digit = calculateEAN(arg1, &ean_size);
    if ((arg1[ean_size-1]-'0') != ean_digit) {
        puts(INVALID_EAN); 
        return -1;
    }
    int pid = searchProductID(arg1, SysP, pC);
    if (pid == -1) { 
        printf("%s: %s\n", arg1, NO_PRODUCT); 
        return -1; 
    } else if (qty <= 0 || qty > SysP[pid].Product.qty) error = INVALID_QUANTITY;
    long bid = searchProductBasketID(&SysP[pid].Product, user_b, pcB);
    if (bid != -1) error = PRODUCT_IN_USE;
    if(error!=NULL) {
        puts(error);
        return -1;
    }
    return pid;
}

/**
 * Deletes an invoice from the system and shifts the array memory.
 * @param SysB Array of registered invoices.
 * @param pcBills Pointer to the total number of invoices.
 * @param pbid Index of the invoice to delete.
 */
void removeBill(SysB *SysB, long *pcBills, long pbid) {
    free(SysB[pbid].clients_name);
    int numToMove = (*pcBills) - pbid - 1;
    if (numToMove > 0)
        memmove(&SysB[pbid], &SysB[pbid + 1], numToMove * sizeof(*SysB));
    (*pcBills)--;
}

/**
 * Deletes a product from the system and shifts the array memory.
 * @param SysP Array of registered system products.
 * @param pC Pointer to the total number of products.
 * @param pbid Index of the product to delete.
 */
void removeProduct(SysP *SysP, int *pC, long pbid) {
    int numToMove = (*pC) - pbid - 1;
    if (numToMove > 0)
        memmove(&SysP[pbid], &SysP[pbid + 1], numToMove * sizeof(*SysP));
    (*pC)--;
}