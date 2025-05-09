package lexer;

import java.util.regex.*;
import java.util.ArrayList;
import java.util.List;

public class Lexer {
    private String input;
    private int position;
    private int line;
    private int column;

    public Lexer(String input) {
        this.input = input;
        this.position = 0;
        this.line = 1;
        this.column = 1;
    }

    public List<Token> tokenize() {
        List<Token> tokens = new ArrayList<>();

        while (position < input.length()) {
            char currentChar = input.charAt(position);

            // Skip whitespace and track new lines
            if (Character.isWhitespace(currentChar)) {
                handleWhitespace();
                continue;
            }

            Token token = null;

            if (Character.isLowerCase(currentChar)) {
                token = scanIdentifier();
            } else if (Character.isDigit(currentChar)) {
                token = scanNumber();
            } else if (isOperator(currentChar)) {
                token = scanOperator();
            } else {
                throw new LexerException("Unexpected character: " + currentChar, line, column);
            }

            tokens.add(token);
        }

        // Add EOF token at the end
        tokens.add(new Token(TokenType.EOF, "", line, column));
        return tokens;
    }

    private void handleWhitespace() {
        char currentChar = input.charAt(position);
        if (currentChar == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        position++;
    }

    private Token scanIdentifier() {
        int start = position;
        int startColumn = column;

        while (position < input.length() && Character.isLowerCase(input.charAt(position))) {
            position++;
            column++;
        }

        String value = input.substring(start, position);
        return new Token(TokenType.IDENTIFIER, value, line, startColumn);
    }

    private Token scanNumber() {
        int start = position;
        int startColumn = column;
        boolean isDecimal = false;

        while (position < input.length() && (Character.isDigit(input.charAt(position)) || input.charAt(position) == '.')) {
            if (input.charAt(position) == '.') {
                if (isDecimal) break; // Only allow one decimal point
                isDecimal = true;
            }
            position++;
            column++;
        }

        String value = input.substring(start, position);
        return new Token(isDecimal ? TokenType.DECIMAL : TokenType.INTEGER, value, line, startColumn);
    }

    private Token scanOperator() {
        char operator = input.charAt(position);
        position++;
        column++;
        return new Token(TokenType.OPERATOR, String.valueOf(operator), line, column - 1);
    }

    private boolean isOperator(char c) {
        return "+-*/%^".indexOf(c) != -1;
    }
}
