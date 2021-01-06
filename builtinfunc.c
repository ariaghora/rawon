#include "io/console.h"
#include "io/file.h"
#include "strutils/strutils.h"
#include "builtinfunc.h"

void init_builtin_funcs(Interpreter *context) {
    register_builtin_func(context, "input", rwn_input);
    register_builtin_func(context, "print", rwn_print);
    register_builtin_func(context, "toint", rwn_toint);

    /* strutils */
    register_builtin_func(context, "split", rwn_split);

    /* file */
    register_builtin_func(context, "readtext", rwn_readtext);
}