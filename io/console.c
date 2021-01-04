#include <stdio.h>
#include "console.h"

RwnObj *rwn_print(Interpreter *context, RwnObj **args) {
    for (int i = 0; i < arrlen(args); ++i) {
        char *res = obj_get_repr(args[i]);
        printf("%s", res);
        free(res);
    }
    return RwnNull(context);
}