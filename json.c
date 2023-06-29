#include "json.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static int set_value(json_value * value, char * str);
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
    printf("key: %s [%i:%i]\n", *key, start, end);
    return end;
}
static int
set_number(char * str, json_value * value)
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

    if (dots == 0) {
        value->type = json_integer;
        value->u.integer = atoll(str);
        printf("integer: %i\n", value->u.integer);
    } else {
        char *p;
        value->type = json_double;
        value->u.dbl = strtod(str, &p);
        printf("double: %lf\n", value->u.dbl);
    }
    return i;
}
/*static void
check_true_type(json_value * value, json_type type, char const *str, char const *check)
*/
static int
set_array(json_value * value, char * str)
{
    value->type = json_array;
    value->u.array.values = NULL;
    value->u.array.length = 0;

    int i = 0;
    int pos = str[0] == '[' ? 1 : 0;
    int end = 0;
    while (str[pos] != ']') {
        value->u.array.values = realloc(value->u.array.values, 
                                      sizeof(json_value*) * (i + 1));

        value->u.array.values[i] = malloc(sizeof(json_value));
        printf("status %s\n=======================\n", str + pos);
        if ( (end = set_value(value->u.array.values[i], str + pos)) == -1 ) {
            fprintf(stderr, "Invalid value of array\n");
            return -1;
        } // на запятой или на конце value
        pos += end;
        //printf("END: %s\n", str + pos);
        i++;
        value->u.array.length++;
        //printf("OBJECT %d %s\n", ++c, curr->u.object.values[i].key);
        while (str[pos] == ',' || isspace(str[pos])) {
            pos += 1;
            if (str[pos] == '\0') {
                fprintf(stderr, "Expected \']\' close object\n");
                return -1;
            }
        }
    }
    pos += 1;

    return pos;
}
static int
set_value(json_value * value, char * str)
{
    int i = 0;
    int end = -1;
    value->type = json_none;
    
    for (; str[i]; i++) {
        if (!isspace(str[i])) {
            break;
        }
    }

        //printf("CHECK VALUE \'%s\n", str + i);
    if (str[i] == 'n') { // maybe null
        if (strncmp(str + i + 1, "ull", 3) == 0) {
            value->type = json_null;
            end = i + 4;
        }
    }
    else if (str[i] == 't') {
        if (strncmp(str + i + 1, "rue", 3) == 0) {
            value->type = json_bool;
            value->u.boolean = 1;
            end = i + 4;
        }
    }
    else if (str[i] == 'f') {
        if (strncmp(str + i + 1, "alse", 4) == 0) {
            value->type = json_bool;
            value->u.boolean = 0;
            end = i + 5;
        }
    }
    else if (str[i] == '-' || isdigit(str[i])) { // maybe number
        //printf("try check integer \'%s\n", str + i);
        end = set_number(str + i, value);
        if (end != -1) {
            end += i;
        }
    }
    else if (str[i] == '{') { // maybe object
        end = set_object(value, str + i + 1);
        if (end != -1) {
            end += i + 1;
        }
    }
    else if (str[i] == '[') { // maybe array
        end = set_array(value, str + i);
        if (end != -1) {
            end += i;
        }
    }
    else if (str[i] == '\"') { // maybe string
        value->type = json_string;
        //printf("value-");
        end = set_key(&value->u.string, str + i);
        if (end != -1) {
            end += i + 1;
        }
        //printf("end = \'%s\n", str + end);
    }
    return end;
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
set_object(json_value * value, char * str)
{
    static int c = 0;
    // Начинать с открывающей скобки
    value->type = json_object;
    value->u.object.values = NULL;
    value->u.object.length = 0;

    int i = 0;
    int pos = str[0] == '{' ? 1 : 0;
    int end = 0;
    while (str[pos] != '}') {
        value->u.object.values = realloc(value->u.object.values, 
                                      sizeof(json_object_entry) * (i + 1));

        if ( (end = set_key(&value->u.object.values[i].key, str + pos)) == -1 ) {
            return -1;
        } // должно стоять на закрывающиеся кавычке
        pos += end + 1; // 

        if ( (end = skip_dots(str + pos)) == -1 ) {
            return -1;
        }   // на двоеточие
        pos += end + 1;
        
        value->u.object.values[i].value = malloc(sizeof(json_value));
        if ( (end = set_value(value->u.object.values[i].value, str + pos)) == -1 ) {
            fprintf(stderr, "Invalid value of object\n");
            return -1;
        } // на запятой или на конце value
        pos += end;
        //printf("END: %s\n", str + pos);
        i++;
        value->u.object.length++;
        //printf("OBJECT %d %s\n", ++c, curr->u.object.values[i].key);
        while (str[pos] == ',' || isspace(str[pos])) {
            pos += 1;
            if (str[pos] == '\0') {
                fprintf(stderr, "Expected \'}\' close object\n");
                return -1;
            }
        }
    }
    pos += 1;
    printf("exit object %s\n\n", str + pos);

    return pos;
}
int 
parse2json(char const * src, json_value * value)
{
    int err = 0;
    char *copy_src = strdup(src);
    value->type = json_none;
    value->next = NULL;

    int i = 0;
    while (err == 0) {
        if (copy_src[i] == '{') {
            break;
        } 
        else if (!isspace(copy_src[i])) {
            fprintf(stderr, "%s\n", 
                    copy_src[i] == '\0' ? "Empty json-file. Expected object" : "Wrong json-format. Expected object\n"
                    );
            err = 1;
        }
        i++;
    }
    if (err == 0) {
        if (set_object(value, copy_src + i) == -1) {
            err = 1;
        }
        //printf("key %s\n", value->u.object.values[i].key);
    }
    if (err) {
        free(copy_src);
    }
    
    return err;
}
