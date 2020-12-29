#ifndef CRAWON_LEXER_H
#define CRAWON_LEXER_H

#define MAX_SRC_LEN   100 * 1000
#define MAX_TOKEN_LEN 100 * 100

typedef enum {
    /* Numeric tokens */
    TK_INT, TK_FLOAT,

    /* Arithmetic tokens */
    TK_PLUS, TK_MINUS, TK_MULT, TK_DIV,

    /* Other tokens */
    TK_LPAREN, TK_RPAREN, TK_EOF,
} tok_kind_t;

typedef struct {
    tok_kind_t kind;
    char txt[50];
} Token;

typedef struct {
    int idx;
    int tkcnt;
    char c;
    char src[MAX_SRC_LEN];
    Token tokens[MAX_TOKEN_LEN];
} Lexer;

Token create_token(tok_kind_t kind, char *txt);

Token make_number(Lexer *lexer);

void lexer_init(Lexer *lexer, char *fn);

void lexer_advance(Lexer *lexer);

void lexer_add_token(Lexer *lexer, Token token);

void lexer_lex(Lexer *lexer);


#endif //CRAWON_LEXER_H
