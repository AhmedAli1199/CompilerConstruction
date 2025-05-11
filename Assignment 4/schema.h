#ifndef SCHEMA_H
#define SCHEMA_H

#include "ast.h"

typedef struct column {
    char *name;
    struct column *next;
} Column;

typedef struct row {
    int id;
    char **values;
    int value_count; // Added to track values size
    struct row *next;
} Row;

typedef struct table {
    char *name;
    Column *columns;
    Row *rows;
    struct table *next;
} Table;

Table *create_table(const char *name, AstNode *node);
Table *create_tables(AstNode *node);
void write_csv(Table *table, const char *dir);
void free_tables(Table *table);
void free_all_tables(void); // Added

#endif