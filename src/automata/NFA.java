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
		return states;
        // Implement ε-closure logic
    }

    public Set<State> move(Set<State> states, char symbol) {
		return states;
        // Implement move logic
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
	
	public void setAcceptingStates(Set<State> acceptingStates) {
		this.acceptingStates = acceptingStates;
	}
	
	public void setTransitions(Set<Transition> transitions) {
		this.transitions = transitions;
	}
    
    
}