package automata;

import java.util.HashSet;
import java.util.Set;

public class REtoNFA {
	
	public REtoNFA() {
	// Constructor
	}
	
	 public NFA convert(String regex) {
	        // Implement Thompson's Construction
		 
		 if(regex.length() == 1)
			             {
								return singleNFA(regex.charAt(0));
							} else if (regex.length() == 2 && regex.charAt(1) == '*') {
								return starNFA(regex.charAt(0));
							} else {
								return convertRegex(regex);
			             }
	    }
	 
		public NFA convertRegex(String regex) {
			NFA nfa = new NFA(null, null, null);
			NFA nfa1 = new NFA(null, null, null);
			NFA nfa2 = new NFA(null, null, null);
			char symbol = '0';
			int i = 0;
			while (i < regex.length()) {
				char c = regex.charAt(i);
				if (c == '(') {
					int j = i + 1;
					int count = 1;
					while (count != 0) {
						if (regex.charAt(j) == '(') {
							count++;
						} else if (regex.charAt(j) == ')') {
							count--;
						}
						j++;
					}
					nfa1 = convertRegex(regex.substring(i + 1, j - 1));
					i = j;
				} else if (c == '|') {
					int j = i + 1;
					while (j < regex.length() && regex.charAt(j) == '|') {
						j++;
					}
					nfa2 = convertRegex(regex.substring(i + 1, j));
					nfa = unionNFA(nfa1, nfa2);
					i = j;
				} else if (c == '.') {
					int j = i + 1;
					while (j < regex.length() && regex.charAt(j) == '.') {
						j++;
					}
					nfa2 = convertRegex(regex.substring(i + 1, j));
					nfa = concatNFA(nfa1, nfa2);
					i = j;
				} else if (c == '*') {
					nfa1 = starNFA(symbol);
				} else {
					nfa1 = singleNFA(c);
					symbol = c;
				}
				i++;
			}
			return nfa;
		}
	 
	 
	 
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
	 
		public NFA unionNFA(NFA nfa1, NFA nfa2) {
			State startState = new State(0, false);
			State acceptState = new State(1, true);
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
	 
	 
		public NFA starNFA(char symbol) {
			State startState = new State(0, false);
			State state1 = new State(1, false);
			State state2 = new State(2, false);
			State acceptState = new State(3, true);
			Transition t1 = new Transition(startState, '0', state1);
			Transition t2 = new Transition(state1, symbol, state2);
			Transition t3 = new Transition(state2, '0', acceptState);
			Transition t4 = new Transition(state2, '0', state1);
			Set<State> acceptingStates = new HashSet<>();
			acceptingStates.add(acceptState);
			Set<Transition> transitions = new HashSet<>();
			transitions.add(t1);
			transitions.add(t2);
			transitions.add(t3);
			transitions.add(t4);
			return new NFA(startState, acceptingStates, transitions);
		}
		
		public NFA singleNFA(char symbol)
		{
			State startState = new State(0, false);
            State acceptState = new State(1, true);
            Transition t1 = new Transition(startState, symbol, acceptState);
            Set<State> acceptingStates = new HashSet<>();
            acceptingStates.add(acceptState);
            Set<Transition> transitions = new HashSet<>();
            transitions.add(t1);
            return new NFA(startState, acceptingStates, transitions);
        
		}
		
		
}
