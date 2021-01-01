#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"
#include "ops.h"

RwnObj *create_number_obj(Interpreter *interpreter,
                          float val,
                          data_type_t data_type) {
    RwnObj *res = calloc(1, sizeof(RwnObj));
    if (data_type == DT_INT)
        res->intval = val;
    else
        res->floatval = val;
    res->data_type = data_type;

    tracker_add_obj(interpreter, res);
    return res;
}

RwnObj *create_bool_obj(Interpreter *interpreter, int val) {
    RwnObj *res = calloc(1, sizeof(RwnObj));
    res->intval = val;
    res->data_type = DT_BOOL;
    tracker_add_obj(interpreter, res);

    return res;
}

RwnObj *create_str_obj(Interpreter *interpreter, char *val) {
    RwnObj *res = calloc(1, sizeof(RwnObj));
    res->strval = val;
    res->strvallen = strlen(val);
    res->data_type = DT_STR;

    tracker_add_obj(interpreter, res);
    return res;
}

RwnObj *create_null_obj(Interpreter *interpreter) {
    RwnObj *res = calloc(1, sizeof(RwnObj));
    res->data_type = DT_NULL;

    tracker_add_obj(interpreter, res);
    return res;
}

RwnObj *create_func(Interpreter *interpreter, char *name) {
    RwnObj *res = calloc(1, sizeof(RwnObj));
    res->data_type = DT_FUNC;
    res->funcname = name;

    tracker_add_obj(interpreter, res);
    return res;
}

RwnObj *interpreter_traverse(Interpreter *interpreter, AST *node) {
    return visit(interpreter, node);
}

RwnObj *visit(Interpreter *interpreter, AST *node) {
    if (node->node_type == NT_INT)
        return visit_int(interpreter, node);
    else if (node->node_type == NT_FLOAT)
        return visit_float(interpreter, node);
    else if (node->node_type == NT_BIN_OP)
        return visit_binop(interpreter, node);
    else if (node->node_type == NT_STRING)
        return visit_str(interpreter, node);
    else if (node->node_type == NT_LIST)
        return visit_list(interpreter, node);
    else if (node->node_type == NT_IF)
        return visit_if(interpreter, node);
    else if (node->node_type == NT_VARACCESS)
        return visit_varaccess(interpreter, node);
    else if (node->node_type == NT_VARASSIGN)
        return visit_varassign(interpreter, node);
    else if (node->node_type == NT_FUNC_DEF)
        return visit_funcdef(interpreter, node);
    else {
        printf("Interpreter error.\n");
        exit(1);
    }
}

RwnObj *visit_float(Interpreter *interpreter, AST *node) {
    return create_number_obj(interpreter, node->floatval, TK_FLOAT);
}

RwnObj *visit_funcdef(Interpreter *interpreter, AST *node) {
    RwnObj *res = create_func(interpreter, node->fn_name.txt);
    shput(interpreter->symbol_table, node->fn_name.txt, res);
    return res;
}

RwnObj *visit_if(Interpreter *interpreter, AST *node) {
    for (int i = 0; i < node->conditions_cnt; ++i) {
        if (node->if_conditions[i]->intval) {
            visit(interpreter, node->if_cases[i]);
            break;
        }
    }

    if (node->else_case != NULL) {
        visit(interpreter, node->else_case);
    }
    return create_null_obj(interpreter);
}

RwnObj *visit_int(Interpreter *interpreter, AST *node) {
    return create_number_obj(interpreter, node->intval, TK_INT);
}

RwnObj *visit_str(Interpreter *interpreter, AST *node) {
    return create_str_obj(interpreter, node->strval);
}

RwnObj *visit_binop(Interpreter *interpreter, AST *node) {
    RwnObj *left = visit(interpreter, node->left);
    RwnObj *right = visit(interpreter, node->right);
    Token op = node->op;

    RwnObj *res;
    if (op.kind == TK_PLUS) {
        res = op_add(interpreter, left, right);
    } else if (op.kind == TK_MINUS) {
        res = op_sub(interpreter, left, right);
    } else if (op.kind == TK_MULT) {
        res = op_mul(interpreter, left, right);
    } else if (op.kind == TK_EE) {
        res = op_ee(interpreter, left, right);
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

RwnObj *visit_varaccess(Interpreter *interpreter, AST *node) {
    RwnObj *res = shget(interpreter->symbol_table, node->var_token.txt);
    if (res == NULL) {
        printf("Error: Identifier `%s` not found.\n", node->var_token.txt);
        exit(1);
    }
    return res;
}

RwnObj *visit_varassign(Interpreter *interpreter, AST *node) {
    RwnObj *res = visit(interpreter, node->var_node);
    shput(interpreter->symbol_table, node->var_token.txt, res);
    return res;
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
        len = sprintf(s, "%.2f", obj->floatval);
        res = calloc(len + 1, sizeof(char));
        strcpy(res, s);
    } else if (obj->data_type == DT_BOOL) {
        char *boolstr = obj->intval == 1 ? "true" : "false";
        res = calloc(strlen(boolstr) + 1, sizeof(char));
        strcpy(res, boolstr);
    } else if (obj->data_type == DT_NULL) {
        char *nullstr = "null";
        res = calloc(strlen(nullstr) + 1, sizeof(char));
        strcpy(res, nullstr);
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
    } else if (obj->data_type == DT_FUNC) {
        char *pre = "<function ";
        char *funcname = obj->funcname;
        char *post = ">";
        res = calloc(strlen(pre) + strlen(funcname) + strlen(post) + 1,
                     sizeof(char));
        strcat(res, pre);
        strcat(res, funcname);
        strcat(res, post);
        res[strlen(res)] = '\0';
    }
    return res;
}

char *obj_typestr(RwnObj *obj) {
    char *res = malloc(20 * sizeof(char));

    switch (obj->data_type) {
        case DT_INT:
            strcpy(res, "int");
            break;
        case DT_FLOAT:
            strcpy(res, "float");
            break;
        case DT_STR:
            strcpy(res, "str");
            break;
        case DT_NULL:
            strcpy(res, "null");
            break;
        default:
            strcpy(res, "unknown");
    }
    return res;
}

void interpreter_init(Interpreter *interpreter) {
    interpreter->tracker = calloc(1, sizeof(RwnObjectTracker));
    interpreter->parent = NULL;
    interpreter->symbol_table = NULL;
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
        /*
         * if current object is a list object, then free
         * its obj_list.
         */
        if (interpreter->tracker->objs[i]->obj_list_cnt > 0) {
            free(interpreter->tracker->objs[i]->obj_list);
        }

        if (interpreter->tracker->objs[i]->strval != NULL) {
        }

        if (interpreter->tracker->objs[i]->strval != NULL) {
            free(interpreter->tracker->objs[i]->strval);
        }
        free(interpreter->tracker->objs[i]);
    }

            hmfree(interpreter->symbol_table);
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
     * (IDK. Maybe unnecessary? )
     */

    /*
     * varaccess/assign cleanup
     */
    if (node->node_type == NT_VARACCESS || node->node_type == NT_VARASSIGN) {
        if (node->var_node != NULL)
            free_AST(node->var_node);
    }

    /*
     * If-expr node cleanup
     */
    if (node->node_type == NT_IF) {
        for (int i = 0; i < node->conditions_cnt; ++i) {
            if (node->if_cases[i] != NULL)
                free_AST(node->if_cases[i]);

            /* free is enough, since if conditions are just a
             * single expression */
            free_AST(node->if_conditions[i]);
        }

        if (node->else_case != NULL)
            free_AST(node->else_case);

        if (node->if_conditions != NULL)
            free(node->if_conditions);
        if (node->if_cases != NULL)
            free(node->if_cases);
    }

    /* Funcdef node cleanup */
    if (node->node_type == NT_FUNC_DEF) {
        if (node->fn_body != NULL) {
            free_AST(node->fn_body);
        }

        if (node->fn_arglist != NULL) {
                    arrfree(node->fn_arglist);
        }
    }

    /* Cleaning items if it is a `list` AST node */
    if (node->node_list_cnt > 0) {
        for (int i = 0; i < node->node_list_cnt; ++i) {
            free_AST(node->node_list[i]);
        }
        if (node->node_list != NULL)
            free(node->node_list);
    }

    free(node);
}