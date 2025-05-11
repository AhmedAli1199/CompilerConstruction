# Compiler Construction Assignment Spring 2025 

## Team Members:

* Ahmed Ali 22i-0825

* Ahmad 22i-1288

-----------------------------------------------

## Language Name: J-

### Keywords:

int, bool, char, decimal, true, false .... (non-exhaustive list, will add more in the future)

### Operators:

+ - / * % ^ (Self explanatory)

### Regular Expressions:

- @: means concatenation

- $: means Kleene Star

- |: means Union

#### IDENTIFIER = 
"(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z)$"


#### DECIMAL = 
"((0|1|2|3|4|5|6|7|8|9)$)@.@(0|1|2|3|4|5|6|7|8|9)@(0|1|2|3|4|5|6|7|8|9)@(0|1|2|3|4|5|6|7|8|9)@(0|1|2|3|4|5|6|7|8|9)@(0|1|2|3|4|5|6|7|8|9)";


#### INTEGER = 
"(0|1|2|3|4|5|6|7|8|9)$";


#### OPERATOR = 
"(+|-|*|/|=|%|^)";


#### CHARACTER = 
"'@(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z)@'"
  

# json2relcsv

json2relcsv is a C-based command-line tool that converts JSON files into relational CSV tables. It parses JSON input, constructs an Abstract Syntax Tree (AST), and generates CSV files representing the data in a relational format. The tool handles nested objects and arrays, creating separate CSV files for top-level objects, nested objects, and arrays, with appropriate id fields for linking.

## Features

* Parses JSON files using a custom lexer (scanner.l) and parser (parser.y).

* Builds an AST to represent JSON structure.

* Converts JSON to relational CSV tables:

* Top-level objects are stored in table_name.csv.

* Arrays of objects are stored in <key>.csv (e.g., items.csv).
  
* Arrays of scalars are stored in junction.csv.
  
* Supports printing the AST for debugging.

* Handles various JSON data types: strings, numbers, booleans, null, objects, and arrays.

* Includes error handling for invalid JSON syntax.

### Prerequisites

* Operating System: Linux (e.g., Ubuntu)

#### Tools:

* gcc (GNU C Compiler)
 
* make (for building the project)
  
* flex (for lexer generation)

* bison (for parser generation)

* valgrind (optional, for memory debugging)

## Usage

Run the json2relcsv tool to process JSON files and generate CSV tables.

* make
* ./json2relcsv tests/test3.json --out-dir output    (for generating the csv file)
* cat output/table_name.csv                          (To view the content of table)
* ./json2relcsv tests/test3.json --print-ast --out-dir output   (To print the AST)

