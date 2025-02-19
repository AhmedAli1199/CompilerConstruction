package automata;

import java.util.List;

import lexer.Lexer;
import lexer.LexerException;
import lexer.Token;

public class Main {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		System.out.println("Hello, World!");
		
		REtoNFA converter = new REtoNFA();

        // Convert the regex (a|b)* 
//        String regex = "((0|1|2|3|4|5|6|7|8|9)*)@.@(0|1|2|3|4|5|6|7|8|9)@(0|1|2|3|4|5|6|7|8|9)@(0|1|2|3|4|5|6|7|8|9)@(0|1|2|3|4|5|6|7|8|9)@(0|1|2|3|4|5|6|7|8|9)";
//        //String regex = "((0|1|2|3|4|5|6|7|8|9)*)@.@(0|1|2|3|4|5|6|7|8|9)";
//		NFA nfa = converter.convert(regex);
//        	
//        // Display the NFA
//		nfa.printNFA();
//		
//		NFAtoDFA converter2 = new NFAtoDFA();
//		DFA dfa = converter2.convert(nfa);
//		dfa.printDFA();
//		
//		// Test the DFA
//		String input = "1223.122335";
//		boolean accepted = dfa.validateString(input);
//		System.out.println("Input " + input + " accepted: " + accepted);
		
		String input = "abc 123 45.67 + - * /";
        Lexer lexer = new Lexer(input);
        
        try {
            List<Token> tokens = lexer.tokenize();
            for (Token token : tokens) {
                System.out.println(token);
            }
        } catch (LexerException e) {
            System.err.println(e.getMessage());
        }
		
		
		
	}
	
	
}
