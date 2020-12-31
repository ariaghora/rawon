//
// Created by hello on 12/31/2020.
//

#ifndef CRAWON_OBJTABLE_H
#define CRAWON_OBJTABLE_H

typedef struct {
    char *key;
    RwnObj *value;
} TableEntry;

typedef struct {
    int size;
    TableEntry **entries;
} ObjTable;

ObjTable *create_obj_table(int size);

int hash(ObjTable *table, char *key);

void free_obj_table(ObjTable *table);

#endif //CRAWON_OBJTABLE_H
