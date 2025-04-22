#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tokens.h"

Token tokens[MAX_TOKENS];
int tokenCount = 0;
char *input;
int pos = 0;

TokenType checkKeyword(const char *value) {
    static struct {
        const char *word;
        TokenType type;
    } keywords[] = {
        {"if", TOKEN_IF},
        {"then", TOKEN_THEN},
        {"repeat", TOKEN_REPEAT},
        {"until", TOKEN_UNTIL},
        {"read", TOKEN_READ},
        {"write", TOKEN_WRITE},
        {"end", TOKEN_END},
        {NULL, TOKEN_ID}
    };

    for (int i = 0; keywords[i].word; i++) {
        if (strcmp(value, keywords[i].word) == 0) {
            return keywords[i].type;
        }
    }
    return TOKEN_ID;
}

char peek() {
    return input[pos];
}

char peekNext() {
    return input[pos + 1];
}

char advance() {
    return input[pos++];
}

void skipWhitespace() {
    while (isspace(peek())) {
        advance();
    }
}

Token* addToken(TokenType type, const char *value) {
    if (tokenCount >= MAX_TOKENS) {
        printf("Error: Too many tokens\n");
        exit(1);
    }
    Token *token = &tokens[tokenCount++];
    token->type = type;
    strncpy(token->value, value, MAX_TOKEN_LEN - 1);
    token->value[MAX_TOKEN_LEN - 1] = '\0';
    return token;
}

void scanTokens(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    input = (char*)malloc(size + 1);
    fread(input, 1, size, file);
    input[size] = '\0';
    fclose(file);

    while (pos < size) {
        char c = peek();

        if (isspace(c)) {
            skipWhitespace();
        }
        else if (isdigit(c)) {
            char num[MAX_TOKEN_LEN];
            int i = 0;
            while (isdigit(peek())) {
                num[i++] = advance();
            }
            num[i] = '\0';
            addToken(TOKEN_NUM, num);
        }
        else if (isalpha(c)) {
            char id[MAX_TOKEN_LEN];
            int i = 0;
            while (isalnum(peek())) {
                id[i++] = advance();
            }
            id[i] = '\0';
            TokenType type = checkKeyword(id);
            addToken(type, id);
        }
        else {
            switch (c) {
                case '+': advance(); addToken(TOKEN_PLUS, "+"); break;
                case '-': advance(); addToken(TOKEN_MINUS, "-"); break;
                case '*': advance(); addToken(TOKEN_MULT, "*"); break;
                case '/': advance(); addToken(TOKEN_DIV, "/"); break;
                case '=': advance(); addToken(TOKEN_EQ, "="); break;
                case ';': advance(); addToken(TOKEN_SEMICOLON, ";"); break;
                case '(': advance(); addToken(TOKEN_LPAREN, "("); break;
                case ')': advance(); addToken(TOKEN_RPAREN, ")"); break;
                case '<': advance(); addToken(TOKEN_LT, "<"); break;
                case ':':
                    if (peekNext() == '=') {
                        advance(); advance();
                        addToken(TOKEN_ASSIGN, ":=");
                    } else {
                        printf("Error: Unexpected character '%c'\n", c);
                        advance();
                    }
                    break;
                default:
                    printf("Error: Unexpected character '%c'\n", c);
                    advance();
            }
        }
    }

    addToken(TOKEN_EOF, "EOF");
    free(input);
}

void writeTokens(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Error: Could not open output file %s\n", filename);
        exit(1);
    }

    for (int i = 0; i < tokenCount; i++) {
        // Convert to lowercase for keywords to match your example
        char value[MAX_TOKEN_LEN];
        strcpy(value, tokens[i].value);
        if (tokens[i].type >= TOKEN_IF) { // It's a keyword
            for (int j = 0; value[j]; j++) {
                value[j] = tolower(value[j]);
            }
        }
        
        fprintf(file, "%s,%s\n", value, tokenTypeToString(tokens[i].type));
    }

    fclose(file);
}

int main() {
    const char *inputFile = "input.tiny";
    const char *outputFile = "tokens.txt";

    scanTokens(inputFile);
    writeTokens(outputFile);

    printf("Scanning complete. Tokens written to %s\n", outputFile);
    return 0;
}