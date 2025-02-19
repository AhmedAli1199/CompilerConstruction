package automata;

import java.util.HashSet;
import java.util.Set;
import java.util.Stack;

public class REtoNFA {
    
	private int currentStateNumber = 0;
	
    public REtoNFA() {
        // Constructor
    }

    public NFA convert(String regex) {
        // Use a stack to keep track of NFAs and operators
        Stack<NFA> nfaStack = new Stack<>();
        Stack<Character> operatorStack = new Stack<>();

        for (int i = 0; i < regex.length(); i++) {
            char c = regex.charAt(i);

            if (c == '(') {
                // Push '(' to the operator stack
                operatorStack.push(c);
            } else if (c == ')') {
                // Process all operators until '(' is encountered
                while (!operatorStack.isEmpty() && operatorStack.peek() != '(') {
                    processOperator(nfaStack, operatorStack);
                }
                // Pop the '(' from the stack
                if (!operatorStack.isEmpty() && operatorStack.peek() == '(') {
                    operatorStack.pop();
                }
            } else if (c == '*' || c == '|' || c == '.') {
                // Push operators to the stack
                operatorStack.push(c);
            } else {
                // Single character, create a single NFA and push to the NFA stack
                NFA single = singleNFA(c);
                nfaStack.push(single);
            }
        }

        // Process any remaining operators in the stack
        while (!operatorStack.isEmpty()) {
            processOperator(nfaStack, operatorStack);
        }

        // The final NFA is on the top of the stack
        if (nfaStack.size() != 1) {
            throw new IllegalArgumentException("Invalid regular expression");
        }

        return nfaStack.pop();
    }

    private void processOperator(Stack<NFA> nfaStack, Stack<Character> operatorStack) {
        if (operatorStack.isEmpty()) {
            throw new IllegalArgumentException("Invalid regular expression");
        }

        char operator = operatorStack.pop();

        if (operator == '*') {
            // Kleene star: Pop one NFA, apply star, and push back
            if (nfaStack.isEmpty()) {
                throw new IllegalArgumentException("Invalid regular expression");
            }
            NFA nfa = nfaStack.pop();
            NFA starNFA = starNFA(nfa);
            nfaStack.push(starNFA);
        } else if (operator == '|') {
            // Union: Pop two NFAs, apply union, and push back
            if (nfaStack.size() < 2) {
                throw new IllegalArgumentException("Invalid regular expression");
            }
            NFA nfa2 = nfaStack.pop();
            NFA nfa1 = nfaStack.pop();
            NFA unionNFA = unionNFA(nfa1, nfa2);
            nfaStack.push(unionNFA);
        } else if (operator == '.') {
            // Concatenation: Pop two NFAs, apply concatenation, and push back
            if (nfaStack.size() < 2) {
                throw new IllegalArgumentException("Invalid regular expression");
            }
            NFA nfa2 = nfaStack.pop();
            NFA nfa1 = nfaStack.pop();
            NFA concatNFA = concatNFA(nfa1, nfa2);
            nfaStack.push(concatNFA);
        } else {
            throw new IllegalArgumentException("Invalid operator: " + operator);
        }
    }

    // Concatenation of two NFAs
    public NFA concatNFA(NFA nfa1, NFA nfa2) {
        State startState = nfa1.getStartState();
        Set<State> acceptingStates = nfa2.getAcceptingStates();
        Set<Transition> transitions = nfa1.getTransitions();
        transitions.addAll(nfa2.getTransitions());

        for (State state : nfa1.getAcceptingStates()) {
            state.setAccepting(false);
            Transition t = new Transition(state, '0', nfa2.getStartState());
            transitions.add(t);
        }

        return new NFA(startState, acceptingStates, transitions);
    }

    // Union of two NFAs
    public NFA unionNFA(NFA nfa1, NFA nfa2) {
        State startState = new State(currentStateNumber++, false);
        State acceptState = new State(currentStateNumber++, true);
        Transition t1 = new Transition(startState, '0', nfa1.getStartState());
        Transition t2 = new Transition(startState, '0', nfa2.getStartState());
        Set<State> acceptingStates = new HashSet<>();
        acceptingStates.add(acceptState);
        Set<Transition> transitions = new HashSet<>();
        transitions.add(t1);
        transitions.add(t2);
        transitions.addAll(nfa1.getTransitions());
        transitions.addAll(nfa2.getTransitions());

        for (State state : nfa1.getAcceptingStates()) {
            state.setAccepting(false);
            Transition t = new Transition(state, '0', acceptState);
            transitions.add(t);
        }
        for (State state : nfa2.getAcceptingStates()) {
            state.setAccepting(false);
            Transition t = new Transition(state, '0', acceptState);
            transitions.add(t);
        }

        return new NFA(startState, acceptingStates, transitions);
    }

    // Kleene Star (Closure) for an existing NFA
    public NFA starNFA(NFA nfa) {
       //Just add a 0 transition from the accepting states to the start state for the nfa
    	for (State state: nfa.getAcceptingStates())
    	{
    		Transition t = new Transition(state, '0', nfa.getStartState());
    		nfa.addTransition(t);	
    	}
    	
    	return nfa;
    }   	
 

    // NFA for a single character
    public NFA singleNFA(char symbol) {
        State startState = new State(currentStateNumber++, false);
        State acceptState = new State(currentStateNumber++, true);
        Transition t1 = new Transition(startState, symbol, acceptState);
        Set<State> acceptingStates = new HashSet<>();
        acceptingStates.add(acceptState);
        Set<Transition> transitions = new HashSet<>();
        transitions.add(t1);
        return new NFA(startState, acceptingStates, transitions);
    }
}
