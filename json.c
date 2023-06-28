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
            fprintf(stderr, "Failed. Expected key-string.\n", str + i);
            return -1; // failure
        }
        i++;
    }

    int j = i + 1;
    while (str[j] != '\"') {
        if (str[j] == '\0' || str[j] == '\n') {
            fprintf(stderr, "Failed. Expected end of key-string.\n", str + i);
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
set_number(char * str, int *end, json_value * value)
{
    int i = str[0] == '-' ? 1 : 0;
    int dots = 0;
    for (; str[i]; i++) { 
        if (isdigit(str[i])) {
            continue;
        }
        else if (str[i] == '.') {
            dots++;
            if (dots == 2) {
                fprintf(stderr, "Wrong value-number (a lot of dots in number)\n");
                return -1;
            }
        }
        else if (isspace(str[i]) || str[i] == ',' || str[i] == ']' || str[i] == '}') {
            break;
        }
        else {
            fprintf(stderr, "Wrong value-number (неправильный разделить после числа\n");
            return -1;
        }
    }

    if (str[i] == '\0') {
        fprintf(stderr, "Неожиданное прерывание json-object'а\n");
        return -1;
    }
    if (i == 0 || !isdigit(str[i - 1])) {
        fprintf(stderr, "Wrong value-number. (Числовое значение заканчивается неверно)\n");
        return -1;
    }

    *end = i;
    if (dots == 0) {
        value->type = json_integer;
        value->u.integer = atoll(str);
    } else {
        char *p;
        value->type = json_double;
        value->u.dbl = strtod(str, &p);
    }
    return 0;
}
/*static void
check_true_type(json_value * value, json_type type, char const *str, char const *check)
*/
static int
set_array(char *str)
{
    return 0;
}
static int
set_value(json_value * value, char * str)
{
    int err = -1;
    int i = 0;
    int end = 0;
    value->type = json_none;
    
    for (; str[i]; i++) {
        if (!isspace(str[i])) {
            break;
        }
    }

        printf("CHECK VALUE \'%s\n", str + i);
    if (str[i] == 'n') { // maybe null
        if (strncmp(str + i + 1, "ull", 3) == 0) {
            value->type = json_null;
            end = i + 4;
            err = 0;
        }
    }
    else if (str[i] == 't') {
        if (strncmp(str + i + 1, "rue", 3) == 0) {
            value->type = json_bool;
            value->u.boolean = 1;
            end = i + 4;
            err = 0;
        }
    }
    else if (str[i] == 'f') {
        if (strncmp(str + i + 1, "alse", 4) == 0) {
            value->type = json_bool;
            value->u.boolean = 0;
            end = i + 5;
            err = 0;
        }
    }
    else if (str[i] == '-' || isdigit(str[i])) { // maybe number
        printf("try check integer \'%s\n", str + i);
        if (set_number(str + i, &end, value) == 0) {
            err = 0;
        }
    }
    else if (str[i] == '{') { // maybe object
        if (!set_object(value, str + i + 1)) {
            err = 0;
        }
    }
    else if (str[i] == '[') { // maybe array
        //if (!set_array(str + i, &end, value)) {
         //   err = 0;
        //}
    }
    else if (str[i] == '\"') { // maybe string
        value->type = json_string;
        end = set_key(&value->u.string, str + i);
        if (end == 0) {
            err = 0;
        }
    }
    return err;
}
static int
skip_dots(char * str)
{
    int i = 0;
    while (str[i] != ':') {
        if (!isspace(str[i])) {
            fprintf(stderr, "Expected separator by key and value\n");
            return -1;
        }

        i++;
    }
    return i;
}
static int
set_object(json_value * curr, char * str)
{
    // Начинать с открывающей скобки
    curr->type = json_object;
    curr->u.object.values = NULL;
    curr->u.object.length = 0;

    int i = 0;
    int pos = 0;
    int end = 0;
    //while (str[pos] != '}') {
        curr->u.object.values = realloc(curr->u.object.values, 
                                      sizeof(json_object_entry) * (i + 1));

        if ( (end = set_key(&curr->u.object.values[i].key, str + pos)) == -1 ) {
            return -1;
        } // должно стоять на закрывающиеся кавычке
        pos += end + 1; // 

        if ( (end = skip_dots(str + pos)) == -1 ) {
            return -1;
        }   // на двоеточие
        pos += end + 1;
        
        curr->u.object.values[i].value = malloc(sizeof(json_value));
        if ( (end = set_value(curr->u.object.values[i].value, str + pos)) == -1 ) {
            fprintf(stderr, "Invalid value-format\n");
            return -1;
        } // на запятой или на конце value
        pos += end + 1;

        i++;
        curr->u.object.length++;
    //}

    return 0;
}
static int
parsing(char * str, json_value * value)
{
    int err = 0;

    json_value *curr = value;
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

/*    if (err) {
        fprintf(stderr, "Different character [%c: pos: %i] was expected\n",
                stack[top - 1], i + 1
               );
    }
    */
    return 0;
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
