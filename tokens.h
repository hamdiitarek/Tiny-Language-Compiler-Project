#ifndef TOKENS_H
#define TOKENS_H

#define MAX_TOKEN_LEN 100
#define MAX_TOKENS 1000

typedef enum {
    TOKEN_ID = 0,           // IDENTIFIER
    TOKEN_NUM = 1,          // NUMBER
    TOKEN_PLUS = 2,         // PLUS
    TOKEN_MINUS = 3,        // MINUS
    TOKEN_MULT = 4,         // MULT
    TOKEN_DIV = 5,          // DIV
    TOKEN_ASSIGN = 6,       // ASSIGN
    TOKEN_SEMICOLON = 7,    // SEMICOLON
    TOKEN_LPAREN = 8,       // LPAREN
    TOKEN_RPAREN = 9,       // RPAREN
    TOKEN_LT = 10,          // LESSTHAN
    TOKEN_EQ = 11,          // EQUAL
    TOKEN_IF = 20,          // IF
    TOKEN_THEN = 21,        // THEN
    TOKEN_REPEAT = 22,      // REPEAT
    TOKEN_UNTIL = 23,       // UNTIL
    TOKEN_READ = 24,        // READ
    TOKEN_WRITE = 25,       // WRITE
    TOKEN_END = 26,         // END
    TOKEN_EOF = 99          // EOF
} TokenType;

typedef struct {
    char value[MAX_TOKEN_LEN];
    TokenType type;
} Token;

const char* tokenTypeToString(TokenType type);

#endif