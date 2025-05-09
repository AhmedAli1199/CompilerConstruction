package lexer;

public class RegexPatterns {
    public static final String IDENTIFIER = "(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z)*";
    public static final String INTEGER = "(0|1|2|3|4|5|6|7|8|9)*";
    public static final String DECIMAL = "(0|1|2|3|4|5|6|7|8|9)*@.@(0|1|2|3|4|5|6|7|8|9)@(0|1|2|3|4|5|6|7|8|9)@(0|1|2|3|4|5|6|7|8|9)@(0|1|2|3|4|5|6|7|8|9)@(0|1|2|3|4|5|6|7|8|9)";
    public static final String OPERATOR = "^[+\\-*/%^]$";
    // Add more patterns for comments, strings, etc.
    
}