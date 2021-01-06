#ifndef CRAWON_FILE_H
#define CRAWON_FILE_H

#include "../lexer.h"
#include "../parser.h"
#include "../interpreter.h"

RwnObj *rwn_readtext(Interpreter *context, RwnObj **args);

#endif
