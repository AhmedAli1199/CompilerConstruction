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
    struct row *next;
} Row;

typedef struct table {
    char *name;
    Column *columns;
    Row *rows;
    struct table *next;
} Table;

Table *create_tables(AstNode *node);
Table *handle_array(AstNode *array, const char *parent_name, int parent_id); /* Add prototype */
void write_csv(Table *table, const char *dir);
void free_tables(Table *table);

#endif