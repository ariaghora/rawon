#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lexer.h"
#include "parser.h"
#include "interpreter.h"


int main(int argc, char *argv[]) {

    char *fn = argv[1];

    Lexer *lexer = calloc(1, sizeof(Lexer));
    lexer_init(lexer, fn);
    lexer_lex(lexer);

    Parser *parser = calloc(1, sizeof(Parser));
    parser_init(parser, lexer);

    AST *root = parse_arith(parser);

    //--- Interpreter start
    Interpreter *interpreter = calloc(1, sizeof(Interpreter));
    interpreter_init(interpreter);

    RwnObj *result = visit(interpreter, root);
    printf("%s\n", result->repr);

    interpreter_cleanup(interpreter);
    //--- Interpreter end

    free_AST(root);
    free(parser);
    free(lexer);

    return 0;
}
