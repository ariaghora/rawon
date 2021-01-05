#include <string.h>
#include <stdio.h>
#include "../3rd_party/stb_ds.h"
#include "strutils.h"

RwnObj *rwn_split(Interpreter *context, RwnObj **args) {
    ensure_n_params("split", 2, arrlen(args));

    if (args[0]->data_type != DT_STR)
        rt_error("First parameter must be a string.");

    if (args[1]->data_type != DT_STR)
        rt_error("Second parameter must be a string.");

    char *ch = NULL;
    char *str = args[0]->strval;
    char *delim = args[1]->strval;

    RwnObj *rwn_list_obj = calloc(1, sizeof(RwnObj));
    tracker_add_obj(context, rwn_list_obj);

    /* Ugly. Really. I hope I can integrate this with stb arr */
    RwnObj **objlist = calloc(1, sizeof(RwnObj *));
    ch = strtok(str, delim);
    int cnt = 0;
    while (ch != NULL) {
        char *s = calloc(1, strlen(ch) + 1);
        strcpy(s, ch);
        objlist = realloc(objlist, (cnt + 1) * sizeof(RwnObj *));
        objlist[cnt] = create_str_obj(context, s);
        cnt++;
        ch = strtok(NULL, delim);
    }
    rwn_list_obj->data_type = DT_LIST;
    rwn_list_obj->obj_list = objlist;
    rwn_list_obj->obj_list_cnt = cnt;

    return rwn_list_obj;
}