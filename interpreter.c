#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

void interpreter_init(Interpreter *interpreter) {
//    interpreter = calloc(1, sizeof(Interpreter));
    interpreter->tracker = calloc(1, sizeof(RwnObjectTracker));
}

void tracker_add_obj(Interpreter *interpreter, RwnObj *obj) {
    interpreter->tracker->objs[interpreter->obj_cnt] = obj;
    interpreter->obj_cnt++;
}

void tracker_remove_obj(Interpreter *interpreter, RwnObj *obj) {
    for (int i = 0; i < interpreter->obj_cnt; ++i) {
        if (obj == interpreter->tracker->objs[i]) {
            printf("SAME\n");
        }
    }
}

void interpreter_cleanup(Interpreter *interpreter) {
    for (int i = 0; i < interpreter->obj_cnt; i++) {
        if (interpreter->tracker->objs[i] != NULL) {
            free(interpreter->tracker->objs[i]);
        }
    }
    free(interpreter->tracker);
    free(interpreter);
}

RwnObj *create_int_obj(Interpreter *interpreter, int val) {
    RwnObj *res = calloc(1, sizeof(RwnObj));
    res->intval = val;
    sprintf(res->repr, "%d", val);

    tracker_add_obj(interpreter, res);

    return res;
}

RwnObj *visit_int(Interpreter *interpreter, AST *node) {
    return create_int_obj(interpreter, node->intval);
}

RwnObj *visit_binop(Interpreter *interpreter, AST *node) {
    RwnObj *left = visit(interpreter, node->left);
    RwnObj *right = visit(interpreter, node->right);
    Token op = node->op;

    RwnObj *res;
    if (op.kind == TK_PLUS) {
        res = create_int_obj(interpreter, left->intval + right->intval);
    } else if (op.kind == TK_MULT) {
        res = create_int_obj(interpreter, left->intval * right->intval);
    }

    return res;
}

RwnObj *visit(Interpreter *interpreter, AST *node) {
    if (node->node_type == NT_INT) return visit_int(interpreter, node);
    else if (node->node_type == NT_BIN_OP) return visit_binop(interpreter, node);
    else {
        printf("Interpreter error.");
        exit(1);
    }
}

void free_AST(AST *node) {
    if (node->right != NULL) free_AST(node->right);
    if (node->left != NULL) free_AST(node->left);
    free(node);
}
