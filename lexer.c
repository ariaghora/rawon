#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

int is_keyword(Token tok, char *txt) {
    return (tok.kind == TK_KEYWORD) && (strcmp(tok.txt, txt) == 0);
}

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

Token make_string(Lexer *lexer, char delim) {
    lexer_advance(lexer);
    char s[255] = "";

    int i = 0;
    while (lexer->c != delim) {
        /* capture escape sequence */
        if (lexer->c == '\\') {
            lexer_advance(lexer);
            switch (lexer->c) {
                case 't':
                    s[i++] = '\t';
                    break;
                case 'n':
                    s[i++] = '\n';
                    break;
                case 'r':
                    s[i++] = '\r';
                    break;
            }
            lexer_advance(lexer);
        } else {
            s[i++] = lexer->c;
            lexer_advance(lexer);
        }
    }
    s[i] = '\0';

    Token tok = create_token(TK_STR, s);
    tok.txtlen = i;

    return tok;
}

Token make_id_or_keyword(Lexer *lexer) {
    char s[255] = "";
    int i = 0;
    while (isalpha(lexer->c) || lexer->c == '_') {
        s[i++] = lexer->c;
        lexer_advance(lexer);
    }
    lexer->c = lexer->src[lexer->idx--];
    s[i] = '\0';

    if (strcmp(s, "if") == 0 ||
        strcmp(s, "elif") == 0 ||
        strcmp(s, "else") == 0 ||
        strcmp(s, "for") == 0 ||
        strcmp(s, "fn") == 0 ||
        strcmp(s, "return") == 0) {
        return create_token(TK_KEYWORD, s);
    }

    return create_token(TK_ID, s);
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

void lexer_advance_n(Lexer *lexer, int n) {
    for (int i = 0; i < n; ++i)
        lexer_advance(lexer);
}

void lexer_add_token_n_char(Lexer *lexer, Token token, int n) {
    lexer->tkcnt++;
    lexer->tokens[lexer->tkcnt - 1] = token;
    lexer_advance_n(lexer, n);
}

void lexer_add_token(Lexer *lexer, Token token) {
    lexer_add_token_n_char(lexer, token, 1);
}

void lexer_lex(Lexer *lexer) {
    while (lexer->c != '\0') {
        /* Skip whitespace. */
        while (lexer->c == ' ' || lexer->c == '\t' || lexer->c == '\r')
            lexer_advance(lexer);

        /* Lex numeric token. */

        if ((lexer->c >= '0') && (lexer->c <= '9'))
            lexer_add_token(lexer, make_number(lexer));

        else if (isalpha(lexer->c) || lexer->c == '_')
            /* Lex identifier */
            lexer_add_token(lexer, make_id_or_keyword(lexer));
        else if (lexer->c == '\'' || lexer->c == '"')
            /* Lex string token. */
            lexer_add_token(lexer, make_string(lexer, lexer->c));
        else if (lexer->c == ';' || lexer->c == '\n')
            /* Lex newline token. Semicolon and newline characters a allowed. */
            lexer_add_token(lexer, create_token(TK_EOL, "EOL"));

        else if (lexer->c == '+')
            lexer_add_token(lexer, create_token(TK_PLUS, "+"));
        else if (lexer->c == '-')
            lexer_add_token(lexer, create_token(TK_MINUS, "-"));
        else if (lexer->c == '*')
            lexer_add_token(lexer, create_token(TK_MULT, "*"));
        else if (lexer->c == '/')
            lexer_add_token(lexer, create_token(TK_DIV, "/"));

        else if (lexer->c == '=') {
            if (lexer_peek(lexer, 1) == '=') {
                /* When encountering `=`,
                 * case 1: check if next token should be `==` (logical equals).
                 */
                lexer_add_token_n_char(lexer, create_token(TK_EE, "=="), 2);
            } else {
                /*
                 * case 2: check if next token should be `=` (assignment op)
                 */
                lexer_add_token(lexer, create_token(TK_EQ, "="));
            }
        } else if (lexer->c == '<')
            lexer_add_token(lexer, create_token(TK_LT, "<"));

        else if (lexer->c == '>') {
            if (lexer_peek(lexer, 1) == '>') {
                lexer_add_token_n_char(lexer, create_token(TK_PIPE, ">>"), 2);
            } else
                lexer_add_token(lexer, create_token(TK_GT, ">"));
        } else if (lexer->c == '(')
            lexer_add_token(lexer, create_token(TK_LPAREN, "("));
        else if (lexer->c == ')')
            lexer_add_token(lexer, create_token(TK_RPAREN, ")"));
        else if (lexer->c == '[')
            lexer_add_token(lexer, create_token(TK_LBRACKET, "["));
        else if (lexer->c == ']')
            lexer_add_token(lexer, create_token(TK_RBRACKET, "]"));
        else if (lexer->c == '{')
            lexer_add_token(lexer, create_token(TK_LBRACE, "{"));
        else if (lexer->c == '}')
            lexer_add_token(lexer, create_token(TK_RBRACE, "}"));
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

char lexer_peek(Lexer *lexer, int offset) {
    return lexer->src[lexer->idx + offset];
}