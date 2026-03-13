#define EAN_DIGITS 14
#define description_max_char 51
#define MAXPRODUCTS 10001
#define NIF_LENGTH 10
#define MAXBUFF 65536
#define EINVALID "Comando Invalido"
#define INVALID_EAN "invalid ean"
#define INVALID_IVA "invalid iva"
#define INVALID_PRICE "invalid price"
#define INVALID_DESCRIPTION "invalid description"
#define PRODUCT_IN_USE "product in use"
#define INVALID_PRODUCT "invalid product"
#define INVALID_QUANTITY "invalid quantity"

typedef struct {
    char description[51];
    char EAN[EAN_DIGITS];
    float price;
    char IVA;
    int stock;
} prod;

typedef struct {
    prod bought_product;
    short total_value;
    short NIF[NIF_LENGTH];
    char clients_name;
} bill;

typedef struct {
    char IVA_LETTER;
    int IVA_PERCENTAGE;
} IVA;

typedef struct {
    short quantity;
    char EAN[EAN_DIGITS];
} basket;

typedef struct {
    prod Product;
    int sold;
} SysP;

typedef struct {
    bill Bills;
} System_Bills;