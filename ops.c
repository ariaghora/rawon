#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"
#include "ops.h"

RwnObj *op_add(Interpreter *interpreter, RwnObj *a, RwnObj *b) {
    if (a->data_type == DT_INT && b->data_type == DT_INT) {
        /* case 1: both operands are int */
        return create_number_obj(interpreter,
                                 a->intval + b->intval,
                                 DT_INT);
    } else if (a->data_type == DT_FLOAT && b->data_type == DT_FLOAT) {
        /* case 2: both operands are float */
        return create_number_obj(interpreter,
                                 a->floatval + b->floatval,
                                 DT_FLOAT);
    } else if (a->data_type == DT_INT && b->data_type == DT_FLOAT) {
        /* case 3: either one is a float; then typecast to float. */
        return create_number_obj(interpreter,
                                 a->intval + b->floatval,
                                 DT_FLOAT);
    } else {
        /* same. */
        return create_number_obj(interpreter,
                                 a->floatval + b->intval,
                                 DT_FLOAT);
    }
}

RwnObj *op_sub(Interpreter *interpreter, RwnObj *a, RwnObj *b) {
    if (a->data_type == DT_INT && b->data_type == DT_INT) {
        return create_number_obj(interpreter,
                                 a->intval - b->intval,
                                 DT_INT);
    } else if (a->data_type == DT_FLOAT && b->data_type == DT_FLOAT) {
        return create_number_obj(interpreter,
                                 a->floatval - b->floatval,
                                 DT_FLOAT);
    } else if (a->data_type == DT_INT && b->data_type == DT_FLOAT) {
        return create_number_obj(interpreter,
                                 a->intval - b->floatval,
                                 DT_FLOAT);
    } else {
        /* same. */
        return create_number_obj(interpreter,
                                 a->floatval - b->intval,
                                 DT_FLOAT);
    }
}

void rt_error(char *msg) {
    printf("Run-time error: %s\n", msg);
    exit(1);
}

RwnObj *op_mul(Interpreter *interpreter, RwnObj *a, RwnObj *b) {
    if (a->data_type == DT_INT && b->data_type == DT_INT) {
        return create_number_obj(interpreter,
                                 a->intval * b->intval,
                                 DT_INT);
    } else if (a->data_type == DT_FLOAT && b->data_type == DT_FLOAT) {
        return create_number_obj(interpreter,
                                 a->floatval * b->floatval,
                                 DT_FLOAT);
    } else if (a->data_type == DT_INT && b->data_type == DT_FLOAT) {
        return create_number_obj(interpreter,
                                 (float) a->intval * b->floatval,
                                 DT_FLOAT);
    } else if (a->data_type == DT_FLOAT && b->data_type == DT_INT) {

        return create_number_obj(interpreter,
                                 a->floatval * (float) b->intval,
                                 DT_FLOAT);
    } else if (a->data_type == DT_INT && b->data_type == DT_STR) {
        /* Handle int and string multiplication.
         * It is equivalent to duplicate string `b`, `a` times.
         */
        int outlen = a->intval * strlen(b->strval) + 1;
        char *res = calloc(outlen, sizeof(char));
        for (int i = 0; i < a->intval; ++i) {
            res = strcat(res, b->strval);
        }

        RwnObj *o = create_str_obj(interpreter, res);

        return o;

    } else if (a->data_type == DT_STR && b->data_type == DT_INT) {
        /*
         * String multiplication, reversed order
         */
        return op_mul(interpreter, b, a);
    }else {
        char *msg = malloc(255);
        sprintf(msg,
                "Operator * is not defined for `%s` and `%s` data types.",
                obj_typestr(a),
                obj_typestr(b));

        rt_error(msg);
    }

    return create_null_obj(interpreter);
}

RwnObj *op_ee(Interpreter *interpreter, RwnObj *a, RwnObj *b) {
    if (a->data_type == DT_INT && b->data_type == DT_INT) {
        return create_bool_obj(interpreter,
                               a->intval == b->intval);
    } else if (a->data_type == DT_FLOAT && b->data_type == DT_FLOAT) {
        return create_bool_obj(interpreter,
                               a->floatval == b->floatval);
    } else if (a->data_type == DT_INT && b->data_type == DT_FLOAT) {
        return create_bool_obj(interpreter,
                               a->intval == b->floatval);
    } else {
        return create_bool_obj(interpreter,
                               a->floatval == b->intval);
    }
}

RwnObj *op_gt(Interpreter *interpreter, RwnObj *a, RwnObj *b) {
    if (a->data_type == DT_INT && b->data_type == DT_INT) {
        return create_bool_obj(interpreter,
                               a->intval > b->intval);
    } else if (a->data_type == DT_FLOAT && b->data_type == DT_FLOAT) {
        return create_bool_obj(interpreter,
                               a->floatval == b->floatval);
    } else if (a->data_type == DT_INT && b->data_type == DT_FLOAT) {
        return create_bool_obj(interpreter,
                               a->intval == b->floatval);
    } else {
        return create_bool_obj(interpreter,
                               a->floatval == b->intval);
    }
}