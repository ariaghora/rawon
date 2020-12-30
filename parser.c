#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"


void node_list_init(NodeList *node_list) {
    node_list->count = 0;
    node_list->max_cap = 10;
    node_list->arr = calloc(node_list->max_cap, sizeof(AST *));
}

void node_list_push(NodeList *node_list, AST *item) {
    if (node_list->count > (node_list->max_cap / 2)) {
        node_list->max_cap *= 2;
        node_list->arr = realloc(node_list->arr,
                                 sizeof(AST *) * node_list->max_cap);
    }
    node_list->arr[node_list->count++] = item;
}

void node_list_free(NodeList *node_list) {
    free(node_list);
}

void parser_advance(Parser *parser) {
    parser->current = parser->tokens[++parser->idx];
}

void parser_skip_newline(Parser *parser) {
    while (parser->current.kind == TK_EOL)
        parser_advance(parser);
}

void parser_init(Parser *parser, Lexer *lexer) {
    parser->idx = -1;
    parser->tokens = lexer->tokens;
    parser_advance(parser);
}

void expect(tok_kind_t exp_kind, Token found, char *exp_txt) {
    if (exp_kind != found.kind) {
        printf("Error: expected \"%s\", but \"%s\" found.\n", exp_txt, found.txt);
        exit(1);
    }
}

AST *create_number_node(Token tok) {
    AST *res = calloc(1, sizeof(AST));
    if (tok.kind == TK_INT) {
        res->node_type = NT_INT;
        res->intval = atoi(tok.txt);
    } else {
        res->node_type = NT_FLOAT;
        res->floatval = atof(tok.txt);
    }
    return res;
}

AST *create_string_node(Token tok) {
    AST *res = calloc(1, sizeof(AST));
    res->node_type = NT_STRING;
    res->strvallen = tok.txtlen;
    res->strval = calloc(res->strvallen + 1, sizeof(char));
    strcpy(res->strval, tok.txt);

    return res;
}

AST *create_bin_op(AST *left, AST *right, Token op) {
    AST *binop = calloc(1, sizeof(AST));
    binop->node_type = NT_BIN_OP;
    binop->left = left;
    binop->op = op;
    binop->right = right;
    return binop;
}

AST *create_list_node(AST **node_list, int node_list_cnt) {
    AST *list_node = calloc(1, sizeof(AST));
    list_node->node_type = NT_LIST;
    list_node->node_list = node_list;
    list_node->node_list_cnt = node_list_cnt;
    return list_node;
}

AST *parser_parse(Parser *parser) {
    return parse_block(parser);
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

AST *parse_atom(Parser *parser) {
    Token tok = parser->current;
    /*
     * Parse numeric literal
     */
    if (tok.kind == TK_INT || tok.kind == TK_FLOAT) {
        parser_advance(parser);
        return create_number_node(tok);
    } else if (tok.kind == TK_STR) {
        /*
         * Parse string literal
         */
        parser_advance(parser);
        return create_string_node(tok);
    } else if (tok.kind == TK_LPAREN) {
        parser_advance(parser);
        AST *expr = parse_expr(parser);

        expect(TK_RPAREN, parser->current, ")");
        parser_advance(parser);

        return expr;
    } else if (tok.kind == TK_LBRACKET) {
        return parse_list(parser);
    }

    return NULL;
}

AST *parse_block(Parser *parser) {
    /* skip trailing empty lines */
    parser_skip_newline(parser);

    NodeList *node_list = calloc(1, sizeof(NodeList));
    node_list_init(node_list);

    AST *stmt = parse_statement(parser);
    node_list_push(node_list, stmt);

    int cnt_newline;
    int more_stmt = 1;
    while (1) {
        cnt_newline = 0;

        while (parser->current.kind == TK_EOL) {
            parser_advance(parser);
            cnt_newline++;
        }

        if (cnt_newline == 0) {
            more_stmt = 0;
        }

        if (!more_stmt) {
            break;
        }

        stmt = parse_statement(parser);
        if (stmt == NULL) {
            more_stmt = 0;
            continue;
        }

        node_list_push(node_list, stmt);
    }

    AST *res = create_list_node(node_list->arr, node_list->count);
    free(node_list);

    return res;

}

AST *parse_call(Parser *parser) {
    AST *at = parse_atom(parser);

    /*
     * Parse function call here later
     * if current token == lparen
     */

    return at;

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

AST *parse_factor(Parser *parser) {
    AST *left = parse_power(parser);
    return left;
}

AST *parse_list(Parser *parser) {
    /*
     * Should be implemented as a dynamic linked list
     */
    AST **node_list = calloc(100, sizeof(AST *));
    parser_advance(parser);
    int i = 0;

    if (parser->current.kind == TK_RBRACKET) {
        /* Empty list */
        parser_advance(parser);
    } else {
        node_list[i++] = parse_expr(parser);
        while (parser->current.kind == TK_COMMA) {
            parser_advance(parser);
            node_list[i++] = parse_expr(parser);
        }

        expect(TK_RBRACKET, parser->current, "]");
    }

    parser_advance(parser);
    return create_list_node(node_list, i);
}

AST *parse_power(Parser *parser) {
    AST *left = parse_call(parser);

    /*
     * Parser power BinOp later if token == '^'
     */

    return left;
}

AST *parse_statement(Parser *parser) {
    /*
     * Parse `return` statement
     * here
     */

    return parse_expr(parser);

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
