package lexer;

public class RegexPatterns {
    public static final String IDENTIFIER = "^[a-z]+$";
    public static final String INTEGER = "^\\d+$";
    public static final String DECIMAL = "^\\d+\\.\\d{1,5}$";
    public static final String OPERATOR = "^[+\\-*/%^]$";
    // Add more patterns for comments, strings, etc.
}