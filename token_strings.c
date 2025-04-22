#include "tokens.h"

const char* tokenTypeToString(TokenType type) {
    switch(type) {
        case TOKEN_ID: return "IDENTIFIER";
        case TOKEN_NUM: return "NUMBER";
        case TOKEN_PLUS: return "PLUS";
        case TOKEN_MINUS: return "MINUS";
        case TOKEN_MULT: return "MULT";
        case TOKEN_DIV: return "DIV";
        case TOKEN_ASSIGN: return "ASSIGN";
        case TOKEN_SEMICOLON: return "SEMICOLON";
        case TOKEN_LPAREN: return "LPAREN";
        case TOKEN_RPAREN: return "RPAREN";
        case TOKEN_LT: return "LESSTHAN";
        case TOKEN_EQ: return "EQUAL";
        case TOKEN_IF: return "IF";
        case TOKEN_THEN: return "THEN";
        case TOKEN_REPEAT: return "REPEAT";
        case TOKEN_UNTIL: return "UNTIL";
        case TOKEN_READ: return "READ";
        case TOKEN_WRITE: return "WRITE";
        case TOKEN_END: return "END";
        case TOKEN_EOF: return "EOF";
        default: return "UNKNOWN";
    }
}