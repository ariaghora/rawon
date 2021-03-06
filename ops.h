#ifndef CRAWON_OPS_H
#define CRAWON_OPS_H

RwnObj *op_add(Interpreter *interpreter, RwnObj *a, RwnObj *b);

RwnObj *op_sub(Interpreter *interpreter, RwnObj *a, RwnObj *b);

RwnObj *op_mul(Interpreter *interpreter, RwnObj *a, RwnObj *b);

RwnObj *op_ee(Interpreter *interpreter, RwnObj *a, RwnObj *b);

RwnObj *op_lt(Interpreter *interpreter, RwnObj *a, RwnObj *b);

RwnObj *op_pipe(Interpreter *interpreter, RwnObj *a, RwnObj *b);

#endif
