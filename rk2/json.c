#include "json.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static int set_value(json_value * value, char const * str);
static int set_object(json_value * curr, char const * str);
 
/*
 *  Функция для опеределения границ json_string
 *      json_string - строка в кавычках.
 */
static int
string_diaposon(char const * str, int *st, int *en)
{
    int i = 0;
    while (str[i] != '\"') { // определить начало
        if (str[i] == '\0' || !isspace(str[i])) { // если встретися ломающий символ
            (void)fprintf(stderr, "Failed. Expected key-string.\n");
            return -1; // failure
        }
        i++;
    }

    int j = i + 1;
    while (str[j] != '\"') { // определить конец
        if (str[j] == '\0' || str[j] == '\n') {
            (void)fprintf(stderr, "Failed. Expected end of key-string.\n");
            return -1;
        }
        j++;
    }

    // результат функции, определяющий поведение других функций
    *st = i + 1;
    *en = j;
    return 0;
}
/*
 *  Взятие строчного ключа для пары "Ключ" - значение  
 */
static int
set_key(char ** key, char const * str)
{
    int start = 0, end = 0;
    if (string_diaposon(str, &start, &end)) { // определяем где начинается, где заканчивается
        return -1;
    }
    // str[end] = '\0';
    *key = calloc(sizeof(char), (end - start + 1)); // выделяем память для результата
    if (*key == NULL) {
        (void)fprintf(stderr, "Doesn't allocate memory for key %s\n", __func__);
        return -1;
    }
    (void)memcpy(*key, str + start, sizeof(char) * (end - start));  // копируем строку
    return end; // вернули позицию последнего символа
}
/*
 * Перевод из строки в число (double || int)
 */
static int
set_number(char const * str, json_value * value)
{
    int i = str[0] == '-' ? 1 : 0; // чтобы начать проверку с цифр и точек
    int dots = 0; 
    for (; str[i]; i++) { 
        if (isdigit(str[i])) {
            continue;
        }
        else if (str[i] == '.') {
            dots++;
            if (dots == 2) { // В числе может быть одна дробная часть
                (void)fprintf(stderr, "Wrong value-number (a lot of dots in number)\n");
                return -1;
            }
        }
        else if (isspace(str[i]) || str[i] == ',' || str[i] == ']' || str[i] == '}') { // допустимые символы после числа
            break;
        }
        else {
            (void)fprintf(stderr, "Wrong value-number (неправильный разделитель после числа\n");
            return -1;
        }
    }

    if (str[i] == '\0') {
        (void)fprintf(stderr, "Неожиданное прерывание json-object'а\n");
        return -1;
    }
    if (i == 0 || !isdigit(str[i - 1])) {
        (void)fprintf(stderr, "Wrong value-number. (Числовое значение заканчивается неверно)\n");
        return -1;
    }

    if (dots == 0) { // если не было точек
        value->type = json_integer;
        value->u.integer = atoll(str); // из станд. библиотеки функция, которая перевод из строки в long long значение
        //(void)printf("integer: %i\n", value->u.integer);
    } else {
        char *p; // переменная для ф-ии strtod, результатом будет адрес последнего символа
        value->type = json_double;
        value->u.dbl = strtod(str, &p); // станд. библ., из строки в double
        //(void)printf("double: %lf\n", value->u.dbl);
    }
    return i; // вернули позицию символа идущего после обработанного исла
}
/*
 * Определение значений в массиве (json_array)
 */
static int
set_array(json_value * value, char const * str)
{
    value->type = json_array;      // стартовые значения
    value->u.array.values = NULL;
    value->u.array.length = 0;

    int i = 0;
    int pos = str[0] == '[' ? 1 : 0; 
    int end = 0;
    int err = 0;
    // Перебираем символы в строке пока не встретилась ошибка ИЛИ не закончились эл-ты
    while (err == 0 && str[pos] != ']') { 
        // Некоторые тонкости: [от NULL тоже работает, как и при любом другом адресе]  
        // realloc(NULL, ...);
        
        // добавили в общую тару кусочек памяти для нового эл-та массива
        value->u.array.values = realloc(value->u.array.values, 
                                      sizeof(json_value*) * (i + 1)); 

        // непосредственно для самого эл-та массива
        value->u.array.values[i] = calloc(sizeof(json_value), 1);
        // __func__ - станд. макрос для вывода имени функции (т.е. сейчас __func__ = set_array)
        if (value->u.array.values[i] == NULL) {
            (void)fprintf(stderr, "Failed allocate memory for value of array in %s\n", __func__);
            err = 1;
            break;
        }
        
        // Попытка взять значение
        if ( (end = set_value(value->u.array.values[i], str + pos)) == -1 ) {
            (void)fprintf(stderr, "Invalid value of array\n");
            err = 1;
        } 
        pos += end; // переход на позицию следующего зн-ия 
                    
        i++;
        value->u.array.length++; // Общее кол-во эл-ов массива
                                 
        while (str[pos] == ',' || isspace(str[pos])) { // пропуск лишних символов
            pos += 1;
            if (str[pos] == '\0') {
                (void)fprintf(stderr, "Expected \']\' close object\n");
                err = 2;
                break;
            }
        }
    }
    pos += 1; // пропустили закрывающую фиг. скобку
    if (err) { // если произошла ошибка, избегаем общей утечки
        for (int j = 0; j < i; j++) {
            destroy(value->u.array.values[i]);
        }
        free(value->u.array.values);
        return -1;
    }

    return pos; // позиция следующего необработанного значения
}
/*
 *
 * Определение что за зверь такой (NULL || число || булевое-значение || object || строка || массив )
 *
 */
static int
set_value(json_value * value, char const * str)
{
    int i = 0;
    int end = -1;
    value->type = json_none;
    
    for (; str[i]; i++) {
        if (!isspace(str[i])) {
            break;
        }
    }

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
    }
    else {
        (void)fprintf(stderr, "Field for value is empty\n");
    }
    return end;
}
static int
skip_dots(char const * str)
{
    int i = 0;
    while (str[i] != ':') {
        if (!isspace(str[i])) {
            (void)fprintf(stderr, "Expected separator by key and value\n");
            return -1;
        }

        i++;
    }
    return i;
}
static int
set_object(json_value * value, char const * str)
{
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
        
        value->u.object.values[i].value = calloc(sizeof(json_value), 1);
        if (value->u.object.values[i].value == NULL) {
            (void)fprintf(stderr, "Doesn't allocate memory for value of object %s\n", __func__);
            break;
        }
        
        if ( (end = set_value(value->u.object.values[i].value, str + pos)) == -1 ) {
            (void)fprintf(stderr, "Invalid value of object\n");
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
                (void)fprintf(stderr, "Expected \'}\' close object\n");
                return -1;
            }
        }
    }
    pos += 1;
    //printf("exit object %s\n\n", str + pos);
    return pos;
}
json_value*
parse2json(char const * src)
{
    //char *copy_src = strdup(src);
    //if (copy_str == NULL) {
      //  (void)fprintf(stderr, "Error allocate memory for duplicate[%s]\n", __func__);
    //}
    json_value *value = calloc(sizeof(json_value), 1);
    if (value == NULL) {
        (void)fprintf(stderr, "Error allocate memory for json_value [%s]\n", __func__);
        return NULL;
    }

    int i = 0;
    int err = 0;
    while (err == 0) {
        if (src[i] == '{') {
            break;
        } 
        else if (!isspace(src[i])) {
            (void)fprintf(stderr, "%s\n", 
                  src[i] == '\0' ? "Empty json-file. Expected object" : "Wrong json-format. Expected object\n"
                    );
            err = 1;
        }
        i++;
    }
    if (err == 0) {
        if (set_object(value, src + i) == -1) {
            err = 1;
        }
    }
    if (err) {
        //destoy(value);
        value = NULL;
    }

    return value;
}
void 
destroy(json_value * value)
{
    // чистка содержимого
    if (value != NULL) {
        if (value->type == json_object) {
            for (int i = 0; i < value->u.object.length; i++) {
                free(value->u.object.values[i].key);
                destroy(value->u.object.values[i].value);
            }
            free(value->u.object.values);
        }
        else if (value->type == json_string) {
            free(value->u.string);
        }
        else if (value->type == json_array) {
            for (int i = 0; i < value->u.array.length; i++) {
                destroy(value->u.array.values[i]);
            }
            free(value->u.array.values);
        }
        free(value);
    }
}



////////////////////////////////////////////////////////////////////////////////////////////////
///
///
///                         PRINT
///
///
////////////////////////////////////////////////////////////////////////////////////////////////




static void out_json(FILE * outstream, json_value const * tb, int depth);


static void
print_depth_shift(FILE * stream, int depth)
{
    for (int i = 0; i < depth; i++) {
        (void)fprintf(stream, " ");
    }
}
static void 
print_array(FILE * stream, json_value const *tb, int depth)
{
    int i = 0;
    (void)fprintf(stream, "[");
    for (i = 0; i < tb->u.array.length - 1; i++) {
        out_json(stream, tb->u.array.values[i], depth);
        (void)fprintf(stream, ", ");
    }
    out_json(stream, tb->u.array.values[i], depth);
    (void)printf("]\n");
}
static void
out_json(FILE * stream, json_value const * tb, int depth)
{
    if (tb->type != json_object) {
        print_depth_shift(stream, depth);       
    }
    else {
        depth += 1;
        (void)fprintf(stream, "{\n");
        print_depth_shift(stream, depth);
        (void)fprintf(stream, "%s : ", tb->u.object.values[0].key);
        out_json(stream, tb->u.object.values[0].value, depth + 1);

        for (int i = 1; i < tb->u.object.length; i++) {
            print_depth_shift(stream, depth);
            (void)fprintf(stream, ",\n%s : ", tb->u.object.values[i].key);
            out_json(stream, tb->u.object.values[i].value, depth + 1);
        }
        (void)fprintf(stream, "\n}\n");
        return;
    }

    switch (tb->type) {
        case json_array:
            print_array(stream, tb, depth + 1);
            break;
        case json_integer:
            (void)fprintf(stream, "%lld", tb->u.integer);
            break;
        case json_double:
            (void)fprintf(stream, "%lf", tb->u.dbl);
            break;
        case json_string:
            (void)fprintf(stream, "%s", tb->u.string);
            break;
        case json_bool:
            (void)fprintf(stream, (tb->u.boolean ? "true" : "false"));
            break;
        case json_null:
            (void)fprintf(stream, "null");
            break;
    }
}
void 
print_json(FILE * outstream, json_value const * tb)
{
    out_json(outstream, tb, 0);
}
