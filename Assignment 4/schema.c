#include "schema.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int id_counter = 1;

typedef struct table_list {
    Table *table;
    struct table_list *next;
} TableList;

static TableList *all_tables = NULL;

void add_table(Table *table) {
    TableList *new_table = malloc(sizeof(TableList));
    new_table->table = table;
    new_table->next = all_tables;
    all_tables = new_table;
}

Table *create_table(const char *name, AstNode *node) {
    Table *table = malloc(sizeof(Table));
    table->name = strdup(name);
    table->columns = NULL;
    table->rows = NULL;
    table->next = NULL;

    Column *id_col = malloc(sizeof(Column));
    id_col->name = strdup("id");
    id_col->next = NULL;
    table->columns = id_col;

    for (AstNode *p = node->data.pair.value; p; p = p->data.pair.next) {
        Column *col = malloc(sizeof(Column));
        col->name = strdup(p->data.pair.key);
        col->next = table->columns;
        table->columns = col;
    }

    return table;
}

Table *handle_array(AstNode *array, const char *parent_name, int parent_id) {
    if (!array || array->type != NODE_ARRAY) return NULL;

    AstNode *first = array->data.array.value;
    if (first && first->type == NODE_OBJECT) {
        // Array of objects (test3.json)
        Table *table = create_table("items", first);
        add_table(table);
        int seq = 0;
        for (AstNode *v = array->data.array.value; v; v = v->data.array.next) {
            Table *child = create_tables(v);
            Row *row = child->rows;
            row->values[0] = malloc(32);
            snprintf(row->values[0], 32, "%d", parent_id);
            row->values[1] = malloc(32);
            snprintf(row->values[1], 32, "%d", seq++);
        }
        return table;
    } else {
        // Array of scalars (test2.json)
        Table *table = malloc(sizeof(Table));
        table->name = strdup("junction");
        table->columns = NULL;
        Column *c1 = malloc(sizeof(Column));
        c1->name = strdup("parent_id");
        c1->next = NULL;
        Column *c2 = malloc(sizeof(Column));
        c2->name = strdup("index");
        c2->next = c1;
        Column *c3 = malloc(sizeof(Column));
        c3->name = strdup("value");
        c3->next = c2;
        table->columns = c3;
        table->rows = NULL;

        int index = 0;
        for (AstNode *v = array->data.array.value; v; v = v->data.array.next) {
            Row *row = malloc(sizeof(Row));
            row->id = id_counter++;
            row->values = malloc(3 * sizeof(char *));
            row->values[0] = malloc(32);
            snprintf(row->values[0], 32, "%d", parent_id);
            row->values[1] = malloc(32);
            snprintf(row->values[1], 32, "%d", index++);
            row->values[2] = v->type == NODE_STRING ? strdup(v->data.string) :
                            v->type == NODE_NUMBER ? (char *)malloc(32) : NULL;
            if (v->type == NODE_NUMBER) snprintf(row->values[2], 32, "%.0f", v->data.number);
            row->next = table->rows;
            table->rows = row;
        }
        add_table(table);
        return table;
    }
}

Table *create_tables(AstNode *node) {
    if (!node) return NULL;

    if (node->type == NODE_OBJECT) {
        char *table_name = "table_name"; // TODO: Derive dynamically
        Table *table = NULL;

        // Create new table (shape detection to be added later)
        table = create_table(table_name, node);
        add_table(table);

        Row *row = malloc(sizeof(Row));
        row->id = id_counter++;
        row->values = malloc(10 * sizeof(char *));
        for (int i = 0; i < 10; i++) row->values[i] = NULL;
        row->next = table->rows;
        table->rows = row;

        int col_idx = 0;
        for (AstNode *p = node->data.pair.value; p; p = p->data.pair.next) {
            AstNode *val = p->data.pair.value;
            if (val->type == NODE_OBJECT) {
                Table *child = create_tables(val);
                char *fk_val = malloc(32);
                snprintf(fk_val, 32, "%d", child->rows->id);
                row->values[col_idx++] = fk_val;
                table->next = child;
            } else if (val->type == NODE_ARRAY) {
                Table *child = handle_array(val, table->name, row->id);
                if (child) table->next = child;
            } else {
                char *str_val = NULL;
                if (val->type == NODE_STRING) str_val = strdup(val->data.string);
                else if (val->type == NODE_NUMBER) {
                    str_val = malloc(32);
                    snprintf(str_val, 32, "%.0f", val->data.number);
                } else if (val->type == NODE_BOOL) {
                    str_val = strdup(val->data.boolean ? "true" : "false");
                }
                row->values[col_idx++] = str_val;
            }
        }

        return table;
    }
    return NULL;
}

void write_csv(Table *table, const char *dir) {
    for (; table; table = table->next) {
        char path[256];
        snprintf(path, sizeof(path), "%s/%s.csv", dir, table->name);
        FILE *fp = fopen(path, "w");
        if (!fp) {
            fprintf(stderr, "Error opening %s\n", path);
            exit(1);
        }

        int col_count = 0;
        for (Column *c = table->columns; c; c = c->next) {
            if (col_count > 0) fprintf(fp, ",");
            fprintf(fp, "%s", c->name);
            col_count++;
        }
        fprintf(fp, "\n");

        for (Row *r = table->rows; r; r = r->next) {
            fprintf(fp, "%d", r->id);
            for (int i = 0; i < col_count - 1; i++) {
                fprintf(fp, ",%s", r->values[i] ? r->values[i] : "");
            }
            fprintf(fp, "\n");
        }
        fclose(fp);
    }
}

void free_tables(Table *table) {
    while (table) {
        Table *next = table->next;
        Column *c = table->columns;
        while (c) {
            Column *next_c = c->next;
            free(c->name);
            free(c);
            c = next_c;
        }
        Row *r = table->rows;
        while (r) {
            Row *next_r = r->next;
            for (int i = 0; r->values[i]; i++) free(r->values[i]);
            free(r->values);
            free(r);
            r = next_r;
        }
        free(table->name);
        free(table);
        table = next;
    }
}