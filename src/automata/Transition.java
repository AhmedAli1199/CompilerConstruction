package automata;

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
}