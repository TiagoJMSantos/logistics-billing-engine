/**
 * @file main_data_struct.h
 * @brief Core data structures and constants for the billing system.
 * @author ist1117516 (Tiago Santos)
 */
#ifndef MAIN_DATA_STRUCT_H
#define MAIN_DATA_STRUCT_H

#define EAN_DIGITS      14      /**< max EAN string length + null	*/
#define desc_max_char   51      /**< max product description length	*/
#define MAXPRODUCTS     10000   /**< max number of registered products	*/
#define NIF_LENGTH      10      /**< NIF string length + null		*/
#define MAXBUFF         65536   /**< max input line length		*/

#define EINVALID        "Comando Invalido"      /**< invalid command		*/
#define INVALID_EAN     "invalid ean"           /**< bad EAN check digit	*/
#define INVALID_IVA     "invalid iva"           /**< unknown IVA code		*/
#define INVALID_PRICE   "invalid price"         /**< non-positive price		*/
#define INVALID_DESCRIPTION "invalid description" /**< description too long	*/
#define PRODUCT_IN_USE  "product in use"        /**< price change on basket item */
#define INVALID_PRODUCT "invalid product"       /**< product limit exceeded	*/
#define INVALID_QUANTITY "invalid quantity"     /**< negative or excess quantity */
#define NO_STOCK        "no stock"              /**< product out of stock	*/
#define INVALID_NAME    "invalid name"          /**< invalid client name	*/
#define INVALID_NIF     "no such nif"           /**< invalid NIF number		*/
#define NO_PRODUCT      "no such product"       /**< product not found		*/
#define NO_CLIENT       "no such client"        /**< client not found		*/
#define INVALID_INVOICE "no such invoice"       /**< invoice not found		*/

/** Product registered in the system. */
typedef struct {
	char desc[desc_max_char];	/**< product description	*/
	char EAN[EAN_DIGITS];		/**< EAN-8 or EAN-13 code	*/
	double price;			/**< unit price			*/
	char IVA;			/**< IVA class letter		*/
	int qty;			/**< available stock		*/
} prod;

/** IVA rate entry. */
typedef struct {
	char IVA_LETTER;		/**< IVA class identifier	*/
	int IVA_PERCENTAGE;		/**< TAX percentage		*/
} IVA;

/** Shopping basket entry. */
typedef struct {
	prod Product;			/**< product and quantity info	*/
} basket;

/** System product entry with sales tracking. */
typedef struct {
	prod Product;			/**< product information	*/
	int sold;			/**< total units sold		*/
} SysP;

/** Invoice stored in the billing system. */
typedef struct {
	int qty;			/**< number of distinct products */
	long total_qty;			/**< total number of items	*/
	double value;			/**< total invoice value	*/
	char NIF[NIF_LENGTH];		/**< client NIF number		*/
	char *clients_name;		/**< dynamically allocated name	*/
	long id;			/**< sequential invoice number	*/
} SysB;

#endif