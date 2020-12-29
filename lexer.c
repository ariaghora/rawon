#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lexer.h"

Token create_token(tok_kind_t kind, char *txt) {
    Token res;
    res.kind = kind;
    strcpy(res.txt, txt);
    return res;
}

Token make_number(Lexer *lexer) {
    char s[50] = "";
    int i = 0;
    int numdot = 0;
    while (isdigit(lexer->c) || lexer->c == '.') {
        s[i++] = lexer->c;
        if (lexer->c == '.') numdot++;
        lexer_advance(lexer);
    }

    // backward 1 character after loop
    lexer->c = lexer->src[lexer->idx--];
    s[i] = '\0';

    if (numdot > 1) {
        printf("Invalid number format: %s\n", s);
        exit(1);
    }

    if (numdot == 1) return create_token(TK_FLOAT, s);
    else return create_token(TK_INT, s);
}

void lexer_init(Lexer *lexer, char *fn) {
    lexer->idx = 0;
    lexer->tkcnt = 0;

    FILE *f = fopen(fn, "r");
    int i = 0;
    if (f) {
        while (!feof(f)) lexer->src[i++] = fgetc(f);
        lexer->src[i - 1] = '\0';
        fclose(f);
    } else {
        printf("Error loading %s.", fn);
        exit(1);
    }

    lexer->c = lexer->src[lexer->idx];
}

void lexer_advance(Lexer *lexer) {
    lexer->c = lexer->src[++lexer->idx];
}

void lexer_add_token(Lexer *lexer, Token token) {
    lexer->tkcnt++;
    lexer->tokens[lexer->tkcnt - 1] = token;
    lexer_advance(lexer);
}

void lexer_lex(Lexer *lexer) {
    while (lexer->c != '\0') {
        while (lexer->c == ' ')
            lexer_advance(lexer); // skip whitespace

        if ((lexer->c >= '0') && (lexer->c <= '9'))
            lexer_add_token(lexer, make_number(lexer));
        else if (lexer->c == '+')
            lexer_add_token(lexer, create_token(TK_PLUS, "+"));
        else if (lexer->c == '-')
            lexer_add_token(lexer, create_token(TK_MINUS, "-"));
        else if (lexer->c == '*')
            lexer_add_token(lexer, create_token(TK_MULT, "*"));
        else if (lexer->c == '/')
            lexer_add_token(lexer, create_token(TK_DIV, "/"));
        else if (lexer->c == '(')
            lexer_add_token(lexer, create_token(TK_LPAREN, "("));
        else if (lexer->c == ')')
            lexer_add_token(lexer, create_token(TK_RPAREN, ")"));
        else if (lexer->c == '[')
            lexer_add_token(lexer, create_token(TK_LBRACKET, "["));
        else if (lexer->c == ']')
            lexer_add_token(lexer, create_token(TK_RBRACKET, "]"));
        else if (lexer->c == ',')
            lexer_add_token(lexer, create_token(TK_COMMA, ","));
        else {
            printf("Bad character: %c\n", lexer->c);
            exit(1);
            lexer_advance(lexer);
        }

    }
    lexer_add_token(lexer, create_token(TK_EOF, "EOF"));
}