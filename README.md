# Logistics Billing Engine

Command-line invoicing and inventory management system developed in C as a university project.

**Grade:** 17.8 / 20

## Features

* Product and inventory management
* Transaction and invoice tracking
* EAN validation
* In-memory data structures
* Custom memory management

The system supports **10,000+ product records** and was tested with Valgrind to achieve **0 errors and 0 memory leaks**.

## Technologies

* C
* Make
* Valgrind

## Running

```bash
gcc -O3 -Wall -Wextra -Werror -Wno-unused-result -o proj *.c
make timed
```
