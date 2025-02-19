package automata;

public class Main {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		System.out.println("Hello, World!");
		
		REtoNFA converter = new REtoNFA();

        // Convert the regex (a|b)*
        String regex = "(a|b|c|d|e|f|g|h|i|j|k|l|z)*";
        NFA nfa = converter.convert(regex);
        	
        // Display the NFA
		nfa.printNFA();
	}
	
	
}
