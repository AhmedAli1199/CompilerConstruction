package lexer;

import automata.DFA;
import automata.State;

public class IdentifierDFA {
    public static DFA build() {
        State q0 = new State(0, false);
        State q1 = new State(1, true);
        DFA dfa = new DFA();
        dfa.setStartState(q0);

        for (char c = 'a'; c <= 'z'; c++) {
            dfa.addTransition(q0, c, q1);
            dfa.addTransition(q1, c, q1);
        }
        
        return dfa;
    }
}