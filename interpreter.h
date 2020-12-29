#ifndef CRAWON_INTERPRETER_H
#define CRAWON_INTERPRETER_H

#define MAX_RWN_OBJ 1000

typedef struct tRwnObj {
    int intval;
    float floatval;
    struct tRwnObj **obj_list;
    char repr[255];
} RwnObj;

typedef struct {
    RwnObj *objs[MAX_RWN_OBJ];
} RwnObjectTracker;

typedef struct {
    RwnObjectTracker *tracker;
    int obj_cnt;
} Interpreter;


RwnObj *create_int_obj(Interpreter *interpreter, int val);

RwnObj *visit(Interpreter *interpreter, AST *node);

RwnObj *visit_int(Interpreter *interpreter, AST *node);

RwnObj *visit_binop(Interpreter *interpreter, AST *node);

void interpreter_init(Interpreter *interpreter);

void interpreter_cleanup(Interpreter *interpreter);

void free_AST(AST *node);


#endif //CRAWON_INTERPRETER_H
