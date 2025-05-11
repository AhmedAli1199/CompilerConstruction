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
        struct ast_node *value;
        char *string;
        double number;
        int boolean;
        struct {
            char *key;
            struct ast_node *value;
            struct ast_node *next;
        } pair;
        struct {
            struct ast_node *value;
            struct ast_node *next;
        } array;
    } data;
} AstNode;

AstNode *create_object_node(AstNode *pairs);
AstNode *create_array_node(AstNode *values);
AstNode *create_string_node(char *value);
AstNode *create_number_node(double value);
AstNode *create_bool_node(int value);
AstNode *create_null_node();
AstNode *create_pair_node(char *key, AstNode *value);
AstNode *append_pair(AstNode *pair, AstNode *next);
AstNode *append_value(AstNode *value, AstNode *next);
void set_root(AstNode *node);
AstNode *get_root(void);
void print_ast(AstNode *node, int indent);
void free_ast(AstNode *node);

int yyparse(void);
void yyerror(const char *msg);

#endif