package automata;

import java.util.*;

public class NFAtoDFA {
    private int currentStateNumber = 0;
    
    public DFA convert(NFA nfa) {
        // Get alphabet (excluding epsilon)
        Set<Character> alphabet = getAlphabet(nfa);
        
        // Initialize the DFA's start state as epsilon closure of NFA's start state
        Set<State> initialNFAStates = nfa.epsilonClosure(new HashSet<>(Arrays.asList(nfa.getStartState())));
        
        // Maps sets of NFA states to DFA states
        Map<Set<State>, State> dfaStates = new HashMap<>();
        // Keep track of unprocessed DFA states
        Queue<Set<State>> unprocessedStates = new LinkedList<>();
        
        // Create start state for DFA
        State dfaStartState = new State(currentStateNumber++, isAccepting(initialNFAStates, nfa));
        dfaStates.put(initialNFAStates, dfaStartState);
        unprocessedStates.offer(initialNFAStates);
        
        Set<Transition> dfaTransitions = new HashSet<>();
        Set<State> dfaAcceptingStates = new HashSet<>();
        if (dfaStartState.isAccepting()) {
            dfaAcceptingStates.add(dfaStartState);
        }
        
        // Process all states in the queue
        while (!unprocessedStates.isEmpty()) {
            Set<State> currentNFAStates = unprocessedStates.poll();
            State currentDFAState = dfaStates.get(currentNFAStates);
            
            // For each symbol in the alphabet
            for (char symbol : alphabet) {
                // Get next state set using move and epsilon closure
                Set<State> nextStates = nfa.epsilonClosure(nfa.move(currentNFAStates, symbol));
                
                if (nextStates.isEmpty()) continue;
                
                // Create new DFA state if we haven't seen this set before
                if (!dfaStates.containsKey(nextStates)) {
                    State newState = new State(currentStateNumber++, isAccepting(nextStates, nfa));
                    dfaStates.put(nextStates, newState);
                    unprocessedStates.offer(nextStates);
                    
                    if (newState.isAccepting()) {
                        dfaAcceptingStates.add(newState);
                    }
                }
                
                // Add transition
                dfaTransitions.add(new Transition(
                    currentDFAState,
                    symbol,
                    dfaStates.get(nextStates)
                ));
            }
        }
        
        return new DFA(dfaStartState, dfaAcceptingStates, dfaTransitions);
    }
    
    private Set<Character> getAlphabet(NFA nfa) {
        Set<Character> alphabet = new HashSet<>();
        for (Transition t : nfa.getTransitions()) {
            if (t.getSymbol() != '0') { // Exclude epsilon transitions
                alphabet.add(t.getSymbol());
            }
        }
        return alphabet;
    }
    
    private boolean isAccepting(Set<State> states, NFA nfa) {
        return states.stream().anyMatch(State::isAccepting);
    }
}