#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokens.h"

#define MAX_CHILDREN 10

typedef struct TreeNode {
    Token token;
    struct TreeNode *children[MAX_CHILDREN];
    int numChildren;
} TreeNode;

Token tokens[MAX_TOKENS];
int tokenCount = 0;
int currentToken = 0;

// Forward declarations
TreeNode* createNode(Token token);
Token getCurrentToken();
Token getNextToken();
TreeNode* expression();
TreeNode* simple_expression();
TreeNode* term();
TreeNode* factor();
TreeNode* statement();
TreeNode* handle_assignment();
TreeNode* handle_read();
TreeNode* handle_write();
TreeNode* handle_if();
TreeNode* handle_repeat();
void readTokens(const char *filename);
void writeDotFile(TreeNode *node, const char *filename);
void freeTree(TreeNode *node);

TreeNode* createNode(Token token) {
    TreeNode *node = (TreeNode*)malloc(sizeof(TreeNode));
    node->token = token;
    node->numChildren = 0;
    for (int i = 0; i < MAX_CHILDREN; i++) {
        node->children[i] = NULL;
    }
    return node;
}

Token getCurrentToken() {
    if (currentToken >= tokenCount) {
        Token eof = {"EOF", TOKEN_EOF};
        return eof;
    }
    return tokens[currentToken];
}

Token getNextToken() {
    currentToken++;
    return getCurrentToken();
}

TreeNode* expression() {
    TreeNode* node = simple_expression();

    if (getCurrentToken().type == TOKEN_LT || 
        getCurrentToken().type == TOKEN_EQ) {
        Token op = getCurrentToken();
        getNextToken();
        TreeNode* opNode = createNode(op);
        opNode->children[0] = node;
        opNode->children[1] = simple_expression();
        opNode->numChildren = 2;
        node = opNode;
    }

    return node;
}

TreeNode* simple_expression() {
    TreeNode* node = term();
    
    while (getCurrentToken().type == TOKEN_PLUS || 
           getCurrentToken().type == TOKEN_MINUS) {
        Token op = getCurrentToken();
        getNextToken();
        TreeNode* opNode = createNode(op);
        opNode->children[0] = node;
        opNode->children[1] = term();
        opNode->numChildren = 2;
        node = opNode;
    }
    
    return node;
}

TreeNode* term() {
    TreeNode *node = factor();
    
    while (getCurrentToken().type == TOKEN_MULT || 
           getCurrentToken().type == TOKEN_DIV) {
        Token opToken = getCurrentToken();
        getNextToken();
        
        TreeNode *opNode = createNode(opToken);
        opNode->children[0] = node;
        opNode->children[1] = factor();
        opNode->numChildren = 2;
        node = opNode;
    }
    
    return node;
}

TokenType getTypeFromString(const char *typeStr) {
    if (strcmp(typeStr, "IDENTIFIER") == 0) return TOKEN_ID;
    if (strcmp(typeStr, "NUMBER") == 0) return TOKEN_NUM;
    if (strcmp(typeStr, "PLUS") == 0) return TOKEN_PLUS;
    if (strcmp(typeStr, "MINUS") == 0) return TOKEN_MINUS;
    if (strcmp(typeStr, "MULT") == 0) return TOKEN_MULT;
    if (strcmp(typeStr, "DIV") == 0) return TOKEN_DIV;
    if (strcmp(typeStr, "ASSIGN") == 0) return TOKEN_ASSIGN;
    if (strcmp(typeStr, "SEMICOLON") == 0) return TOKEN_SEMICOLON;
    if (strcmp(typeStr, "LPAREN") == 0) return TOKEN_LPAREN;
    if (strcmp(typeStr, "RPAREN") == 0) return TOKEN_RPAREN;
    if (strcmp(typeStr, "LESSTHAN") == 0) return TOKEN_LT;
    if (strcmp(typeStr, "EQUAL") == 0) return TOKEN_EQ;
    if (strcmp(typeStr, "IF") == 0) return TOKEN_IF;
    if (strcmp(typeStr, "THEN") == 0) return TOKEN_THEN;
    if (strcmp(typeStr, "REPEAT") == 0) return TOKEN_REPEAT;
    if (strcmp(typeStr, "UNTIL") == 0) return TOKEN_UNTIL;
    if (strcmp(typeStr, "READ") == 0) return TOKEN_READ;
    if (strcmp(typeStr, "WRITE") == 0) return TOKEN_WRITE;
    if (strcmp(typeStr, "END") == 0) return TOKEN_END;
    if (strcmp(typeStr, "EOF") == 0) return TOKEN_EOF;
    
    printf("Unknown token type: %s\n", typeStr);
    return TOKEN_EOF;
}

TreeNode* factor() {
    Token token = getCurrentToken();
    
    if (token.type == TOKEN_NUM || token.type == TOKEN_ID) {
        getNextToken();
        return createNode(token);
    }
    else if (token.type == TOKEN_LPAREN) {
        getNextToken();
        TreeNode *node = expression();
        
        if (getCurrentToken().type != TOKEN_RPAREN) {
            printf("Error: Expected right parenthesis\n");
            exit(1);
        }
        getNextToken();
        return node;
    }
    else {
        printf("Error: Unexpected token type %d\n", token.type);
        exit(1);
    }
}

TreeNode* handle_assignment() {
    TreeNode* idNode = createNode(getCurrentToken());
    getNextToken();
    
    Token current = getCurrentToken();
    if (current.type != TOKEN_ASSIGN) {
        printf("Error: Expected := after identifier '%s', but got '%s' (type %d)\n", 
              idNode->token.value, current.value, current.type);
        exit(1);
    }
    
    TreeNode* assignNode = createNode(current);
    getNextToken();
    
    assignNode->children[0] = idNode;
    assignNode->children[1] = expression();
    assignNode->numChildren = 2;
    
    return assignNode;
}

TreeNode* handle_read() {
    TreeNode* readNode = createNode(getCurrentToken());
    getNextToken();
    
    if (getCurrentToken().type != TOKEN_ID) {
        printf("Error: Expected identifier after READ\n");
        exit(1);
    }
    
    readNode->children[0] = createNode(getCurrentToken());
    readNode->numChildren = 1;
    getNextToken();
    
    return readNode;
}

TreeNode* handle_write() {
    TreeNode* writeNode = createNode(getCurrentToken());
    getNextToken();
    
    writeNode->children[0] = expression();
    writeNode->numChildren = 1;
    
    return writeNode;
}

TreeNode* handle_if() {
    TreeNode* ifNode = createNode(getCurrentToken());
    getNextToken();
    
    ifNode->children[0] = expression();
    
    if (getCurrentToken().type != TOKEN_THEN) {
        printf("Expected THEN after IF condition\n");
        exit(1);
    }
    getNextToken();
    
    ifNode->children[1] = statement();
    ifNode->numChildren = 2;
    
    return ifNode;
}

TreeNode* handle_repeat() {
    TreeNode* repeatNode = createNode(getCurrentToken());
    getNextToken();
    
    int stmtCount = 0;
    while (getCurrentToken().type != TOKEN_UNTIL) {
        repeatNode->children[stmtCount++] = statement();
        if (stmtCount >= MAX_CHILDREN) {
            printf("Too many statements in REPEAT block\n");
            exit(1);
        }
    }
    
    getNextToken(); // Consume UNTIL
    repeatNode->children[stmtCount] = expression();
    repeatNode->numChildren = stmtCount + 1;
    
    return repeatNode;
}

TreeNode* statement() {
    Token token = getCurrentToken();
    TreeNode* node = NULL;

    switch(token.type) {
        case TOKEN_READ: node = handle_read(); break;
        case TOKEN_WRITE: node = handle_write(); break;
        case TOKEN_IF: node = handle_if(); break;
        case TOKEN_REPEAT: node = handle_repeat(); break;
        case TOKEN_ID: node = handle_assignment(); break;
        default:
            printf("Unexpected token '%s' (type %d) at statement start\n", 
                  token.value, token.type);
            exit(1);
    }

    if (token.type != TOKEN_END && 
        token.type != TOKEN_THEN &&
        token.type != TOKEN_UNTIL) {
        if (getCurrentToken().type == TOKEN_SEMICOLON) {
            getNextToken();
        }
    }

    return node;
}

TreeNode* program() {
    TreeNode* root = createNode((Token){"PROGRAM", TOKEN_EOF});
    int stmtIndex = 0;
    
    while (getCurrentToken().type != TOKEN_EOF && 
           getCurrentToken().type != TOKEN_END) {
        root->children[stmtIndex] = statement();
        root->numChildren++;
        stmtIndex++;
        
        if (stmtIndex >= MAX_CHILDREN) {
            printf("Error: Too many statements\n");
            exit(1);
        }
    }
    
    if (getCurrentToken().type == TOKEN_END) {
        getNextToken();
    }
    
    return root;
}

void readTokens(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open token file %s\n", filename);
        exit(1);
    }

    char line[256];
    char value[MAX_TOKEN_LEN];
    char typeStr[MAX_TOKEN_LEN];
    
    tokenCount = 0;
    
    while (fgets(line, sizeof(line), file)) {
        // Remove trailing newline
        line[strcspn(line, "\n")] = 0;
        
        // Parse the line (format: "value,TYPE")
        if (sscanf(line, "%[^,],%s", value, typeStr) == 2) {
            TokenType type = getTypeFromString(typeStr);
            
            tokens[tokenCount].type = type;
            strncpy(tokens[tokenCount].value, value, MAX_TOKEN_LEN - 1);
            tokens[tokenCount].value[MAX_TOKEN_LEN - 1] = '\0';
            
            tokenCount++;
            
            if (tokenCount >= MAX_TOKENS) {
                printf("Error: Too many tokens\n");
                exit(1);
            }
        } else {
            printf("Error: Malformed token line: %s\n", line);
        }
    }

    fclose(file);
}

// Write a node and all its children recursively, returning the next node ID
int writeDotNode(FILE *file, TreeNode *node, int nodeID) {
    int currentID = nodeID;
    
    if (!node) return currentID;
    
    // Print this node
    fprintf(file, "  node%d [label=\"%s\"];\n", currentID, node->token.value);
    
    // Save current node ID for edges
    int parentID = currentID;
    currentID++;
    
    // Process all children
    for (int i = 0; i < node->numChildren; i++) {
        if (node->children[i]) {
            // Print edge from this node to child
            fprintf(file, "  node%d -> node%d;\n", parentID, currentID);
            
            // Process child node and update the ID counter
            currentID = writeDotNode(file, node->children[i], currentID);
        }
    }
    
    return currentID;
}

void writeDotFile(TreeNode *node, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Error: Could not open DOT file %s\n", filename);
        exit(1);
    }

    fprintf(file, "digraph SyntaxTree {\n");
    fprintf(file, "  node [shape=box, style=filled, fillcolor=lightblue];\n");
    fprintf(file, "  edge [arrowhead=vee];\n");
    
    writeDotNode(file, node, 0);
    
    fprintf(file, "}\n");
    fclose(file);
}

void freeTree(TreeNode *node) {
    if (!node) return;
    for (int i = 0; i < node->numChildren; i++) {
        freeTree(node->children[i]);
    }
    free(node);
}

int main() {
    const char *tokenFile = "tokens.txt";
    const char *dotFile = "tree.dot";

    readTokens(tokenFile);
    printf("Read %d tokens\n", tokenCount);
    
    TreeNode *root = program();
    
    if (getCurrentToken().type != TOKEN_EOF) {
        printf("Error: Expected end of input, got %s\n", 
               getCurrentToken().value);
        exit(1);
    }

    writeDotFile(root, dotFile);
    
    char command[256];
    snprintf(command, sizeof(command), "dot -Tpng %s -o tree.png", dotFile);
    system(command);

    printf("Syntax tree visualization has been generated as tree.png\n");

    freeTree(root);
    return 0;
}