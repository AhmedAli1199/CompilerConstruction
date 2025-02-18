package automata;

import java.util.HashMap;
import java.util.Map;

public class DFA {
    private State currentState;
    private State startState;
    private Map<State, Map<Character, State>> transitionTable = new HashMap<>();

    public void setStartState(State startState) {
        this.startState = startState;
        this.currentState = startState;
    }

    public void addTransition(State from, char symbol, State to) {
        transitionTable.putIfAbsent(from, new HashMap<>());
        transitionTable.get(from).put(symbol, to);
    }

    public boolean processInput(String input) {
        currentState = startState;
        for (char c : input.toCharArray()) {
            Map<Character, State> transitions = transitionTable.get(currentState);
            if (transitions == null || !transitions.containsKey(c)) return false;
            currentState = transitions.get(c);
        }
        return currentState.isAccepting();
    }

    public Map<State, Map<Character, State>> getTransitionTable() {
        return transitionTable;
    }
}