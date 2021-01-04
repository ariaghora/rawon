#include "io/console.h"
#include "builtinfunc.h"

void init_builtin_funcs(Interpreter *context) {
    register_builtin_func(context, "input", rwn_input);
    register_builtin_func(context, "print", rwn_print);
    register_builtin_func(context, "toint", rwn_toint);
}