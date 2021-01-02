#ifndef CRAWON_INTERPRETER_H
#define CRAWON_INTERPRETER_H

#define MAX_RWN_OBJ 10000

#include "3rd_party/stb_ds.h"

typedef enum {
    DT_INT, DT_FLOAT, DT_BOOL, DT_STR, DT_NULL, DT_LIST,
    DT_FUNC
} data_type_t;

struct tInterpreter;

typedef struct tRwnObj {
    int intval;
    float floatval;

    char *strval;
    int strvallen;

    /* list object properties */
    struct tRwnObj **obj_list;
    int obj_list_cnt;

    /* func object properties */
    char *funcname;
    char **funcargnames;
    AST *funcbody;

    /* builtin func object properties */
    int is_builtin;

    struct tRwnObj *(*builtin_func)(struct tInterpreter *context,
                                    struct tRwnObj **args);

    data_type_t data_type;
} RwnObj;

typedef struct {
    RwnObj *objs[MAX_RWN_OBJ];
} RwnObjectTracker;

typedef struct {
    char *key;
    RwnObj *value;
} ObjMap;

typedef struct tInterpreter {
    struct tInterpreter *parent;

    /* To store symbols, i.e., `varname`-`value` pairs */
    ObjMap *symbol_table;

    /* To track the created objects */
    RwnObjectTracker *tracker;
    int obj_cnt;
} Interpreter;

RwnObj *create_number_obj(Interpreter *interpreter,
                          float val,
                          data_type_t data_type);

RwnObj *create_bool_obj(Interpreter *interpreter, int val);

RwnObj *create_builtin_func(Interpreter *interpreter,
                            char *funcname,
                            RwnObj *(*func)(Interpreter *context,
                                           RwnObj **args),
                            RwnObj **args);

RwnObj *create_str_obj(Interpreter *interpreter, char *val);

RwnObj *create_null_obj(Interpreter *interpreter);

RwnObj *create_func(Interpreter *interpreter, char *name,
                    char **argnames,
                    AST *body);

RwnObj *interpreter_traverse(Interpreter *interpreter, AST *node);

RwnObj *visit(Interpreter *interpreter, AST *node);

RwnObj *visit_binop(Interpreter *interpreter, AST *node);

RwnObj *visit_builtin_funccall(Interpreter *interpreter, AST *node);

RwnObj *visit_float(Interpreter *interpreter, AST *node);

RwnObj *visit_funccall(Interpreter *interpreter, AST *node);

RwnObj *visit_funcdef(Interpreter *interpreter, AST *node);

RwnObj *visit_list(Interpreter *interpreter, AST *node);

RwnObj *visit_if(Interpreter *interpreter, AST *node);

RwnObj *visit_int(Interpreter *interpreter, AST *node);

RwnObj *visit_str(Interpreter *interpreter, AST *node);

RwnObj *visit_varaccess(Interpreter *interpreter, AST *node);

RwnObj *visit_varassign(Interpreter *interpreter, AST *node);

char *obj_get_repr(RwnObj *obj);

char *obj_typestr(RwnObj *obj);

void interpreter_init(Interpreter *interpreter);

void tracker_add_obj(Interpreter *interpreter, RwnObj *obj);

void tracker_remove_obj(Interpreter *interpreter, RwnObj *obj);

void free_rwn_obj(RwnObj *obj);

void interpreter_cleanup(Interpreter *interpreter);

void free_AST(AST *node);


#endif //CRAWON_INTERPRETER_H
