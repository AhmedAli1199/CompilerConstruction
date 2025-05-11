#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "schema.h"

extern int yyparse(void); // Add declaration

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <json_file> [--print-ast] [--out-dir <dir>]\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    char *out_dir = ".";
    int print_ast = 0;

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--print-ast") == 0) {
            print_ast = 1;
        } else if (strcmp(argv[i], "--out-dir") == 0 && i + 1 < argc) {
            out_dir = argv[++i];
        }
    }

    extern FILE *yyin;
    yyin = fopen(filename, "r");
    if (!yyin) {
        fprintf(stderr, "Error opening %s\n", filename);
        return 1;
    }

    if (yyparse() != 0) {
        fclose(yyin);
        return 1;
    }
    fclose(yyin);

    if (print_ast) {
        print_root();
    }

    Table *tables = create_tables(get_root());
    write_csv(tables, out_dir);
    free_tables(tables);
    free_all_tables();
    free_root();

    return 0;
}