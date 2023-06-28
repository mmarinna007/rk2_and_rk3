#include "json.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static int set_object(json_value * curr, char * str);

static int
string_diaposon(char * str, int *st, int *en)
{
    int i = 0;
    while (str[i] != '\"') {
        if (str[i] == '\0' || !isspace(str[i])) { // 
            fprintf(stderr, "Failed[%5s]. Expected key-string.\n", str + i);
            return -1; // failure
        }
        i++;
    }

    int j = i + 1;
    while (str[j] != '\"') {
        if (str[j] == '\0' || str[j] == '\n') {
            fprintf(stderr, "Failed[%5s]. Expected key-string.\n", str + i);
            return -1;
        }
        j++;
    }

    *st = i + 1;
    *en = j;
    return 0;
}
static int
set_key(char ** key, char * str)
{
    int start = 0, end = 0;
    if (string_diaposon(str, &start, &end)) {
        return -1;
    }
    str[end] = '\0';
    *key = str + start;
    return end;
}
static int
isnumber(char * str, int *end)
{
    int i = (str[i] == '-') ?  1 : 0;
    int dot = 0;
    for (; str[i]; i++) {
        if (isdigit(str[i])) {
            continue;
        }
        else if (str[i] == '.') {
            dot++;
            if (dot == 2) {
                break;
            }
        }
        else {
            break;
        }
    }
    *end = i;
    if ()//dot == 2 || !isdigit(str[i])) {
        fprintf(stderr, "Wrong value-number\n");
        return -1;
    }

    return i;
}
/*static void
check_true_type(json_value * value, json_type type, char const *str, char const *check)
*/
static int
set_value(json_value * value, char * str)
{
    int err = -1;
    int i = 0;
    int end = 0;
    value->type = none;
    
    for (; str[i]; i++) {
        if (isspace(str[i])) {
            continue;
        }
    }
    // ToDo: сократить до одной функции
    if (str[i] == 'n') { // maybe null
        if (strncmp(str + i, "ull", 3) == 0) {
            value->type = json_null;
            err = 0;
        }
    }
    else if (str[i] == 't') {
        if (strncmp(str + i + 1, "rue", 3) == 0) {
            value->type = json_bool;
            value->u.boolean = 1;
            err = 0;
        }
    }
    else if (str[i] == 'f') {
        if (strncmp(str + i + 1, "alse", 4) == 0) {
            value->type = json_bool;
            value->u.boolean = 0;
            err = 0;
        }
    }
    else if (str[i] == '-' || isdigit(str[i])) { // maybe number
        if (isnumber(str + i, &end)) {
            value->type     = json_number;
            value->u.number = str + i;
            err = 0;
        }
    }
    else if (str[i] == '{') { // maybe object
                              
    }
    else if (str[i] == '[') { // maybe array
    
    }
    else if (str[i] == '\"') { // maybe string

    }
    return err;
}
static int
skip_dots(char * str)
{
    int i = 0;
    while (str[i] != ':') {
        if (str[i] == ' ' || isspace(str[i])) {
            i++;
        } else {
            fprintf(stderr, "Expected separator by key and value\n");
            return -1;
        }
    }
    return i;
}
static int
set_object(json_value * curr, char * str)
{
    curr->type = json_object;
    curr->u.object.values = NULL;
    curr->u.object.length = 0;

    int i = 0;
    int pos = 0;

    curr->u.object.values = realloc(curr->u.object.values, 
                                  sizeof(json_object_entry) * (i + 1));

    if ( (pos = set_key(&curr->u.object.values[i].key, str + pos)) == -1 ) {
        return -1;
    }
    pos += 1;

    if ( (pos = skip_dots(str + pos)) == -1 ) {
        return -1;
    }
    pos += 1;
    
    if ( (pos = set_value(&curr->u.object.values[i].value, str + pos)) != -1 ) {
        fprintf(stderr, "Invalid value-format\n");
        return -1;
    }

    i++;
    curr->u.object.length++;

    return 0;
}
static int
parsing(char * str, json_value * value)
{
    int err = 0;
    int top = 1; // специально начинаем, чтобы не было захода за границу массива
    char stack[100] = {0}; // ожидаемые символы
    
    json_value *curr = value;
    waiting_type waiting = NOTHING;
    int i = 0;
    for (i = 0; err == 0 && str[i]; i++)
    {
        if (str[i] == '{') {
            err = set_object(curr, str + i + 1);
            printf("Found key: %s\n", curr->u.object.values[0].key);
            //printf("Found values: %s\n", curr->u.object.values[0].);
            break;
            /*curr->length = 0;
            curr->type = json_object;
            curr->value = json_object;
            do {
                curr->values = 
                get_key(str, i);
                get_value(str, i);
            } while (str[i] != '}');
            */
        }
    } // for (stc[i])

    if (top != 1 || err) {
        fprintf(stderr, "Different character [%c: pos: %i] was expected\n",
                stack[top - 1], i + 1
               );
    }
}
int 
parse2json(char const * src, json_value * value)
{
    int err = 0;
    char *copy_src = strdup(src);
    value->type = json_none;
    //value->parent = NULL;
    value->next = NULL;
    
    err = parsing(copy_src, value);
    if (err) {
        free(copy_src);
    }
    
    return err;
}
