#ifndef AST_H
#define AST_H

typedef enum {
    NODE_OBJECT,
    NODE_ARRAY,
    NODE_STRING,
    NODE_NUMBER,
    NODE_BOOL,
    NODE_NULL,
    NODE_PAIR
} NodeType;

typedef struct ast_node {
    NodeType type;
    union {
        struct {
            char *key;              // Added for pair key
            struct ast_node *value; // Pair value
            struct ast_node *next;  // Next pair in object
        } pair;
        struct {
            struct ast_node *value; // Array value
            struct ast_node *next;  // Next value in array
        } array;
        char *string;
        double number;
        int boolean;
    } data;
} AstNode;

AstNode *create_string_node(const char *value);
AstNode *create_number_node(double value);
AstNode *create_bool_node(int value);
AstNode *create_null_node(void);
AstNode *create_object_node(AstNode *pairs);
AstNode *create_array_node(AstNode *values);
AstNode *create_pair_node(const char *key, AstNode *value);
AstNode *append_pair(AstNode *pair, AstNode *pairs);
AstNode *append_value(AstNode *value, AstNode *values);
void set_root(AstNode *node);
void print_root(void);
void free_root(void);
AstNode *get_root(void);

#endif