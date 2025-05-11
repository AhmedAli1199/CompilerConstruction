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

void free_all_tables() {
    TableList *tl = all_tables;
    while (tl) {
        TableList *next = tl->next;
        free(tl);
        tl = next;
    }
    all_tables = NULL;
}

// Helper: Add column if not exists
void add_column_if_missing(Table *table, const char *col_name) {
    for (Column *c = table->columns; c; c = c->next) {
        if (strcmp(c->name, col_name) == 0) return;
    }
    Column *col = malloc(sizeof(Column));
    col->name = strdup(col_name);
    col->next = NULL;
    // Append to end
    if (!table->columns) {
        table->columns = col;
    } else {
        Column *last = table->columns;
        while (last->next) last = last->next;
        last->next = col;
    }
}

// Helper: Collect all keys from all objects in array
void collect_columns_from_array(Table *table, AstNode *array) {
    for (AstNode *v = array->data.array.value; v; v = v->data.array.next) {
        if (v->type != NODE_OBJECT) continue;
        for (AstNode *p = v->data.pair.value; p; p = p->data.pair.next) {
            if (p->type == NODE_PAIR && p->data.pair.key && p->data.pair.value &&
                p->data.pair.value->type != NODE_ARRAY && p->data.pair.value->type != NODE_OBJECT) {
                add_column_if_missing(table, p->data.pair.key);
            }
        }
    }
}

// Create table for array of objects, collecting all possible columns
Table *create_table_for_array(const char *name, AstNode *array) {
    Table *table = malloc(sizeof(Table));
    table->name = strdup(name);
    table->columns = NULL;
    table->rows = NULL;
    table->next = NULL;

    // id column always first
    add_column_if_missing(table, "id");

    collect_columns_from_array(table, array);

    return table;
}

// Create table for a single object
Table *create_table_for_object(const char *name, AstNode *object) {
    Table *table = malloc(sizeof(Table));
    table->name = strdup(name);
    table->columns = NULL;
    table->rows = NULL;
    table->next = NULL;

    add_column_if_missing(table, "id");
    for (AstNode *p = object->data.pair.value; p; p = p->data.pair.next) {
        if (p->type == NODE_PAIR && p->data.pair.key && p->data.pair.value &&
            p->data.pair.value->type != NODE_ARRAY && p->data.pair.value->type != NODE_OBJECT) {
            add_column_if_missing(table, p->data.pair.key);
        }
        // For nested objects, add a column to store their id
        if (p->type == NODE_PAIR && p->data.pair.key && p->data.pair.value &&
            p->data.pair.value->type == NODE_OBJECT) {
            add_column_if_missing(table, p->data.pair.key);
        }
    }
    return table;
}

// Find column index by name
int column_index(Table *table, const char *col_name) {
    int idx = 0;
    for (Column *c = table->columns; c; c = c->next, idx++) {
        if (strcmp(c->name, col_name) == 0) return idx;
    }
    return -1;
}

// Fill row values for object
void fill_row_values(Table *table, Row *row, AstNode *object) {
    int col_count = 0;
    for (Column *c = table->columns; c; c = c->next) col_count++;
    row->values = calloc(col_count, sizeof(char *));
    row->value_count = col_count;

    // id
    int idx = column_index(table, "id");
    if (idx >= 0) {
        row->values[idx] = malloc(32);
        snprintf(row->values[idx], 32, "%d", row->id);
    }
    // other columns
    for (AstNode *p = object->data.pair.value; p; p = p->data.pair.next) {
        if (p->type != NODE_PAIR || !p->data.pair.key || !p->data.pair.value) continue;
        if (p->data.pair.value->type == NODE_ARRAY || p->data.pair.value->type == NODE_OBJECT) continue;
        idx = column_index(table, p->data.pair.key);
        if (idx >= 0) {
            if (p->data.pair.value->type == NODE_STRING) {
                row->values[idx] = strdup(p->data.pair.value->data.string);
            } else if (p->data.pair.value->type == NODE_NUMBER) {
                row->values[idx] = malloc(32);
                snprintf(row->values[idx], 32, "%.0f", p->data.pair.value->data.number);
            } else if (p->data.pair.value->type == NODE_BOOL) {
                row->values[idx] = strdup(p->data.pair.value->data.boolean ? "true" : "false");
            } else if (p->data.pair.value->type == NODE_NULL) {
                row->values[idx] = strdup("");
            }
        }
    }
}

// Recursively create tables for AST
Table *create_tables_recursive(AstNode *node, const char *name, const char *parent_name, int parent_id) {
    if (!node) return NULL;
    Table *main_table = NULL, *last_table = NULL;

    if (node->type == NODE_OBJECT) {
        main_table = create_table_for_object(name, node);
        add_table(main_table);

        Row *row = malloc(sizeof(Row));
        row->id = id_counter++;
        fill_row_values(main_table, row, node);

        // For each pair, if value is an object, create child table and store its id in parent row
        for (AstNode *p = node->data.pair.value; p; p = p->data.pair.next) {
            if (p->type != NODE_PAIR || !p->data.pair.key || !p->data.pair.value) continue;
            if (p->data.pair.value->type == NODE_OBJECT) {
                Table *child = create_tables_recursive(p->data.pair.value, p->data.pair.key, name, row->id);
                if (child && child->rows) {
                    // Store the id of the nested object in the parent row
                    int idx = column_index(main_table, p->data.pair.key);
                    if (idx >= 0) {
                        char id_buf[32];
                        snprintf(id_buf, sizeof(id_buf), "%d", child->rows->id);
                        if (row->values[idx]) free(row->values[idx]);
                        row->values[idx] = strdup(id_buf);
                    }
                }
                // Chain child tables
                if (child) {
                    if (!last_table) main_table->next = child;
                    else last_table->next = child;
                    while (child->next) child = child->next;
                    last_table = child;
                }
            } else if (p->data.pair.value->type == NODE_ARRAY) {
                Table *child = create_tables_recursive(p->data.pair.value, p->data.pair.key, name, row->id);
                if (child) {
                    if (!last_table) main_table->next = child;
                    else last_table->next = child;
                    while (child->next) child = child->next;
                    last_table = child;
                }
            }
        }

        row->next = main_table->rows;
        main_table->rows = row;
        return main_table;
    } else if (node->type == NODE_ARRAY) {
        AstNode *first = node->data.array.value;
        if (!first) return NULL;
        int is_obj_array = (first->type == NODE_OBJECT);

        if (is_obj_array) {
            Table *table = create_table_for_array(name, node);
            add_table(table);
            for (AstNode *v = node->data.array.value; v; v = v->data.array.next) {
                Row *row = malloc(sizeof(Row));
                row->id = id_counter++;
                fill_row_values(table, row, v);
                row->next = table->rows;
                table->rows = row;

                // Recursively handle nested arrays/objects
                for (AstNode *p = v->data.pair.value; p; p = p->data.pair.next) {
                    if (p->type != NODE_PAIR || !p->data.pair.key || !p->data.pair.value) continue;
                    if (p->data.pair.value->type == NODE_ARRAY || p->data.pair.value->type == NODE_OBJECT) {
                        Table *child = create_tables_recursive(p->data.pair.value, p->data.pair.key, name, row->id);
                        if (child && p->data.pair.value->type == NODE_OBJECT && child->rows) {
                            // Store the id of the nested object in the parent row
                            int idx = column_index(table, p->data.pair.key);
                            if (idx >= 0) {
                                char id_buf[32];
                                snprintf(id_buf, sizeof(id_buf), "%d", child->rows->id);
                                if (row->values[idx]) free(row->values[idx]);
                                row->values[idx] = strdup(id_buf);
                            }
                        }
                        if (child) {
                            if (!last_table) table->next = child;
                            else last_table->next = child;
                            while (child->next) child = child->next;
                            last_table = child;
                        }
                    }
                }
            }
            return table;
        } else {
            // Array of primitives: <parent>_id, index, value
            Table *table = malloc(sizeof(Table));
            table->name = strdup(name);
            table->columns = NULL;
            table->rows = NULL;
            table->next = NULL;

            // Add parent id column (e.g., movie_id), index, value
            char fk_col[128];
            if (parent_name && strlen(parent_name) > 0) {
                snprintf(fk_col, sizeof(fk_col), "%s_id", parent_name);
            } else {
                snprintf(fk_col, sizeof(fk_col), "parent_id");
            }
            add_column_if_missing(table, fk_col);
            add_column_if_missing(table, "index");
            add_column_if_missing(table, "value");

            add_table(table);

            int idx = 0;
            for (AstNode *v = node->data.array.value; v; v = v->data.array.next, idx++) {
                Row *row = malloc(sizeof(Row));
                row->id = 0; // not used
                int col_count = 0;
                for (Column *c = table->columns; c; c = c->next) col_count++;
                row->values = calloc(col_count, sizeof(char *));
                row->value_count = col_count;

                int fk_idx = column_index(table, fk_col);
                int index_idx = column_index(table, "index");
                int value_idx = column_index(table, "value");

                if (fk_idx >= 0) {
                    row->values[fk_idx] = malloc(32);
                    snprintf(row->values[fk_idx], 32, "%d", parent_id);
                }
                if (index_idx >= 0) {
                    row->values[index_idx] = malloc(32);
                    snprintf(row->values[index_idx], 32, "%d", idx);
                }
                if (value_idx >= 0) {
                    if (v->type == NODE_STRING) row->values[value_idx] = strdup(v->data.string);
                    else if (v->type == NODE_NUMBER) {
                        row->values[value_idx] = malloc(32);
                        snprintf(row->values[value_idx], 32, "%.0f", v->data.number);
                    } else if (v->type == NODE_BOOL) {
                        row->values[value_idx] = strdup(v->data.boolean ? "true" : "false");
                    }
                }
                row->next = table->rows;
                table->rows = row;
            }
            return table;
        }
    }
    return NULL;
}

Table *create_tables(AstNode *node) {
    return create_tables_recursive(node, "table_name", NULL, 0);
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
            for (int i = 0; i < col_count; i++) {
                if (i > 0) fprintf(fp, ",");
                fprintf(fp, "%s", r->values[i] ? r->values[i] : "");
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
            int value_count = r->value_count;
            for (int i = 0; i < value_count; i++) {
                if (r->values[i]) {
                    free(r->values[i]);
                    r->values[i] = NULL;
                }
            }
            free(r->values);
            free(r);
            r = next_r;
        }
        free(table->name);
        free(table);
        table = next;
    }
}