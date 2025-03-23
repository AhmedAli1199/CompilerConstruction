#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 256
#define MAX_SYMBOLS 100
#define MAX_PROD 10
#define EPSILON "#"

typedef struct {
    char nt[10];
    char productions[MAX_PROD][MAX_LINE];
    int prod_count;
} Production;

typedef struct {
    Production prods[MAX_SYMBOLS];
    int prod_count;
    char terminals[MAX_SYMBOLS][10];
    int term_count;
    char non_terminals[MAX_SYMBOLS][10];
    int nt_count;
    char start_symbol[10];
} CFG;

typedef struct {
    char symbol[10];
    char first[MAX_SYMBOLS][10];
    int first_count;
} FirstSet;

typedef struct {
    char symbol[10];
    char follow[MAX_SYMBOLS][10];
    int follow_count;
} FollowSet;

// Function prototypes
void readCFG(CFG *cfg, const char *filename);
void printCFG(CFG *cfg, const char *stage);
void leftFactor(CFG *cfg);
void removeLeftRecursion(CFG *cfg);
void computeFirst(CFG *cfg, FirstSet *first);
void computeFollow(CFG *cfg, FirstSet *first, FollowSet *follow);
void buildLL1Table(CFG *cfg, FirstSet *first, FollowSet *follow);
int isNonTerminal(CFG *cfg, const char *symbol);
int isTerminal(CFG *cfg, const char *symbol);
int addToSet(char set[][10], int *count, const char *symbol);

void readCFG(CFG *cfg, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file %s\n", filename);
        exit(1);
    }

    char line[MAX_LINE];
    cfg->prod_count = 0;
    cfg->term_count = 0;
    cfg->nt_count = 0;

    while (fgets(line, MAX_LINE, file)) {
        line[strcspn(line, "\n")] = 0;
        if (strlen(line) <= 1) continue;

        char nt[10], rhs[MAX_LINE];
        if (sscanf(line, "%s -> %[^\n]", nt, rhs) != 2) continue;

        strcpy(cfg->prods[cfg->prod_count].nt, nt);
        cfg->prods[cfg->prod_count].prod_count = 0;
        if (cfg->prod_count == 0) strcpy(cfg->start_symbol, nt);

        char *token = strtok(rhs, "|");
        while (token) {
            while (*token && isspace(*token)) token++;
            char *end = token + strlen(token) - 1;
            while (end > token && isspace(*end)) *end-- = 0;
            strcpy(cfg->prods[cfg->prod_count].productions[cfg->prods[cfg->prod_count].prod_count++], token);
            token = strtok(NULL, "|");
        }

        addToSet(cfg->non_terminals, &cfg->nt_count, nt);
        cfg->prod_count++;
    }

    for (int i = 0; i < cfg->prod_count; i++) {
        for (int j = 0; j < cfg->prods[i].prod_count; j++) {
            char *rhs = cfg->prods[i].productions[j];
            char token[10];
            int pos = 0, k = 0;
            while (rhs[pos]) {
                if (isspace(rhs[pos])) { pos++; continue; }
                k = 0;
                while (rhs[pos] && !isspace(rhs[pos]) && k < 9) token[k++] = rhs[pos++];
                token[k] = '\0';
                if (!isNonTerminal(cfg, token)) addToSet(cfg->terminals, &cfg->term_count, token);
            }
        }
    }

    fclose(file);
    printCFG(cfg, "Original CFG");
}

void printCFG(CFG *cfg, const char *stage) {
    printf("\n%s:\n", stage);
    for (int i = 0; i < cfg->prod_count; i++) {
        printf("%s -> ", cfg->prods[i].nt);
        for (int j = 0; j < cfg->prods[i].prod_count; j++) {
            printf("%s", cfg->prods[i].productions[j]);
            if (j < cfg->prods[i].prod_count - 1) printf(" | ");
        }
        printf("\n");
    }
}

void leftFactor(CFG *cfg) {
    CFG new_cfg = *cfg;
    int new_count = cfg->prod_count;

    for (int i = 0; i < cfg->prod_count; i++) {
        Production *p = &cfg->prods[i];
        char prefix[MAX_LINE] = "";
        int has_common = 0;

        for (int j = 0; j < p->prod_count - 1; j++) {
            for (int k = j + 1; k < p->prod_count; k++) {
                char *p1 = p->productions[j];
                char *p2 = p->productions[k];
                int len = 0;
                while (p1[len] && p2[len] && p1[len] == p2[len] && !isspace(p1[len])) len++;
                if (len > 0) {
                    strncpy(prefix, p1, len);
                    prefix[len] = '\0';
                    has_common = 1;
                    break;
                }
            }
            if (has_common) break;
        }

        if (has_common) {
            char new_nt[10];
            sprintf(new_nt, "%s'", p->nt);
            addToSet(new_cfg.non_terminals, &new_cfg.nt_count, new_nt);

            strcpy(new_cfg.prods[new_count].nt, new_nt);
            new_cfg.prods[new_count].prod_count = 0;

            p->prod_count = 1;
            sprintf(p->productions[0], "%s %s", prefix, new_nt);

            for (int j = 0; j < cfg->prods[i].prod_count; j++) {
                char *prod = cfg->prods[i].productions[j];
                if (strncmp(prod, prefix, strlen(prefix)) == 0) {
                    char suffix[MAX_LINE];
                    strcpy(suffix, prod + strlen(prefix));
                    if (strlen(suffix) == 0) strcpy(suffix, EPSILON);
                    else if (suffix[0] == ' ') memmove(suffix, suffix + 1, strlen(suffix));
                    strcpy(new_cfg.prods[new_count].productions[new_cfg.prods[new_count].prod_count++], suffix);
                } else {
                    strcpy(new_cfg.prods[i].productions[p->prod_count++], prod);
                }
            }
            new_count++;
        }
    }

    *cfg = new_cfg;
    cfg->prod_count = new_count;
    printCFG(cfg, "After Left Factoring");
}

void removeLeftRecursion(CFG *cfg) {
    CFG new_cfg = *cfg;
    int new_count = cfg->prod_count;

    for (int i = 0; i < cfg->prod_count; i++) {
        Production *p = &new_cfg.prods[i];
        int recursive = 0;
        int non_recursive_count = 0;
        char non_recursive[MAX_PROD][MAX_LINE];
        char recursive_suffix[MAX_PROD][MAX_LINE];
        int recursive_count = 0;

        // Separate recursive and non-recursive productions
        for (int j = 0; j < p->prod_count; j++) {
            char *rhs = p->productions[j];
            char token[10];
            sscanf(rhs, "%s", token);
            if (strcmp(token, p->nt) == 0) {
                recursive = 1;
                char suffix[MAX_LINE];
                strcpy(suffix, rhs + strlen(token));
                if (suffix[0] == ' ') memmove(suffix, suffix + 1, strlen(suffix));
                strcpy(recursive_suffix[recursive_count++], suffix);
            } else {
                strcpy(non_recursive[non_recursive_count++], rhs);
            }
        }

        if (recursive) {
            char new_nt[10];
            sprintf(new_nt, "%s'", p->nt);
            addToSet(new_cfg.non_terminals, &new_cfg.nt_count, new_nt);

            // Replace original productions with non-recursive ones
            p->prod_count = 0;
            for (int j = 0; j < non_recursive_count; j++) {
                sprintf(p->productions[p->prod_count++], "%s %s", non_recursive[j], new_nt);
            }

            // Add new productions for the recursive part
            strcpy(new_cfg.prods[new_count].nt, new_nt);
            new_cfg.prods[new_count].prod_count = 0;
            for (int j = 0; j < recursive_count; j++) {
                sprintf(new_cfg.prods[new_count].productions[new_cfg.prods[new_count].prod_count++], "%s %s", recursive_suffix[j], new_nt);
            }
            strcpy(new_cfg.prods[new_count].productions[new_cfg.prods[new_count].prod_count++], EPSILON);
            new_count++;
        }
    }

    *cfg = new_cfg;
    cfg->prod_count = new_count;
    printCFG(cfg, "After Left Recursion Removal");
}

void computeFirst(CFG *cfg, FirstSet *first) {
    for (int i = 0; i < cfg->nt_count; i++) {
        strcpy(first[i].symbol, cfg->non_terminals[i]);
        first[i].first_count = 0;
    }

    int changed;
    do {
        changed = 0;
        for (int i = 0; i < cfg->prod_count; i++) {
            char *nt = cfg->prods[i].nt;
            int nt_idx = -1;
            for (int k = 0; k < cfg->nt_count; k++) {
                if (strcmp(first[k].symbol, nt) == 0) nt_idx = k;
            }

            for (int j = 0; j < cfg->prods[i].prod_count; j++) {
                char *rhs = cfg->prods[i].productions[j];
                char token[10];
                sscanf(rhs, "%s", token);

                if (strcmp(token, EPSILON) == 0) {
                    if (addToSet(first[nt_idx].first, &first[nt_idx].first_count, EPSILON)) changed = 1;
                } else if (isTerminal(cfg, token)) {
                    if (addToSet(first[nt_idx].first, &first[nt_idx].first_count, token)) changed = 1;
                } else if (isNonTerminal(cfg, token)) {
                    int sub_idx = -1;
                    for (int m = 0; m < cfg->nt_count; m++) {
                        if (strcmp(first[m].symbol, token) == 0) sub_idx = m;
                    }
                    for (int m = 0; m < first[sub_idx].first_count; m++) {
                        if (addToSet(first[nt_idx].first, &first[nt_idx].first_count, first[sub_idx].first[m])) changed = 1;
                    }
                }
            }
        }
    } while (changed);

    printf("\nFIRST Sets:\n");
    for (int i = 0; i < cfg->nt_count; i++) {
        printf("FIRST(%s) = { ", first[i].symbol);
        for (int j = 0; j < first[i].first_count; j++) {
            printf("%s%s", first[i].first[j], j < first[i].first_count - 1 ? ", " : "");
        }
        printf(" }\n");
    }
}

void computeFollow(CFG *cfg, FirstSet *first, FollowSet *follow) {
    // Initialize FOLLOW sets
    for (int i = 0; i < cfg->nt_count; i++) {
        strcpy(follow[i].symbol, cfg->non_terminals[i]);
        follow[i].follow_count = 0;
    }
    addToSet(follow[0].follow, &follow[0].follow_count, "$"); // Start symbol gets $

    int changed;
    do {
        changed = 0;
        for (int i = 0; i < cfg->prod_count; i++) {
            char *nt = cfg->prods[i].nt;
            int nt_idx = -1;
            for (int k = 0; k < cfg->nt_count; k++) {
                if (strcmp(follow[k].symbol, nt) == 0) nt_idx = k;
            }

            for (int j = 0; j < cfg->prods[i].prod_count; j++) {
                char *rhs = cfg->prods[i].productions[j];
                char tokens[MAX_SYMBOLS][10];
                int token_count = 0;
                char *token = strtok(rhs, " ");
                while (token) {
                    strcpy(tokens[token_count++], token);
                    token = strtok(NULL, " ");
                }

                for (int t = 0; t < token_count; t++) {
                    if (isNonTerminal(cfg, tokens[t])) {
                        int curr_idx = -1;
                        for (int k = 0; k < cfg->nt_count; k++) {
                            if (strcmp(follow[k].symbol, tokens[t]) == 0) curr_idx = k;
                        }

                        // If the current non-terminal is at the end of the production,
                        // add FOLLOW(nt) to FOLLOW(current non-terminal)
                        if (t == token_count - 1) {
                            for (int m = 0; m < follow[nt_idx].follow_count; m++) {
                                if (addToSet(follow[curr_idx].follow, &follow[curr_idx].follow_count, follow[nt_idx].follow[m])) changed = 1;
                            }
                        } else {
                            // Look at the next token
                            char *next = tokens[t + 1];
                            if (isTerminal(cfg, next)) {
                                if (addToSet(follow[curr_idx].follow, &follow[curr_idx].follow_count, next)) changed = 1;
                            } else if (isNonTerminal(cfg, next)) {
                                int next_idx = -1;
                                for (int k = 0; k < cfg->nt_count; k++) {
                                    if (strcmp(first[k].symbol, next) == 0) next_idx = k;
                                }

                                // Add FIRST(next) to FOLLOW(current non-terminal)
                                for (int m = 0; m < first[next_idx].first_count; m++) {
                                    if (strcmp(first[next_idx].first[m], EPSILON) != 0) {
                                        if (addToSet(follow[curr_idx].follow, &follow[curr_idx].follow_count, first[next_idx].first[m])) changed = 1;
                                    }
                                }

                                // If FIRST(next) contains epsilon, add FOLLOW(nt) to FOLLOW(current non-terminal)
                                int has_epsilon = 0;
                                for (int m = 0; m < first[next_idx].first_count; m++) {
                                    if (strcmp(first[next_idx].first[m], EPSILON) == 0) {
                                        has_epsilon = 1;
                                        break;
                                    }
                                }
                                if (has_epsilon) {
                                    for (int m = 0; m < follow[nt_idx].follow_count; m++) {
                                        if (addToSet(follow[curr_idx].follow, &follow[curr_idx].follow_count, follow[nt_idx].follow[m])) changed = 1;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    } while (changed);

    printf("\nFOLLOW Sets:\n");
    for (int i = 0; i < cfg->nt_count; i++) {
        printf("FOLLOW(%s) = { ", follow[i].symbol);
        for (int j = 0; j < follow[i].follow_count; j++) {
            printf("%s%s", follow[i].follow[j], j < follow[i].follow_count - 1 ? ", " : "");
        }
        printf(" }\n");
    }
}
void buildLL1Table(CFG *cfg, FirstSet *first, FollowSet *follow) {
    printf("\nLL(1) Parsing Table:\n");
    printf("NT\\T ");
    for (int i = 0; i < cfg->term_count; i++) printf("%-5s ", cfg->terminals[i]);
    printf("$    \n");

    for (int i = 0; i < cfg->nt_count; i++) {
        printf("%-4s ", cfg->non_terminals[i]);
        int nt_idx = i;

        for (int t = 0; t <= cfg->term_count; t++) {
            char term[10];
            if (t < cfg->term_count) strcpy(term, cfg->terminals[t]);
            else strcpy(term, "$");
            int prod_idx = -1, prod_alt = -1;

            for (int p = 0; p < cfg->prod_count; p++) {
                if (strcmp(cfg->prods[p].nt, cfg->non_terminals[i]) == 0) {
                    for (int j = 0; j < cfg->prods[p].prod_count; j++) {
                        char *rhs = cfg->prods[p].productions[j];
                        char token[10];
                        sscanf(rhs, "%s", token);

                        if (strcmp(token, EPSILON) == 0) {
                            for (int f = 0; f < follow[nt_idx].follow_count; f++) {
                                if (strcmp(follow[nt_idx].follow[f], term) == 0) {
                                    prod_idx = p;
                                    prod_alt = j;
                                    break;
                                }
                            }
                        } else if (isTerminal(cfg, token) && strcmp(token, term) == 0) {
                            prod_idx = p;
                            prod_alt = j;
                        } else if (isNonTerminal(cfg, token)) {
                            int sub_idx = -1;
                            for (int k = 0; k < cfg->nt_count; k++) {
                                if (strcmp(first[k].symbol, token) == 0) sub_idx = k;
                            }
                            for (int f = 0; f < first[sub_idx].first_count; f++) {
                                if (strcmp(first[sub_idx].first[f], term) == 0) {
                                    prod_idx = p;
                                    prod_alt = j;
                                    break;
                                }
                            }
                        }
                        if (prod_idx != -1) break;
                    }
                    if (prod_idx != -1) break;
                }
            }
            if (prod_idx != -1) {
                printf("%s->%-5s ", cfg->prods[prod_idx].nt, cfg->prods[prod_idx].productions[prod_alt]);
            } else {
                printf("     ");
            }
        }
        printf("\n");
    }
}

int isNonTerminal(CFG *cfg, const char *symbol) {
    for (int i = 0; i < cfg->nt_count; i++) {
        if (strcmp(cfg->non_terminals[i], symbol) == 0) return 1;
    }
    return 0;
}

int isTerminal(CFG *cfg, const char *symbol) {
    for (int i = 0; i < cfg->term_count; i++) {
        if (strcmp(cfg->terminals[i], symbol) == 0) return 1;
    }
    return 0;
}

int addToSet(char set[][10], int *count, const char *symbol) {
    for (int i = 0; i < *count; i++) {
        if (strcmp(set[i], symbol) == 0) return 0;
    }
    strcpy(set[*count], symbol);
    (*count)++;
    return 1;
}

int main() {
    CFG cfg;
    FirstSet first[MAX_SYMBOLS];
    FollowSet follow[MAX_SYMBOLS];

    readCFG(&cfg, "input.txt");
    leftFactor(&cfg);
    removeLeftRecursion(&cfg);
    computeFirst(&cfg, first);
    computeFollow(&cfg, first, follow);
    buildLL1Table(&cfg, first, follow);

    return 0;
}