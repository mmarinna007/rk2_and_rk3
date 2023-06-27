#include "json.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
//static void
struct { // для определения символов
    int start;
    int end;
} pair_t;


static int
get_task(char const *src, pair_t *stack, int *top)
{
    // перебор содержимого строки для определения границ содержимого
    int i = 0;
    for (i = 0; src[i] != '\0'; i++) {
        if (src[i] == '{') {
            
        }
        else if (src[i] == '[') {
        }
        else if (src[i] == '
    }
}
int 
parse2json(char const *src, json_value *value)
{
    int err = 0;
    char *copy_src = strdup(content);
    // Выделить основную часть:
    //
    // Распознать объект или массив
    //
    value->parent = NULL;
    value->type = none;
    value->next = NULL;
    for (int i = 0; copy_src[i]; i++) {
       if (isdigit(copy_src[i])) {
           //value->type = json_object;   
       }
       else if () {
       }
       else {
       }
    }
    
    // Варианты:
    // [ - массив
    // { - объект
    // " - строка
    // 123 - число
    // null
    //
    // Распознать вариацию
    
    return err;
}
