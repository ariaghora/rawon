#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"
#include "objtable.h"

int hash(ObjTable *table, char *key) {
    int val = 0;
    int i = 0;

    while (val < ULONG_MAX && i < strlen(key)) {
        val = val << 8;
        val += key[i];
        i++;
    }
    return val % table->size;
}

ObjTable *create_obj_table(int size) {
    ObjTable *table = calloc(1, sizeof(ObjTable));
    table->size = size;
    table->entries = calloc(size, sizeof(TableEntry *));
    return table;
}

void free_obj_table(ObjTable *table) {
    for (int i = 0; i < table->size; ++i) {
        /* Only free pointer to i-th entry. The RwnObj
         * will be automatically freed.
         */
        free(table->entries[i]);
    }
    free(table->entries);
    free(table);
}