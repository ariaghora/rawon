#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

RwnObj *create_int_obj(Interpreter *interpreter, int val) {
    RwnObj *res = calloc(1, sizeof(RwnObj));
    res->intval = val;
    res->data_type = DT_INT;

    tracker_add_obj(interpreter, res);

    return res;
}

RwnObj *create_str_obj(Interpreter *interpreter, AST *node) {
    RwnObj *res = calloc(1, sizeof(RwnObj));
    res->strval = node->strval;
    res->strvallen = node->strvallen;
    res->data_type = DT_STR;

    tracker_add_obj(interpreter, res);
    return res;
}

RwnObj *visit(Interpreter *interpreter, AST *node) {
    if (node->node_type == NT_INT) return visit_int(interpreter, node);
    else if (node->node_type == NT_BIN_OP) return visit_binop(interpreter, node);
    else if (node->node_type == NT_STRING) return visit_str(interpreter, node);
    else if (node->node_type == NT_LIST) return visit_list(interpreter, node);
    else {
        printf("Interpreter error.");
        exit(1);
    }
}

RwnObj *visit_int(Interpreter *interpreter, AST *node) {
    return create_int_obj(interpreter, node->intval);
}

RwnObj *visit_str(Interpreter *interpreter, AST *node) {
    return create_str_obj(interpreter, node);
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

RwnObj *visit_list(Interpreter *interpreter, AST *node) {
    RwnObj *rwn_list_obj = calloc(1, sizeof(RwnObj));
    tracker_add_obj(interpreter, rwn_list_obj);

    rwn_list_obj->obj_list_cnt = node->node_list_cnt;
    rwn_list_obj->obj_list = calloc(rwn_list_obj->obj_list_cnt,
                                    sizeof(RwnObj *));
    rwn_list_obj->data_type = DT_LIST;

    for (int i = 0; i < node->node_list_cnt; ++i) {
        /*
         * We don't have to track `item`, since the return value of `visit`
         * has been tracked within the `visit` function
        /*/
        RwnObj *item = visit(interpreter, node->node_list[i]);
        rwn_list_obj->obj_list[i] = item;
    }

    return rwn_list_obj;
}

char *obj_get_repr(RwnObj *obj) {
    char s[255] = "";
    char *res;
    int len;

    if ((obj->data_type) == DT_INT) {
        len = sprintf(s, "%d", obj->intval);
        res = calloc(len + 1, sizeof(char));
        strcpy(res, s);
    } else if (obj->data_type == DT_FLOAT) {
        len = sprintf(s, "%f", obj->floatval);
        res = calloc(len + 1, sizeof(char));
        strcpy(res, s);
    } else if (obj->data_type == DT_STR) {
        len = obj->strvallen + 2;
        res = calloc(len + 1, sizeof(char));
        strcpy(res, "'");
        strcat(res, obj->strval);
        strcat(res, "'");
    } else if (obj->data_type == DT_LIST) {
        res = (char *) calloc(2, sizeof(char));
        strcat(res, "[");
        for (int i = 0; i < obj->obj_list_cnt; ++i) {
            char *item_repr = obj_get_repr(obj->obj_list[i]);

            /*
             * A space to put "," after each item
             */
            int extra_space = 2 * (i < obj->obj_list_cnt - 1);

            res = (char *) realloc(res, strlen(res) + strlen(item_repr) + 1 + extra_space);
            strcat(res, item_repr);
            if (extra_space) strcat(res, ", ");
            free(item_repr);
        }
        res = (char *) realloc(res, strlen(res) + 2);
        strcat(res, "]");
        return res;
    }
    return res;
}

void interpreter_init(Interpreter *interpreter) {
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

void free_rwn_obj(RwnObj *obj) {
    if (obj->obj_list_cnt > 0) {
        for (int i = 0; i < obj->obj_list_cnt; ++i) {
            free(obj->obj_list[i]);
        }
    }
}

void interpreter_cleanup(Interpreter *interpreter) {
    for (int i = 0; i < interpreter->obj_cnt; i++) {
        /*
         * if current object is a list object, then free
         * its obj_list.
         */
        if (interpreter->tracker->objs[i]->obj_list_cnt > 0) {
            free(interpreter->tracker->objs[i]->obj_list);
        }

        free(interpreter->tracker->objs[i]);
    }

    free(interpreter->tracker);
    free(interpreter);
}

void free_AST(AST *node) {
    /*
     * Unary & binary operation AST node cleanup
     */
    if (node->right != NULL) free_AST(node->right);
    if (node->left != NULL) free_AST(node->left);

    /*
     * String AST node cleanup
     */
    free(node->strval);

    /*
     * Cleaning items if it is a list AST node
     */
    if (node->node_list_cnt > 0) {
        for (int i = 0; i < node->node_list_cnt; ++i) {
            free_AST(node->node_list[i]);
        }
        free(node->node_list);
    }

    free(node);
}
