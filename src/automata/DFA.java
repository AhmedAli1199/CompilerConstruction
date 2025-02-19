package automata;

import java.util.*;

public class DFA {
    private State startState;
    private Set<State> acceptingStates;
    private Set<Transition> transitions;
    
    public DFA(State startState, Set<State> acceptingStates, Set<Transition> transitions) {
        this.startState = startState;
        this.acceptingStates = acceptingStates;
        this.transitions = transitions;
    }
    
    public boolean validateString(String input) {
        State currentState = startState;
        System.out.println("Starting at state " + currentState.getId());
        
        for (char c : input.toCharArray()) {
            System.out.println("Processing character: " + c);
            State nextState = getNextState(currentState, c);
            
            if (nextState == null) {
                System.out.println("Rejected: No transition found for character " + c + 
                                 " from state " + currentState.getId());
                return false;
            }
            
            System.out.println("Transitioned from state " + currentState.getId() + 
                              " to state " + nextState.getId());
            currentState = nextState;
        }
        
        boolean isAccepted = acceptingStates.contains(currentState);
        System.out.println("Final state: " + currentState.getId() + 
                          " (Accepting: " + isAccepted + ")");
        return isAccepted;
    }
    
    public State getNextState(State current, char symbol) {
        for (Transition t : transitions) {
            if (t.getFromState().equals(current) && t.getSymbol() == symbol) {
                return t.getToState();
            }
        }
        return null;  // No valid transition found
    }
    
    // Add getters and setters similar to NFA class
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
	
	public void setAcceptingStates(Set<State> acceptingStates) {
		this.acceptingStates = acceptingStates;

	}
	
	public void setTransitions(Set<Transition> transitions) {
		this.transitions = transitions;

	
	}
	
	public void addTransition(Transition transition) {
		transitions.add(transition);
	}
	
	
    public void printDFA() {
        System.out.println("DFA:");
        System.out.println("Start state: " + startState.getId());
        System.out.print("Accepting states: ");
        for (State state : acceptingStates) {
            System.out.print(state.getId() + " ");
        }
        System.out.println();
        for (Transition t : transitions) {
            System.out.println("Transition: " + t.getFromState().getId() + " -> " 
                + t.getToState().getId() + " on " + t.getSymbol());
        }
    }
}