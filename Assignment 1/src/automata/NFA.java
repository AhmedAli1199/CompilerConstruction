package automata;

import java.util.*;

public class NFA {
    private State startState;
    private Set<State> acceptingStates;
    private Set<Transition> transitions;

    public NFA(State startState, Set<State> acceptingStates, Set<Transition> transitions) {
        this.startState = startState;
        this.acceptingStates = acceptingStates;
        this.transitions = transitions;
    }

	public void printNFA() {
		System.out.println("Start state: " + startState.getId());
		System.out.print("Accepting states: ");
		for (State state : acceptingStates) {
			System.out.print(state.getId() + " ");
		}
		System.out.println();
		for (Transition transition : transitions) {
			System.out.println("Transition: " + transition.getFromState().getId() + " -> "
					+ transition.getToState().getId() + " on " + transition.getSymbol());
		}
	}
	
	
	
	
    
	public Set<State> epsilonClosure(Set<State> states) {
	    Set<State> closure = new HashSet<>(states);
	    Stack<State> stack = new Stack<>();
	    stack.addAll(states);
	    
	    while (!stack.isEmpty()) {
	        State current = stack.pop();
	        for (Transition t : transitions) {
	            if (t.getFromState().equals(current) && t.getSymbol() == '0') {
	                State toState = t.getToState();
	                if (!closure.contains(toState)) {
	                    closure.add(toState);
	                    stack.push(toState);
	                }
	            }
	        }
	    }
	    return closure;
	}

	public Set<State> move(Set<State> states, char symbol) {
	    Set<State> result = new HashSet<>();
	    for (State state : states) {
	        for (Transition t : transitions) {
	            if (t.getFromState().equals(state) && t.getSymbol() == symbol) {
	                result.add(t.getToState());
	            }
	        }
	    }
	    return result;
	}
    
	public State getStartState() {
		return startState;
	}
	
	public Set<State> getAcceptingStates() {
		return acceptingStates;
	}
	
	public Set<Transition> getTransitions() {
		return transitions;
	}
	
	public void setStartState(State startState) {
		this.startState = startState;
	}
	
	public void setStartState(boolean startState) {
		
	}
	
	public void setAcceptingStates(Set<State> acceptingStates) {
		this.acceptingStates = acceptingStates;
	}
	
	public void setTransitions(Set<Transition> transitions) {
		this.transitions = transitions;
	}
	
	public void addTransition(Transition transition) {
		transitions.add(transition);
	}
    
    
}