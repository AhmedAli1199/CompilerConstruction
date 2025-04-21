#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>

using namespace std;

class CFGParser {
private:
    map<string, vector<vector<string>>> productions;
    set<string> terminals;
    set<string> nonTerminals;
    string start_symbol;
    string epsilon;
    set<char> usedNonTerminals;
    map<pair<string, string>, vector<string>> table;


    void processProduction(const string& line) {
        size_t arrowPos = line.find("->");
        if (arrowPos == string::npos) return;

        string lhs = line.substr(0, arrowPos);
        lhs.erase(0, lhs.find_first_not_of(" \t"));
        lhs.erase(lhs.find_last_not_of(" \t") + 1);

        nonTerminals.insert(lhs);
        if (lhs.size() == 1 && isupper(lhs[0])) usedNonTerminals.insert(lhs[0]);
        if (start_symbol.empty()) start_symbol = lhs;

        string rhs = line.substr(arrowPos + 2);
        vector<string> alternatives;
        size_t start = 0, end = rhs.find('|');
        
        while (end != string::npos) {
            alternatives.push_back(rhs.substr(start, end - start));
            start = end + 1;
            end = rhs.find('|', start);
        }
        alternatives.push_back(rhs.substr(start));
        
        for (const auto& alt : alternatives) {
            string trimmedAlt = alt;
            trimmedAlt.erase(0, trimmedAlt.find_first_not_of(" \t"));
            trimmedAlt.erase(trimmedAlt.find_last_not_of(" \t") + 1);
            vector<string> symbols = tokenizeRHS(trimmedAlt);
            productions[lhs].push_back(symbols);
        }
    }

    vector<string> tokenizeRHS(const string& rhsString) {
        vector<string> tokens;
        string currentToken;
        
        for (size_t i = 0; i < rhsString.size(); i++) {
            char c = rhsString[i];
            if (isspace(c)) {
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
                continue;
            }
            if (c == '(' || c == ')') {
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
                tokens.push_back(string(1, c));
                continue;
            }
            if (isalpha(c)) {
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
                string identifier(1, c);
                size_t j = i + 1;
                while (j < rhsString.size() && (isalnum(rhsString[j]) || rhsString[j] == '\'')) {
                    identifier += rhsString[j];
                    j++;
                }
                tokens.push_back(identifier);
                if (identifier.size() == 1 && isupper(identifier[0])) usedNonTerminals.insert(identifier[0]);
                i = j - 1;
                continue;
            }
            if (c == '+' || c == '*') {
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
                tokens.push_back(string(1, c));
                continue;
            }
            currentToken += c;
        }
        if (!currentToken.empty()) tokens.push_back(currentToken);
        return tokens;
    }

    void identifyTerminals() {
        terminals.clear();
        for (const auto& production : productions) {
            for (const auto& alternatives : production.second) {
                for (const auto& symbol : alternatives) {
                    if (nonTerminals.find(symbol) == nonTerminals.end() && symbol != epsilon) {
                        terminals.insert(symbol);
                    }
                }
            }
        }
    }

    string getNewNonTerminal() {
        for (char c = 'A'; c <= 'Z'; c++) {
            if (usedNonTerminals.find(c) == usedNonTerminals.end()) {
                usedNonTerminals.insert(c);
                return string(1, c);
            }
        }
        throw runtime_error("No available non-terminals (A-Z exhausted)");
    }

public:
    CFGParser(const string& epsilonSym = "#") : epsilon(epsilonSym) {}
    
    bool parseFile(const string& filePath) {
        ifstream file(filePath);
        if (!file.is_open()) {
            cerr << "Error: Could not open file " << filePath << endl;
            return false;
        }

        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            processProduction(line);
        }

        if (nonTerminals.size() > 0 && start_symbol.empty()) {
            start_symbol = *nonTerminals.begin();
        }

        identifyTerminals();
        return true;
    }
    
    const map<string, vector<vector<string>>>& getProductions() const { return productions; }
    const set<string>& getTerminals() const { return terminals; }
    const set<string>& getNonTerminals() const { return nonTerminals; }
    const string& getStartSymbol() const { return start_symbol; }
    
    void printGrammar() const {
        for (const auto& production : productions) {
            cout << production.first << " -> ";
            bool first = true;
            for (const auto& alternative : production.second) {
                if (!first) cout << " | ";
                first = false;
                if (alternative.empty()) {
                    cout << epsilon;
                } else {
                    for (size_t i = 0; i < alternative.size(); i++) {
                        if (i > 0) cout << " ";
                        cout << alternative[i];
                    }
                }
            }
            cout << endl;
        }
    }

    // Left Factoring
    void leftFactor() {
        map<string, vector<vector<string>>> newProductions;
        for (const auto& prod : productions) {
            string nt = prod.first;
            vector<vector<string>> prods = prod.second;

            // Check if there's a common prefix to factor
            bool needsFactoring = false;
            string commonPrefix;
            vector<vector<string>> suffixes;
            vector<vector<string>> nonFactored;

            for (size_t i = 0; i < prods.size(); i++) {
                if (!prods[i].empty() && prods[i][0] == nt) {
                    needsFactoring = true;
                    commonPrefix = prods[i][0];
                    suffixes.push_back(vector<string>(prods[i].begin() + 1, prods[i].end()));
                } else {
                    nonFactored.push_back(prods[i]);
                }
            }

            if (needsFactoring && suffixes.size() > 1) {
                string newNt = getNewNonTerminal();
                newProductions[nt].push_back({commonPrefix, newNt});
                for (const auto& alt : nonFactored) {
                    newProductions[nt].push_back(alt);
                }
                newProductions[newNt] = suffixes;
                nonTerminals.insert(newNt);
            } else {
                newProductions[nt] = prods; // No factoring needed
            }
        }
        productions = newProductions;
        identifyTerminals();
        cout << "\nAfter Left Factoring:\n";
        printGrammar();
    }

    // Left Recursion Removal
    void removeLeftRecursion() {
        map<string, vector<vector<string>>> newProductions;
        for (const auto& prod : productions) {
            string nt = prod.first;
            vector<vector<string>> alphas, betas;
            for (const auto& alt : prod.second) {
                if (!alt.empty() && alt[0] == nt) {
                    vector<string> alpha(alt.begin() + 1, alt.end());
                    alphas.push_back(alpha);
                } else {
                    betas.push_back(alt);
                }
            }

            if (alphas.empty()) {
                newProductions[nt] = prod.second;
            } else {
                string newNt = getNewNonTerminal();
                nonTerminals.insert(newNt);
                vector<vector<string>> newBetas, newAlphas;
                for (auto beta : betas) {
                    if (beta.empty()) beta.push_back(epsilon);
                    beta.push_back(newNt);
                    newBetas.push_back(beta);
                }
                for (auto alpha : alphas) {
                    if (alpha.empty()) alpha.push_back(epsilon);
                    alpha.push_back(newNt);
                    newAlphas.push_back(alpha);
                }
                newAlphas.push_back({epsilon});
                newProductions[nt] = newBetas;
                newProductions[newNt] = newAlphas;
            }
        }
        productions = newProductions;
        identifyTerminals();
        cout << "\nAfter Left Recursion Removal:\n";
        printGrammar();
    }

    // Compute FIRST Sets
    map<string, set<string>> computeFirst() const {
        map<string, set<string>> first;
        for (const auto& nt : nonTerminals) first[nt];
        for (const auto& t : terminals) first[t].insert(t);

        bool changed;
        do {
            changed = false;
            for (const auto& prod : productions) {
                string nt = prod.first;
                for (const auto& alt : prod.second) {
                    if (alt.empty() || alt[0] == epsilon) {
                        if (first[nt].insert(epsilon).second) changed = true;
                    } else {
                        for (size_t i = 0; i < alt.size(); i++) {
                            string sym = alt[i];
                            for (const auto& f : first[sym]) {
                                if (f != epsilon && first[nt].insert(f).second) changed = true;
                            }
                            if (first[sym].find(epsilon) == first[sym].end()) break;
                            if (i == alt.size() - 1 && first[sym].find(epsilon) != first[sym].end()) {
                                if (first[nt].insert(epsilon).second) changed = true;
                            }
                        }
                    }
                }
            }
        } while (changed);

        cout << "\nFIRST Sets:\n";
        for (const auto& f : first) {
            if (nonTerminals.find(f.first) != nonTerminals.end()) {
                cout << "FIRST(" << f.first << ") = { ";
                bool firstItem = true;
                for (const auto& item : f.second) {
                    if (!firstItem) cout << ", ";
                    cout << item;
                    firstItem = false;
                }
                cout << " }\n";
            }
        }
        return first;
    }

    // Compute FOLLOW Sets
    map<string, set<string>> computeFollow() {
        map<string, set<string>> follow;
        for (const auto& nt : nonTerminals) follow[nt];
        follow[start_symbol].insert("$");

        map<string, set<string>> first = computeFirst();
        bool changed;
        do {
            changed = false;
            for (const auto& prod : productions) {
                string nt = prod.first;
                for (const auto& alt : prod.second) {
                    for (size_t i = 0; i < alt.size(); i++) {
                        if (nonTerminals.find(alt[i]) != nonTerminals.end()) {
                            if (i + 1 < alt.size()) {
                                string next = alt[i + 1];
                                for (const auto& f : first[next]) {
                                    if (f != epsilon && follow[alt[i]].insert(f).second) changed = true;
                                }
                                if (first[next].find(epsilon) != first[next].end()) {
                                    for (const auto& f : follow[nt]) {
                                        if (follow[alt[i]].insert(f).second) changed = true;
                                    }
                                }
                            } else {
                                for (const auto& f : follow[nt]) {
                                    if (follow[alt[i]].insert(f).second) changed = true;
                                }
                            }
                        }
                    }
                }
            }
        } while (changed);

        cout << "\nFOLLOW Sets:\n";
        for (const auto& f : follow) {
            cout << "FOLLOW(" << f.first << ") = { ";
            bool firstItem = true;
            for (const auto& item : f.second) {
                if (!firstItem) cout << ", ";
                cout << item;
                firstItem = false;
            }
            cout << " }\n";
        }
        return follow;
    }

    // Build LL(1) Parsing Table
    void buildLL1Table() {
        map<string, set<string>> first = computeFirst();
        map<string, set<string>> follow = computeFollow();
       
        for (const auto& prod : productions) {
            string nt = prod.first;
            for (const auto& alt : prod.second) {
                set<string> firstSet;
                bool hasEpsilon = false;
                if (alt.empty() || alt[0] == epsilon) {
                    hasEpsilon = true;
                } else {
                    for (size_t i = 0; i < alt.size(); i++) {
                        for (const auto& f : first[alt[i]]) {
                            if (f != epsilon) firstSet.insert(f);
                            else if (i == alt.size() - 1) hasEpsilon = true;
                        }
                        if (first[alt[i]].find(epsilon) == first[alt[i]].end()) break;
                    }
                }
                for (const auto& f : firstSet) {
                    table[{nt, f}] = alt;
                }
                if (hasEpsilon) {
                    for (const auto& f : follow[nt]) {
                        table[{nt, f}] = alt;
                    }
                }
            }
        }

        cout << "\nLL(1) Parsing Table:\n";
        cout << "NT\\T ";
        vector<string> terms(terminals.begin(), terminals.end());
        terms.push_back("$");
        for (const auto& t : terms) cout << "\t" << t;
        cout << "\n";

        for (const auto& nt : nonTerminals) {
            cout << nt << "\t";
            for (const auto& t : terms) {
                auto it = table.find({nt, t});
                if (it != table.end()) {
                    cout << nt << " -> ";
                    if (it->second.empty()) cout << epsilon;
                    else for (size_t i = 0; i < it->second.size(); i++) {
                        cout << it->second[i];
                        if (i < it->second.size() - 1) cout << " ";
                    }
                } else {
                    cout << " ";
                }
                cout << "\t";
            }
            cout << "\n";
        }

        write_table_to_file("LL1Table.txt");
        cout << "LL(1) Parsing Table written to LL1Table.txt\n";
    }

    void write_table_to_file(const string& filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Error: Could not open file " << filename << endl;
            return;
        }

        for (const auto& entry : table) {
            file << entry.first.first << " " << entry.first.second << " -> ";
            for (const auto& symbol : entry.second) {
                file << symbol << " ";
            }
            file << "\n";
        }
    }

    //This function returns the LL1 table
    map<pair<string, string>, vector<string>> getLL1Table() const {
        return table;
    }

   

    
};

// int main() {
//     CFGParser parser;
    
//     if (!parser.parseFile("input.txt")) {
//         cerr << "Failed to parse input file." << endl;
//         return 1;
//     }
    
//     cout << "Original Grammar:\n";
//     parser.printGrammar();
    
//     parser.leftFactor();
//     parser.removeLeftRecursion();
//     parser.buildLL1Table();
    
//     return 0;
// }