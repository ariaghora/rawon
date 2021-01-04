#ifndef CRAWON_PARSER_H
#define CRAWON_PARSER_H


typedef enum {
    NT_INT, NT_FLOAT, NT_STRING, NT_LIST,
    NT_BIN_OP, NT_UN_OP,
    NT_IF,
    NT_FUNC_DEF,
    NT_FUNC_CALL,
    NT_BUILTIN_FUNC_CALL, /* distinction from run-time created function */
    NT_RETURN_NODE,
    NT_VARACCESS, NT_VARASSIGN,
} node_type_t;

typedef struct tAST {
    node_type_t node_type;
    int intval;
    float floatval;

    char *strval;
    int strvallen;

    /* For unary operation node, `left` is used as the operand */
    struct tAST *left;
    struct tAST *right;
    Token op;

    /* A list node contains a node list */
    struct tAST **node_list;
    int node_list_cnt;

    /* Varassign node contains a token of variable name
     * and the corresponding value node
     */
    Token var_token;
    struct tAST *var_node;

    /* An `if` node contains a list of conditions, a corresponding list of
     * cases, and an `else` case
     */
    struct tAST **if_conditions;
    struct tAST **if_cases;
    struct tAST *else_case;
    int conditions_cnt;

    /* A function definition node contains an argument list and a function
     * body node
     */
    Token fn_name;
    Token *fn_arglist;
    struct tAST *fn_body;

    /* A function call node contains a node to call and an argument list.
     */
    struct tAST **fcall_arglsit;
    struct tAST *fcall_node_to_call;

    /* A return node containing the returned node */
    struct tAST *returned_node;

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

AST *create_funccall_node(AST *node_to_call, AST **arglist);

AST *create_funcdef_node(Token name, Token *arglist, AST *body);

AST *create_list_node(AST **node_list, int node_list_cnt);

AST *create_if_node(AST **conditions, AST **cases, AST *else_case,
                    int conditions_cnt);

AST *create_varaccess_node(Token var_token);

AST *create_varassign_node(Token var_token, AST *node);

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

AST *parse_funcdef(Parser *parser);

AST *parse_if_expr(Parser *parser);

AST *parse_list(Parser *parser);

AST *parse_power(Parser *parser);

AST *parse_statement(Parser *parser);

AST *parse_term(Parser *parser);

#endif //CRAWON_PARSER_H
