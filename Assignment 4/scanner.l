%{
#include "parser.h"
#include <stdlib.h>
#include <string.h>

int line = 1, column = 1;
%}

%option noyywrap
%option nounput noinput

%%

"{"         { column += strlen(yytext); return LBRACE; }
"}"         { column += strlen(yytext); return RBRACE; }
"["         { column += strlen(yytext); return LBRACK; }
"]"         { column += strlen(yytext); return RBRACK; }
":"         { column += strlen(yytext); return COLON; }
","         { column += strlen(yytext); return COMMA; }
"true"      { column += strlen(yytext); return TRUE; }
"false"     { column += strlen(yytext); return FALSE; }
"null"      { column += strlen(yytext); return NULL_TOKEN; }

\"([^\\\"]|\\.)*\"  {
    yylval.str = strdup(yytext + 1);
    yylval.str[strlen(yylval.str) - 1] = '\0';
    column += strlen(yytext);
    return STRING;
}

-?[0-9]+(\.[0-9]+)? {
    yylval.num = atof(yytext);
    column += strlen(yytext);
    return NUMBER;
}

[ \t]       { column += strlen(yytext); }
\n          { line++; column = 1; }
.           { column += strlen(yytext); /* Ignore invalid characters */ }

%%