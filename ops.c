//
// Created by ghora on 20. 12. 31..
//

#include "lexer.h"
#include "parser.h"
#include "interpreter.h"
#include "ops.h"

RwnObj *number_add(Interpreter *interpreter, RwnObj *a, RwnObj *b) {
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

RwnObj *number_mul(Interpreter *interpreter, RwnObj *a, RwnObj *b) {
    if (a->data_type == DT_INT && b->data_type == DT_INT) {
        /* case 1: both operands are int */
        return create_number_obj(interpreter,
                                 a->intval * b->intval,
                                 DT_INT);
    } else if (a->data_type == DT_FLOAT && b->data_type == DT_FLOAT) {
        /* case 2: both operands are float */
        return create_number_obj(interpreter,
                                 a->floatval * b->floatval,
                                 DT_FLOAT);
    } else if (a->data_type == DT_INT && b->data_type == DT_FLOAT) {
        /* case 3: either one is a float; then typecast to float. */
        return create_number_obj(interpreter,
                                 (float) a->intval * b->floatval,
                                 DT_FLOAT);
    } else {
        /* same. */
        return create_number_obj(interpreter,
                                 a->floatval * (float) b->intval,
                                 DT_FLOAT);
    }
}