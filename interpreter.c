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

//    tracker_add_obj(interpreter, res);

    return res;
}

RwnObj *visit_list(Interpreter *interpreter, AST *node) {
    RwnObj *rwn_list_obj = calloc(1, sizeof(RwnObj));
    tracker_add_obj(interpreter, rwn_list_obj);

    strcat(rwn_list_obj->repr, "[");
    for (int i = 0; i < node->node_list_cnt; ++i) {
        RwnObj *item = visit(interpreter, node->node_list[i]);
        // We don't have to track `item`, since the output of above `visit`
        // has been tracked
        // tracker_add_obj(interpreter, item);

        strcat(rwn_list_obj->repr, item->repr);
        if (i < node->node_list_cnt - 1)
            strcat(rwn_list_obj->repr, ", ");
    }
    strcat(rwn_list_obj->repr, "]");

    return rwn_list_obj;
}

RwnObj *visit(Interpreter *interpreter, AST *node) {
    if (node->node_type == NT_INT) return visit_int(interpreter, node);
    else if (node->node_type == NT_BIN_OP) return visit_binop(interpreter, node);
    else if (node->node_type == NT_LIST) return visit_list(interpreter, node);
    else {
        printf("Interpreter error.");
        exit(1);
    }
}

void free_AST(AST *node) {
    /*
     * Unary & binary operation AST node cleanup
     */
    if (node->right != NULL) free_AST(node->right);
    if (node->left != NULL) free_AST(node->left);

    /*
     * Cleaning items if it is a list AST node
     */
    if (node->node_list_cnt > 0) {
        for (int i = 0; i < node->node_list_cnt; ++i) {
            free_AST(node->node_list[i]);
        }
    }

    free(node->node_list);
    free(node);
}
