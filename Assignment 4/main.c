#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "schema.h"

extern FILE *yyin;

int main(int argc, char *argv[]) {
    char *input_file = NULL;
    char *out_dir = NULL;
    int should_print_ast = 0; // Renamed from print_ast

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--print-ast") == 0) {
            should_print_ast = 1;
        } else if (strcmp(argv[i], "--out-dir") == 0 && i + 1 < argc) {
            out_dir = argv[++i];
        } else {
            input_file = argv[i];
        }
    }

    if (!input_file) {
        fprintf(stderr, "Error: No input file provided\n");
        return 1;
    }

    yyin = fopen(input_file, "r");
    if (!yyin) {
        fprintf(stderr, "Error: Could not open input file %s\n", input_file);
        return 1;
    }

    if (yyparse() != 0) {
        fclose(yyin);
        return 1;
    }
    fclose(yyin);

    AstNode *root = get_root();
    if (should_print_ast) {
        print_ast(root, 0);
    }

    if (out_dir) {
        Table *tables = create_tables(root);
        write_csv(tables, out_dir);
        free_tables(tables);
    }

    free_ast(root);
    return 0;
}