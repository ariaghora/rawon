#ifndef CRAWON_PARSER_H
#define CRAWON_PARSER_H


typedef enum {
    NT_INT, NT_FLOAT, NT_STRING, NT_LIST, NT_BIN_OP, NT_UN_OP, NT_FUNC_DEF, NT_FUNC_CALL
} node_type_t;

typedef struct tAST {
    node_type_t node_type;
    int intval;
    float floatval;

    char *strval;
    int strvallen;

    /*
     * For unary operation node, `left` is used as the operand
     */
    struct tAST *left;
    struct tAST *right;
    Token op;

    /*
     * A list node contains a node list
     */
    struct tAST **node_list;
    int node_list_cnt;
} AST;

typedef struct {
    int max_cap;
    int count;
    AST **arr;
} NodeList;

typedef struct {
    int idx;
    Token current;
    Token *tokens;
} Parser;

void node_list_init(NodeList *node_list);

void node_list_push(NodeList *node_list, AST *item);

void node_list_free(NodeList *node_list);

void parser_advance(Parser *parser);

void parser_skip_newline(Parser *parser);

void parser_init(Parser *parser, Lexer *lexer);

/*
 * Initializer functions
 */
AST *create_number_node(Token tok);

AST *create_string_node(Token tok);

AST *create_bin_op(AST *left, AST *right, Token op);

AST *create_list_node(AST **node_list, int node_list_cnt);

/*
 * Parsing functions
 */

AST *parser_parse(Parser *parser);

AST *parse_arith(Parser *parser);

AST *parse_atom(Parser *parser);

AST *parse_block(Parser *parser);

AST *parse_call(Parser *parser);

AST *parse_comp_expr(Parser *parser);

AST *parse_expr(Parser *parser);

AST *parse_factor(Parser *parser);

AST *parse_list(Parser *parser);

AST *parse_power(Parser *parser);

AST *parse_statement(Parser *parser);

AST *parse_term(Parser *parser);


#endif //CRAWON_PARSER_H
