int process_args_p(char *user_input, prod *proc_prod) {
    user_input[strcspn(user_input, "\r\n")] = '\0';    
    char temp_desc[MAXBUFF];
    sscanf(user_input, "%*s %s %c %f %d %65535[^\n]",
           proc_prod->EAN,
           &proc_prod->IVA,
           &proc_prod->price,
           &proc_prod->stock,
           temp_desc);
    proc_prod->EAN[EAN_DIGITS - 1] = '\0';
    if (strlen(temp_desc) >= description_max_char) {
        puts(INVALID_DESCRIPTION);
        return 0;
    }
    strncpy(proc_prod->description, temp_desc, description_max_char - 1);
    proc_prod->description[description_max_char - 1] = '\0';    
    return 1;
}

int calc_ean_digit(prod *validating_prod, int *ean_size) {
    int i, sum=0;
    char *pEAN = validating_prod->EAN;
    *ean_size = strlen(pEAN);
    for (i=0;i<*ean_size-1;i++) {
        int digit =  pEAN[i] - '0';
        if (i%2==0) sum += digit;
        else sum += (digit*3);
    }
    int verification_digit = (10 - (sum % 10)) % 10;
    return verification_digit;
}

void add_IVA_through_files(IVA *IStr, int *counter, const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) return;
    while (fscanf(fp, " %c %d", 
        &IStr[*counter].IVA_LETTER,
        &IStr[*counter].IVA_PERCENTAGE) == 2) {
        (*counter)++;
    };
    fclose(fp);
}

int initialize_IVA(IVA *IVA_STRUCT) {
    int counter=0;
    IVA_STRUCT[0].IVA_LETTER='A'; IVA_STRUCT[0].IVA_PERCENTAGE=0;
    IVA_STRUCT[1].IVA_LETTER='B'; IVA_STRUCT[1].IVA_PERCENTAGE=6;
    IVA_STRUCT[2].IVA_LETTER='C'; IVA_STRUCT[2].IVA_PERCENTAGE=13;
    IVA_STRUCT[3].IVA_LETTER='D'; IVA_STRUCT[3].IVA_PERCENTAGE=23;
    counter = 4;
    return counter;
}

int detect_product_id(prod *prod, SysP *Products_List, int *counter) {
    int i;
    if (*counter != 0) {
        for (i=0; i<*counter; i++) {
            if (strcmp(Products_List[i].Product.EAN, prod->EAN) == 0) {
                return i;
            }
        }
    } 
    return -1;
}

int checkForIVA(prod *product, IVA *IVA_STRUCT, int iva_amount) {
    int i=0;
    for (i=0; i< iva_amount; i++) {
        if(product->IVA == IVA_STRUCT[i].IVA_LETTER) { 
            return i; 
        }
    }
    return -1;
}

int check_basket_for_product(prod *prod, basket *bask, int *pcB) {
    int i;
    if (*pcB>0) {
        for(i=0;i<*pcB;i++) {
            if (strcmp(prod->EAN, bask[i].EAN)==0) {
                return i;
            } 
        }
    }
    return -1;
}

int err_check(prod *prod, SysP *SysP, int *c, int b_id, int iva_id, int pID) {
    int ean_size=0;
    int *pean_s = &ean_size;
    int ean_digit = calc_ean_digit(prod, pean_s);
    char *error = NULL;
    if (*c >= MAXPRODUCTS) error = INVALID_PRODUCT; 
    else if (prod->price <= 0) error= INVALID_PRICE; 
    else if (prod->stock < 0) error = INVALID_QUANTITY; 
    else if (iva_id == -1) error = INVALID_IVA;
    else if ((prod->EAN[ean_size-1]-'0' ) != ean_digit) error = INVALID_EAN; 
    else if(!isupper((unsigned char)prod->description[0])) {
        error = INVALID_DESCRIPTION;
    }
    else if((b_id != -1)&&(pID != -1)&&(prod->price != SysP[pID].Product.price)) 
        error = PRODUCT_IN_USE;
    if (error != NULL) {
        puts(error);
        return 0;
    }
    return 1;
}


int wildcard_match(const char *pattern, const char *str) {
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

void print_product(SysP *SysP, int i) {
    printf("%s %c %.2f %d %d %s\n",
        SysP[i].Product.EAN,
        SysP[i].Product.IVA,
        SysP[i].Product.price,
        SysP[i].sold,
        SysP[i].Product.stock,
        SysP[i].Product.description);
}

int list_by_wildcard(SysP *SysP, int *c, char *pattern) {
    int i, found = 0;
    for (i = 0; i < *c; i++) {
        if (SysP[i].Product.stock > 0 &&
            wildcard_match(pattern, SysP[i].Product.EAN)) {
            print_product(SysP, i);
            found = 1;
        }
    }
    if (!found) printf("%s: no such product\n", pattern);
    return found;
}


void process_args_a(char *user_input, basket *user_b, int *pcB) {

}

int a_err_check() {
    return 0;
}