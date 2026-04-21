/**
 * @file aux_functions.h
 * @brief Declarations of auxiliary functions for the billing system.
 * @author ist1117516 (Tiago Santos)
 */

#ifndef AUX_FUNCTIONS_H
#define AUX_FUNCTIONS_H
#include "main_data_struct.h"
#include "memory.h"
int processInput_P(char *user_input, prod *proc_prod);
int calculateEAN(char *EAN, int *ean_size);
void createIVATableFile(IVA *IStr, int *counter, const char *filename);
int createIVATable(IVA *IVA_STRUCT, int argc);
int searchProductID(char *EAN, SysP *Products_List, int *counter);
int searchForIVA(prod *product, IVA *IVA_STRUCT, int iva_amount);
long searchProductBasketID(prod *prod, basket *bask, long *pcB);
int checkErrorsP(prod *prod, SysP *SysP, int *c, long b_id, int iva_id, int pID);
int wildcardMatch(const char *pattern, const char *str);
void printProduct(SysP *SysP, int i);
void printBasket(basket *user_b, double totalIVA, int i);
int listByWildcard(SysP *SysP, int *c, char *pattern);
int processInput_A(char *user_input, char *out_ean, int *out_qty);
int checkErrorsA(char *ean, int qty, SysP *SysP, int *c, basket *user_b, long *pcB, long *bid);
long calculateTotalCents(double price, int qty, int ivaPercent);
long roundToCents(double value);
void updateProductInBasket(basket *user_b, int bid, SysP *SysP, int pid, int qty);
void insertProductInBasket(basket **user_b, long *pcB, SysP *SysP, int pid, int qty, int *max_b);
void processInput_F(char *input, char *nif, char **name);
int checkErrorsF(char *nif, char *name);
void cancelBasket(long *pcB, basket *user_b, SysP *SysP, int *pC);
void insertBill(SysB **ptr_SysB, long *pcBills, char *nif, char *name, long *pcB, double value, long total_qty);
int processInput_R(char *input, char *ean);
int checkErrorsR(char *EAN, SysP *SysP, int *pC);
long getSoldProd(SysB *SysB, long *pcBills);
double getBillsTotal(SysB *SysB, long *pcBills);
int processInput_C(char *input, char **name);
int checkErrorsC(char *name);
long searchName(SysB *SysB, char *name, long *pcBills);
long searchForBill(SysB *SysB, long billID, long *pcBills);
int processInput_D(char *input, char *arg1, int *qty);
long checkErrorsD(int argsAmount, char *arg1, int qty, SysB *SysB, SysP *SysP, long *pcBills, int *pC, basket *user_b, long *pcB);
void removeBill(SysB *SysB, long *pcBills, long pbid);
void removeProduct(SysP *SysP, int *pC, long pbid);
#endif