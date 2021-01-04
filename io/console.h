#ifndef CRAWON_CONSOLE_H
#define CRAWON_CONSOLE_H

#include "../lexer.h"
#include "../parser.h"
#include "../interpreter.h"

RwnObj *rwn_input(Interpreter *context, RwnObj **args);
RwnObj *rwn_print(Interpreter *context, RwnObj **args);
RwnObj *rwn_toint(Interpreter *context, RwnObj **args);

#endif //CRAWON_CONSOLE_H
