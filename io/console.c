#include <stdio.h>
#include <errno.h>
#include "console.h"

void ensure_n_params(char *funcname, int expected, int got) {
    if (got != expected) {
        printf("Error: Function `%s` accepts %d parameters, but %d given.\n",
               funcname,
               expected,
               got);
        exit(1);
    }
}

RwnObj *rwn_input(Interpreter *context, RwnObj **args) {
    ensure_n_params("input", 0, arrlen(args));
    size_t len = 0;
    char *s = NULL;
    getline(&s, &len, stdin);
    return create_str_obj(context, s);
}

RwnObj *rwn_print(Interpreter *context, RwnObj **args) {
    for (int i = 0; i < arrlen(args); ++i) {
        char *res = obj_get_repr(args[i]);
        printf("%s", res);
        free(res);
    }
    return RwnNull(context);
}

RwnObj *rwn_toint(Interpreter *context, RwnObj **args) {
    ensure_n_params("input", 1, arrlen(args));

    char *endptr;
    errno = 0;
    int res = strtol(args[0]->strval, &endptr, 10);

    if (errno != 0) {
        puts("Numerical result of range.");
        exit(1);
    }
    if (endptr == args[0]->strval) {
        puts("Cannot convert empty or invalid string to `int`.");
        exit(1);
    }

    return create_number_obj(context, res, DT_INT);
}