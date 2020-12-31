#ifndef CRAWON_INTERPRETER_H
#define CRAWON_INTERPRETER_H

#define MAX_RWN_OBJ 1000

typedef enum {
    DT_INT, DT_FLOAT, DT_BOOL, DT_STR, DT_NULL, DT_LIST
} data_type_t;

typedef struct tRwnObj {
    int intval;
    float floatval;

    char *strval;
    int strvallen;

    struct tRwnObj **obj_list;
    int obj_list_cnt;

    data_type_t data_type;
} RwnObj;

typedef struct {
    RwnObj *objs[MAX_RWN_OBJ];
} RwnObjectTracker;

typedef struct {
    RwnObjectTracker *tracker;
    int obj_cnt;
} Interpreter;

RwnObj *create_number_obj(Interpreter *interpreter,
                          float val,
                          data_type_t data_type);

RwnObj *create_bool_obj(Interpreter *interpreter, int val);

RwnObj *create_str_obj(Interpreter *interpreter, char *val);

RwnObj *create_null_obj(Interpreter *interpreter);

RwnObj *interpreter_traverse(Interpreter *interpreter, AST *node);

RwnObj *visit(Interpreter *interpreter, AST *node);

RwnObj *visit_binop(Interpreter *interpreter, AST *node);

RwnObj *visit_float(Interpreter *interpreter, AST *node);

RwnObj *visit_list(Interpreter *interpreter, AST *node);

RwnObj *visit_if(Interpreter *interpreter, AST *node);

RwnObj *visit_int(Interpreter *interpreter, AST *node);

RwnObj *visit_str(Interpreter *interpreter, AST *node);

char *obj_get_repr(RwnObj *obj);

char *obj_typestr(RwnObj *obj);

void interpreter_init(Interpreter *interpreter);

void tracker_add_obj(Interpreter *interpreter, RwnObj *obj);

void tracker_remove_obj(Interpreter *interpreter, RwnObj *obj);

void free_rwn_obj(RwnObj *obj);

void interpreter_cleanup(Interpreter *interpreter);

void free_AST(AST *node);


#endif //CRAWON_INTERPRETER_H
