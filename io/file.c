#include <stdio.h>
#include "file.h"

char *load_file(char const *path) {
    char *buffer = 0;
    long length;
    FILE *f = fopen(path, "r");

    if (f) {
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);
        buffer = (char *) malloc((length + 1) * sizeof(char));
        if (buffer) {
            fread(buffer, sizeof(char), length, f);
        }
        fclose(f);
    }
    buffer[length] = '\0';

    return buffer;
}

RwnObj *rwn_readtext(Interpreter *context, RwnObj **args) {
    ensure_n_params("readtext", 1, arrlen(args));
    if (args[0]->data_type != DT_STR)
        rt_error("readtext only accepts filename string argument");

    char *res = load_file(args[0]->strval);
    return create_str_obj(context, res);
}