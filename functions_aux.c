product process_args(char *user_input, product *product_processed) {
    int i=0;
    user_input[strcspn(user_input, "\r\n")] = '\0';
    char *token = strtok(user_input, " ");
    while (token != NULL) {
        switch (i) {
        case 0: break;
        case 1: strncpy(product_processed->EAN, token, EAN_DIGITS - 1); product_processed->EAN[EAN_DIGITS - 1] = '\0'; break; 
        case 2: product_processed->IVA = token[0]; break;
        case 3: product_processed->price = (float)atof(token); break;
        case 4: product_processed->stock = atoi(token); break;
        case 5 :
            strncpy(product_processed->description, token, MAXBUFF - 1);
            product_processed->description[MAXBUFF - 1] = '\0';
            char *left = strtok(NULL, ""); 
            if (left) {
                strcat(product_processed->description, " ");
                int espaco_restante = (MAXBUFF - 1) - strlen(product_processed->description);
                if (espaco_restante > 0) strncat(product_processed->description, left, espaco_restante);
            } break;
        }
        i++;
        token = strtok(NULL, " ");
    }
    return *product_processed;
}

int calc_ean_digit(product *validating_prod) {
    int i, sum=0;
    char *pEAN = validating_prod->EAN;
    int size_ean = strlen(pEAN);
    for (i=0;i<size_ean-1;i++) {
        int digit =  pEAN[i] - '0';
        if (i%2==0) sum += digit;
        else sum += (digit*3);
    }
    int verification_digit = (10 - (sum % 10)) % 10;
    return verification_digit;
}

int validate_errors(product *validating_prod) {
    int ean_digit = calc_ean_digit(validating_prod);
    int ean_size = strlen(validating_prod->EAN);
    char *IVA_POSSIBILITIES = "ABCD";
    if ((validating_prod->EAN[ean_size-1]-'0') != ean_digit) puts(INVALID_EAN);
    if (strpbrk(&validating_prod->IVA, IVA_POSSIBILITIES) == NULL) puts(INVALID_IVA);
    if (validating_prod->price < 0) puts(INVALID_PRICE);
    if (validating_prod->stock < 0) puts(INVALID_QUANTITY);
    if (strlen(validating_prod->description) > description_max_char) puts(INVALID_DESCRIPTION);
    return 1;
}