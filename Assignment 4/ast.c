#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

AstNode *create_string_node(const char *value) {
    AstNode *node = malloc(sizeof(AstNode));
    node->type = NODE_STRING;
    node->data.string = strdup(value);
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

AstNode *create_null_node(void) {
    AstNode *node = malloc(sizeof(AstNode));
    node->type = NODE_NULL;
    return node;
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
    node->data.array.next = NULL;
    return node;
}

AstNode *create_pair_node(const char *key, AstNode *value) {
    AstNode *node = malloc(sizeof(AstNode));
    node->type = NODE_PAIR;
    node->data.pair.key = strdup(key);
    node->data.pair.value = value;
    node->data.pair.next = NULL;
    return node;
}

AstNode *append_pair(AstNode *pair, AstNode *pairs) {
    pair->data.pair.next = pairs;
    return pair;
}

AstNode *append_value(AstNode *value, AstNode *values) {
    AstNode *node = malloc(sizeof(AstNode));
    node->type = value->type;
    node->data = value->data;
    node->data.array.next = values;
    free(value);
    return node;
}

static AstNode *root = NULL;

void set_root(AstNode *node) {
    root = node;
}

AstNode *get_root(void) {
    return root;
}

void print_ast(AstNode *node, int indent) {
    if (!node) return;
    for (int i = 0; i < indent; i++) printf("  ");
    switch (node->type) {
        case NODE_OBJECT:
            printf("OBJECT\n");
            print_ast(node->data.pair.value, indent + 1);
            break;
        case NODE_ARRAY:
            printf("ARRAY\n");
            print_ast(node->data.array.value, indent + 1);
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
            printf("%s:\n", node->data.pair.key);
            print_ast(node->data.pair.value, indent + 1);
            print_ast(node->data.pair.next, indent);
            break;
    }
}

void print_root(void) {
    print_ast(root, 0);
}

void free_ast(AstNode *node) {
    if (!node) return;
    switch (node->type) {
        case NODE_OBJECT:
            free_ast(node->data.pair.value);
            break;
        case NODE_ARRAY: {
            AstNode *value = node->data.array.value;
            while (value) {
                AstNode *next = value->data.array.next;
                free_ast(value);
                value = next;
            }
            break;
        }
        case NODE_STRING:
            if (node->data.string) free(node->data.string);
            break;
        case NODE_PAIR:
            if (node->data.pair.key) free(node->data.pair.key);
            free_ast(node->data.pair.value);
            free_ast(node->data.pair.next);
            break;
        default:
            break;
    }
    free(node);
}

void free_root(void) {
    free_ast(root);
    root = NULL;
}