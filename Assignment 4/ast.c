#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static AstNode *root = NULL;

void set_root(AstNode *node) {
    root = node;
}

AstNode *get_root(void) {
    return root;
}

AstNode *create_object_node(AstNode *pairs) {
    AstNode *node = malloc(sizeof(AstNode));
    node->type = NODE_OBJECT;
    node->data.pair.value = pairs;
    return node;
}

AstNode *create_array_node(AstNode *values) {
    AstNode *node = malloc(sizeof(AstNode));
    node->type = NODE_ARRAY;
    node->data.array.value = values;
    return node;
}

AstNode *create_string_node(char *value) {
    AstNode *node = malloc(sizeof(AstNode));
    node->type = NODE_STRING;
    node->data.string = value;
    return node;
}

AstNode *create_number_node(double value) {
    AstNode *node = malloc(sizeof(AstNode));
    node->type = NODE_NUMBER;
    node->data.number = value;
    return node;
}

AstNode *create_bool_node(int value) {
    AstNode *node = malloc(sizeof(AstNode));
    node->type = NODE_BOOL;
    node->data.boolean = value;
    return node;
}

AstNode *create_null_node() {
    AstNode *node = malloc(sizeof(AstNode));
    node->type = NODE_NULL;
    return node;
}

AstNode *create_pair_node(char *key, AstNode *value) {
    AstNode *node = malloc(sizeof(AstNode));
    node->type = NODE_PAIR;
    node->data.pair.key = key;
    node->data.pair.value = value;
    node->data.pair.next = NULL;
    return node;
}

AstNode *append_pair(AstNode *pair, AstNode *next) {
    pair->data.pair.next = next;
    return pair;
}

AstNode *append_value(AstNode *value, AstNode *next) {
    value->data.array.next = next;
    return value;
}

void print_ast(AstNode *node, int indent) {
    if (!node) return;
    for (int i = 0; i < indent; i++) printf("  ");
    switch (node->type) {
        case NODE_OBJECT:
            printf("OBJECT\n");
            for (AstNode *p = node->data.pair.value; p; p = p->data.pair.next) {
                for (int i = 0; i < indent + 1; i++) printf("  ");
                printf("%s:\n", p->data.pair.key);
                print_ast(p->data.pair.value, indent + 2);
            }
            break;
        case NODE_ARRAY:
            printf("ARRAY\n");
            for (AstNode *v = node->data.array.value; v; v = v->data.array.next) {
                print_ast(v, indent + 1);
            }
            break;
        case NODE_STRING:
            printf("STRING: \"%s\"\n", node->data.string);
            break;
        case NODE_NUMBER:
            printf("NUMBER: %.2f\n", node->data.number);
            break;
        case NODE_BOOL:
            printf("BOOL: %s\n", node->data.boolean ? "true" : "false");
            break;
        case NODE_NULL:
            printf("NULL\n");
            break;
        case NODE_PAIR:
            printf("PAIR\n");
            break;
    }
}

void free_ast(AstNode *node) {
    if (!node) return;
    switch (node->type) {
        case NODE_OBJECT:
            free_ast(node->data.pair.value);
            break;
        case NODE_ARRAY:
            free_ast(node->data.array.value);
            break;
        case NODE_STRING:
            free(node->data.string);
            break;
        case NODE_PAIR:
            free(node->data.pair.key);
            free_ast(node->data.pair.value);
            free_ast(node->data.pair.next);
            break;
        default:
            break;
    }
    free(node);
}