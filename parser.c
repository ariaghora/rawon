#include "lexer.h"
#include "parser.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


void parser_advance(Parser *parser) {
    parser->current = parser->tokens[++parser->idx];
}

void parser_init(Parser *parser, Lexer *lexer) {
    parser->idx = -1;
    parser->tokens = lexer->tokens;
    parser_advance(parser);
}

void expect(tok_kind_t exp_kind, Token found, char *exp_txt) {
    if (exp_kind != found.kind) {
        printf("Error: expected \"%s\", but \"%s\" found.", exp_txt, found.txt);
        exit(1);
    }
}

/*
 * Initializer
 */
AST *create_number_node(Token tok) {
    AST *res = calloc(1, sizeof(AST));
    if (tok.kind == TK_INT) {
        res->node_type = NT_INT;
        res->intval = atoi(tok.txt);
        sprintf(res->repr, "%d", res->intval);
    } else {
        res->node_type = NT_FLOAT;
        res->floatval = atof(tok.txt);
        sprintf(res->repr, "%f", res->floatval);
    }
    return res;
}

AST *create_bin_op(AST *left, AST *right, Token op) {
    AST *binop = malloc(sizeof(AST));
    binop->node_type = NT_BIN_OP;
    binop->left = left;
    binop->op = op;
    binop->right = right;
    return binop;
}

AST *parse_comp_expr(Parser *parser) {

    /*
     * Parse logical NOT operator
     *
     */

    AST *left = parse_arith(parser);

    /*
     * Parse ==, >, >=, etc
     */

    return left;
}

AST *parse_expr(Parser *parser) {
    AST *left = parse_comp_expr(parser);

    /*
     * Parse AND, OR
     */

    return left;
}

AST *parse_atom(Parser *parser) {
    Token tok = parser->current;
    if (tok.kind == TK_INT || tok.kind == TK_FLOAT) {
        parser_advance(parser);
        return create_number_node(tok);
    } else if (tok.kind == TK_LPAREN) {
        parser_advance(parser);
        AST *expr = parse_expr(parser);

        expect(TK_RPAREN, parser->current, ")");
        parser_advance(parser);

        return expr;
    }

    return NULL;
}

AST *parse_call(Parser *parser) {
    AST *at = parse_atom(parser);

    /*
     * Parse function call here later
     * if current token == lparen
     */

    return at;
}

AST *parse_power(Parser *parser) {
    AST *left = parse_call(parser);

    /*
     * Parser power BinOp later if token == '^'
     */

    return left;
}

AST *parse_factor(Parser *parser) {
    AST *left = parse_power(parser);
    return left;
}

AST *parse_term(Parser *parser) {
    AST *left = parse_factor(parser);

    /*
     * Parse multiplication and division (and div and mod)
     */
    while ((parser->current.kind == TK_MULT) ||
           (parser->current.kind == TK_DIV)) {
        Token op = parser->current;
        parser_advance(parser);
        AST *right = parse_factor(parser);
        left = create_bin_op(left, right, op);
    }

    return left;
}

AST *parse_arith(Parser *parser) {
    AST *left = parse_term(parser);

    while (parser->current.kind == TK_PLUS || parser->current.kind == TK_MINUS) {
        Token op = parser->current;
        parser_advance(parser);
        AST *right = parse_term(parser);
        left = create_bin_op(left, right, op);
    }

    return left;
}
