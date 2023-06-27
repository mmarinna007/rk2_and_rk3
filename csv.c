#include "csv.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


    /*res->count = 0;
    res->lines = malloc(sizeof(char*) * DELTA_ALLCT);

    char *line = strtok(chg_line, "\n");
    while (line) {
        res->lines[res->count] = line;
        res->count++;
        if (res->count % DELTA_ALLCT == 0) {
            res->lines = realloc(res->lines, sizeof(char*) * (res->count + DELTA_ALLCT));
        }
        res->lines = strtok(NULL, "\n");
    }
    res->lines = realloc(res->lines, sizeof(char*) * res->count);
    */
static void 
strboard(char const *s, int *start, int *len)
{
/*    *start = 0;
    int i = 0;
    while (s[i] && s[i] <= 32) { // '\t' || space etc.
        i++;
    }
    *start = i;
    
    int j = strlen(s + i); // last_symbol
    while (j > i && s[j] <= 32) {
        j--;
    }
    *len = j;
    // взять актуальный размер строки
    */
}
static int
set_keys(csv_table *tb, char *line)
{
    int err = 0;
    // разбить строку на 
    int start = 0, i = 0;
    int loop = 1;
    for (i = 0; loop; i++) {
        if (line[i] == '\0' || line[i] == ',') {
            if (line[i] == '\0') {
                loop = 0;
            } else {
                line[i] = '\0';
            }
            // ToDo: performace realloc
            tb->data = realloc(tb->data, sizeof(csv_format) * (tb->length + 1));

            //printf("set: %s\n", line + start); - ok
            
            tb->data[tb->length].key    = line + start;
            //for (int j = 0; j < tb->length; j++)
                // printf("place: %s\n", tb->data[j].key); - ok
            tb->data[tb->length].values = NULL;
            tb->data[tb->length].length = 0;
            tb->length += 1;

            start = i + 1;
        }
    }
    return err;
}
static int
set_values(csv_table *tb, char *line)
{
    int err = 0;
    // ToDo: ошибочная ситуация (кол-во value > key)
    //       строка пуста
    int start = 0, i = 0;
    int loop = 1;
    int index_key = 0;
    csv_format *it = tb->data + index_key;
    for (i = 0; loop; i++) {
        if (line[i] == '\0' || line[i] == ',') {
            if (line[i] == '\0') {
                loop = 0;
            } else {
                line[i] = '\0';
            }   
            it->values = realloc(it->values, sizeof(char*) * (it->length + 1));
            it->values[it->length] = line + start;
            printf("Value: %s\n", line + start);
            it->length += 1;
            start = i + 1;
            
            index_key++;
            if (loop && index_key == tb->length) {
                fprintf(stderr, "A lot of values for keys\n");
                err = 1;
                break;
            }
            it = tb->data + index_key;
        }
    }
    return err;
}
int
content2csv(char const *src_content, csv_table *tb)
{
    int err = 0;
    tb->data   = NULL;
    tb->length = 0;
    tb->memory = NULL;

    char *content = strdup(src_content);

    char *first_line = strtok(content, "\n");
    //printf("first line: %s\n", first_line); - ok
    err += set_keys(tb, first_line);

    char *other = strtok(NULL, "\n");
    while (!err && other) {
        err  += set_values(tb, other);
        other = strtok(NULL, "\n");
    }
    if (err) {
        free(content);
    } else {
        tb->memory = content;
    }

    return err;
}
