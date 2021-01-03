#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define STB_DS_IMPLEMENTATION

#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

RwnObj *print(Interpreter *context, RwnObj **args) {
    for (int i = 0; i < arrlen(args); ++i) {
        printf("%s", obj_get_repr(args[i]));
    }
    return create_null_obj(context);
}

int main(int argc, char *argv[]) {

    char *fn = argv[1];

    Lexer *lexer = calloc(1, sizeof(Lexer));
    lexer_init(lexer, fn);
    lexer_lex(lexer);

    Parser *parser = calloc(1, sizeof(Parser));
    parser_init(parser, lexer);

    AST *root = parser_parse(parser);

    //--- Interpreter start
    Interpreter *interpreter = calloc(1, sizeof(Interpreter));
    interpreter_init(interpreter);

    RwnObj *f = create_builtin_func(interpreter, "print", print, NULL);
    shput(interpreter->symbol_table, "print", f);

    RwnObj *result = interpreter_traverse(interpreter, root);
    char *result_repr = obj_get_repr(result);

    interpreter_cleanup(interpreter);
    //--- Interpreter end

    free(result_repr);
    free_AST(root);
    free(parser);
    free(lexer);

    return 0;
}
