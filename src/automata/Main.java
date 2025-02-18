package automata;

public class Main {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		System.out.println("Hello, World!");
		
		REtoNFA reToNFA = new REtoNFA();
		NFA nfa = reToNFA.convert("a.b");
		nfa.printNFA();
	}
	
	
}
