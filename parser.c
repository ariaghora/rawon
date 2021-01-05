#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"
#include "3rd_party/stb_ds.h"

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

AST *create_funccall_node(AST *node_to_call, AST **arglist) {
    AST *funccall_node = calloc(1, sizeof(AST));
    funccall_node->node_type = NT_FUNC_CALL;
    funccall_node->fcall_node_to_call = node_to_call;
    funccall_node->fcall_arglsit = arglist;
    return funccall_node;
}

AST *create_funcdef_node(Token name, Token *arglist, AST *body) {
    AST *funcdef_node = calloc(1, sizeof(AST));
    funcdef_node->node_type = NT_FUNC_DEF;
    funcdef_node->fn_arglist = arglist;
    funcdef_node->fn_body = body;
    funcdef_node->fn_name = name;

    return funcdef_node;
}

AST *create_return_node(AST *returned_node) {
    AST *return_node = calloc(1, sizeof(AST));
    return_node->node_type = NT_RETURN_NODE;
    return_node->returned_node = returned_node;
    return return_node;
}

AST *create_list_node(AST **node_list, int node_list_cnt) {
    AST *list_node = calloc(1, sizeof(AST));
    list_node->node_type = NT_LIST;
    list_node->node_list = node_list;
    list_node->node_list_cnt = node_list_cnt;
    return list_node;
}

AST *create_if_node(AST **conditions, AST **cases, AST *else_case,
                    int conditions_cnt) {
    AST *if_node = calloc(1, sizeof(AST));
    if_node->node_type = NT_IF;
    if_node->if_conditions = conditions;
    if_node->if_cases = cases;
    if_node->else_case = else_case;
    if_node->conditions_cnt = conditions_cnt;
    return if_node;
}

AST *create_varaccess_node(Token var_token) {
    AST *varaccess_node = calloc(1, sizeof(AST));
    varaccess_node->node_type = NT_VARACCESS;
    varaccess_node->var_token = var_token;
    return varaccess_node;
}

AST *create_varassign_node(Token var_token, AST *node) {
    AST *varassign_node = calloc(1, sizeof(AST));
    varassign_node->node_type = NT_VARASSIGN;
    varassign_node->var_token = var_token;
    varassign_node->var_node = node;
    return varassign_node;
}

AST *create_subscript_node(AST *node, AST *subscript_expr) {
    AST *subscript_node = calloc(1, sizeof(AST));
    subscript_node->node_type = NT_SUBSCRIPT;
    subscript_node->subscripted_node = node;
    subscript_node->subscript_expr = subscript_expr;
    return subscript_node;
}

AST *parser_parse(Parser *parser) {
    return parse_block(parser);
}

AST *parse_arith(Parser *parser) {
    AST *left = parse_term(parser);

    while (parser->current.kind == TK_PLUS ||
           parser->current.kind == TK_MINUS ||
           parser->current.kind == TK_PIPE) {
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
    } else if (tok.kind == TK_ID) {
        /*
         * Parse identifier, which can be:
         * - Variable assignment, if the next token is `=`
         * - Variable access
         */
        Token var_tok = parser->current;
        parser_advance(parser);
        if (parser->current.kind == TK_EQ) {
            parser_advance(parser);
            AST *var_node = parse_expr(parser);
            return create_varassign_node(var_tok, var_node);
        } else {
            return create_varaccess_node(var_tok);
        }
    } else if (tok.kind == TK_LPAREN) {
        /*
         * Parse parenthesized expr
         */
        parser_advance(parser);
        AST *expr = parse_expr(parser);

        expect(TK_RPAREN, parser->current, ")");
        parser_advance(parser);

        return expr;
    } else if (tok.kind == TK_LBRACKET) {
        /*
         * Parse list node
         */
        return parse_list(parser);
    } else if (is_keyword(tok, "if")) {
        return parse_if_expr(parser);
    } else if (is_keyword(tok, "fn")) {
        return parse_funcdef(parser);
    } else if (is_keyword(tok, "return")) {
        parser_advance(parser);
        AST *returned_node = parse_expr(parser);
        return create_return_node(returned_node);
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
    if (parser->current.kind == TK_LPAREN) {
        AST **arglist = NULL;
        parser_advance(parser);

        if (parser->current.kind == TK_RPAREN) {
            parser_advance(parser);
        } else {
                    arrpush(arglist, parse_expr(parser));
            while (parser->current.kind == TK_COMMA) {
                parser_advance(parser);
                        arrpush(arglist, parse_expr(parser));
            }

            expect(TK_RPAREN, parser->current, ")");
            parser_advance(parser);
        }

        return create_funccall_node(at, arglist);
    } else if (parser->current.kind == TK_LBRACKET) {
        parser_advance(parser);
        if (parser->current.kind == TK_RBRACKET) {
            printf("Error: invalid syntax.\n");
            exit(1);
        }
        AST *subscript_expr = parse_expr(parser);
        expect(TK_RBRACKET, parser->current, "]");

        parser_advance(parser);
        return create_subscript_node(at, subscript_expr);
    }

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
    while (parser->current.kind == TK_EE ||
           parser->current.kind == TK_LT) {
        Token op = parser->current;
        parser_advance(parser);
        AST *right = parse_arith(parser);
        left = create_bin_op(left, right, op);
    }

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

AST *parse_funcdef(Parser *parser) {
    parser_advance(parser);
    expect(TK_ID, parser->current, "identifier");
    Token func_name = parser->current;

    /* opening parentheses */
    parser_advance(parser);
    expect(TK_LPAREN, parser->current, "(");
    parser_advance(parser);

    Token *arglist = NULL;
    /* Do the function have arguments? If so, store them in a
     * list of argument.
     * */
    if (parser->current.kind == TK_RPAREN) {
        parser_advance(parser);
        parser_skip_newline(parser);
    } else if (parser->current.kind == TK_ID) {
                arrput(arglist, parser->current);
        parser_advance(parser);

        while (parser->current.kind == TK_COMMA) {
            parser_advance(parser);
            expect(TK_ID, parser->current, "identifier");
                    arrput(arglist, parser->current);
            parser_advance(parser);
        }

        expect(TK_RPAREN, parser->current, ")");
        parser_advance(parser);
        parser_skip_newline(parser);
    } else {
        printf("Expected `)` or `identifier`.");
        exit(1);
    }

    expect(TK_LBRACE, parser->current, "{");
    parser_advance(parser);
    parser_skip_newline(parser);

    /* Is the function's body not empty (i.e., immediately encountering
     * right brace token)? */
    AST *body = NULL;
    if (parser->current.kind != TK_RBRACE) {
        /* If not, parse the function body. */
        body = parse_block(parser);
    }

    /* Finally, we expect closing brace. */
    expect(TK_RBRACE, parser->current, "} (function definition)");
    parser_advance(parser);
    return create_funcdef_node(func_name, arglist, body);
}

AST *parse_if_expr(Parser *parser) {
    parser_advance(parser);
    expect(TK_LPAREN, parser->current, "(");
    parser_advance(parser);

    AST *condition = parse_expr(parser);
    if (condition == NULL) {
        printf("Expected logical expression.\n");
        exit(1);
    }

    NodeList *conditions = calloc(1, sizeof(NodeList));
    node_list_init(conditions);

    /* Add the first condition */
    node_list_push(conditions, condition);

    expect(TK_RPAREN, parser->current, ")");
    parser_advance(parser);
    parser_skip_newline(parser);
    expect(TK_LBRACE, parser->current, "{");
    parser_advance(parser);
    parser_skip_newline(parser);


    /* Prepare the list to hold all possible cases */
    NodeList *cases = calloc(1, sizeof(NodeList));;
    node_list_init(cases);

    AST *block = NULL;
    /* if the block is not empty */
    if (parser->current.kind != TK_RBRACE) {
        /* Add the first case to the case list */
        block = parse_block(parser);
        if (block != NULL) {
            node_list_push(cases, block);
        }
    } else if (parser->current.kind == TK_RBRACE) {
        expect(TK_RBRACE, parser->current, "} (if expr)");
        parser_advance(parser);
    }
    parser_advance(parser);
    parser_skip_newline(parser);

    while (is_keyword(parser->current, "elif")) {
        parser_advance(parser);
        expect(TK_LPAREN, parser->current, "(");
        parser_advance(parser);

        condition = parse_expr(parser);

        expect(TK_RPAREN, parser->current, ")");
        parser_advance(parser);
        parser_skip_newline(parser);
        expect(TK_LBRACE, parser->current, "{");
        parser_advance(parser);
        parser_skip_newline(parser);

        block = parse_block(parser);

        node_list_push(conditions, condition);
        node_list_push(cases, block);

        expect(TK_RBRACE, parser->current, "} (elif if expr)");
        parser_advance(parser);
        parser_skip_newline(parser);
    }

    /*
     * Capture `else` block
     */
    AST *else_case = NULL;

    if (is_keyword(parser->current, "else")) {
        parser_advance(parser);
        parser_skip_newline(parser);
        expect(TK_LBRACE, parser->current, "{");
        parser_advance(parser);
        parser_skip_newline(parser);

        else_case = parse_block(parser);

        expect(TK_RBRACE, parser->current, "} (else expr)");
        parser_advance(parser);
    }

    AST *res = create_if_node(conditions->arr,
                              cases->arr,
                              else_case,
                              conditions->count);
    free(cases);
    free(conditions);

    return res;
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
