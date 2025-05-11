%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

extern int line, column;
extern void yyerror(const char *msg);
extern int yylex(void); /* Declare yylex */
%}

%union {
    char *str;
    double num;
    struct ast_node *node;
}

%token LBRACE RBRACE LBRACK RBRACK COLON COMMA
%token <str> STRING
%token <num> NUMBER
%token TRUE FALSE NULL_TOKEN

%type <node> value object array pair pairs values

%%

json: value { set_root($1); } ;

value: object
     | array
     | STRING    { $$ = create_string_node($1); free($1); }
     | NUMBER    { $$ = create_number_node($1); }
     | TRUE      { $$ = create_bool_node(1); }
     | FALSE     { $$ = create_bool_node(0); }
     | NULL_TOKEN { $$ = create_null_node(); }
     ;

object: LBRACE pairs RBRACE { $$ = create_object_node($2); }
      | LBRACE RBRACE       { $$ = create_object_node(NULL); }
      ;

pairs: pair             { $$ = $1; }
     | pair COMMA pairs { $$ = append_pair($1, $3); }
     ;

pair: STRING COLON value { $$ = create_pair_node($1, $3); }
    ;

array: LBRACK values RBRACK { $$ = create_array_node($2); }
     | LBRACK RBRACK        { $$ = create_array_node(NULL); }
     ;

values: value              { $$ = $1; }
      | value COMMA values { $$ = append_value($1, $3); }
      ;

%%

void yyerror(const char *msg) {
    fprintf(stderr, "Error: %s at line %d, column %d\n", msg, line, column);
    exit(1);
}