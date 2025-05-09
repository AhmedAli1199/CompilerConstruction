package lexer;

public class LexerException extends RuntimeException {
    public LexerException(String message, int line, int column) {
        super("Lexer Error at line " + line + ", column " + column + ": " + message);
    }
}
