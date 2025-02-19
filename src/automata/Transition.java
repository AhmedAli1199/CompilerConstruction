package automata;

import java.util.Objects;

public class Transition {
    private State fromState;
    private char symbol;
    private State toState;

    public Transition(State fromState, char symbol, State toState) {
        this.fromState = fromState;
        this.symbol = symbol;
        this.toState = toState;
    }

    public State getFromState() {
        return fromState;
    }

    public char getSymbol() {
        return symbol;
    }

    public State getToState() {
        return toState;
    }
    
    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true;
        if (obj == null || getClass() != obj.getClass()) return false;
        Transition that = (Transition) obj;
        return symbol == that.symbol &&
               Objects.equals(fromState, that.fromState) &&
               Objects.equals(toState, that.toState);
    }

    @Override
    public int hashCode() {
        return Objects.hash(fromState, symbol, toState);
    }
}