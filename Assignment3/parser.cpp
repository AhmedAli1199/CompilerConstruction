#include "CFGParser.cpp"
#include<stack>
#include<fstream>
#include<string>
#include<vector>
using namespace std;

string get_terminal(string str, int input_pointer)
{
    string terminal="";
    for(;input_pointer<str.length();input_pointer++)
    {
        if(str[input_pointer]==' ' || str[input_pointer]=='\n')
          {  
             input_pointer++;
             break;
          }
          else if(str[input_pointer]=='\t')
          {
           input_pointer+=4;
              break; 
          }
          else if( str[input_pointer]=='$')
          {
                terminal+=str[input_pointer];
                break;
          }
            else
            terminal+=str[input_pointer];
    }
    return terminal;
}

bool is_NonTerminal(string str, set<string> nonTerminals)
{
    
    return nonTerminals.find(str) != nonTerminals.end();
}

void print_current_state(stack<string> parse_stack, string input, int index)
{
    cout << "Stack: ";
    stack<string> temp_stack = parse_stack;
    while (!temp_stack.empty()) {
        cout << temp_stack.top() << " ";
        temp_stack.pop();
    }
    
    cout << "\nInput: " << input.substr(index) << endl;
    cout << "----------------------------------------" << endl;
   
}


void perform_stack_parsing( map<pair<string, string>, vector<string>> table, CFGParser parser,string input)
{
    stack<string> parse_stack;

    set<string> terminals = parser.getTerminals();
    string start_symbol = parser.getStartSymbol();
    set<string> nonTerminals = parser.getNonTerminals();

    parse_stack.push("$");
    parse_stack.push(start_symbol);
    input += "$"; // Append end marker to input


    int input_pointer=0;
    string last_input="";
    vector<string> errors;
    
    print_current_state(parse_stack, input, input_pointer);
    
    string current_input = get_terminal(input, input_pointer);
    while(!parse_stack.empty())
    { 
       
        string top = parse_stack.top();
        
        if(top=="$" && current_input=="$")
        {
            cout<<"Parsing Completed with "<<errors.size()<<" Errors "<<endl;
            if(errors.size()>0)
            {
                cout<<"Errors: "<<endl;
                for(auto error:errors)
                {
                    cout<<error<<endl;
                }
            }
            else
            {
                cout<<"No Errors Found"<<endl;
            }
            return;
        }
        if (input_pointer >= input.length()) {
            cout << "Error: Input string ended unexpectedly." << endl;
            return;
        }

        if (is_NonTerminal(top, nonTerminals)) {
            
            parse_stack.pop();
            auto it = table.find({top, current_input});
            if (it != table.end()) {
                const vector<string>& production = it->second;
                for (auto rit = production.rbegin(); rit != production.rend(); ++rit) {
                    if (*rit != "#") { // Skip epsilon
                        parse_stack.push(*rit);
                    }
                }
            } else {
                cout << "Error: No production found for " << top << " with input " << current_input << endl;
                cout<<"Unexpected Token: "<<current_input<<" after "<<last_input<<endl;
                errors.push_back("Unexpected Token: "+current_input+" after "+last_input);
            }
        }
         else if (top == current_input)
        {   
            last_input = current_input;
            parse_stack.pop();
            input_pointer+=current_input.length()+1; // Move past the current input token
            // Get the next input token
            current_input = get_terminal(input, input_pointer);
        }
         else
        {
            cout << "Error: Stack top " << top << " does not match input " << current_input << endl;
            errors.push_back("Syntax Error: Expected " + top + " before " + current_input);
            parse_stack.pop(); // Pop the top of the stack to try and recover
        }
        
        //Print the current state of the stack and input
        print_current_state(parse_stack, input, input_pointer);


    }


}




string load_input_file(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        return "";
    }
    
    string content, line;
    while (getline(file, line)) {
        content += line + "\n";
    }
    
    return content;
}


int main()
{
    CFGParser parser;
    
    
    if (!parser.parseFile("input.txt")) {
        cerr << "Failed to parse input file." << endl;
        return 1;
    }
    
    cout << "Original Grammar:\n";
    parser.printGrammar();
    
    parser.leftFactor();
    parser.removeLeftRecursion();
    parser.buildLL1Table();
    cout<<endl<<" Start Symbol: "<<parser.getStartSymbol()<<endl;
    cout<<endl<<endl;

    //Sytax Parsing:
    string input = load_input_file("program.txt");
    cout << "Input String: " << input << endl;
    perform_stack_parsing(parser.getLL1Table(), parser, input);
    


}
