#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "json.h"

void print_json(json_value const *tb);

void print_array(json_value const *tb)
{
    printf("\n[");
    for (int i = 0; i < tb->u.array.length; i++) {
        print_json(tb->u.array.values[i]);
        printf(" ");
    }
    printf("]\n");
}
void print_json(json_value const *tb)
{
    static int deep = 0;
    for (int i = 0; i < deep; i++) {
        printf("\t");
    }
    deep++;
    if (tb->type == json_object) {
        printf("{\n");
        for (int i = 0; i < tb->u.object.length; i++) {
            printf("[%i]%s : ", i, tb->u.object.values[i].key);
            print_json(tb->u.object.values[i].value);
        }
        printf("\n}\n");
        deep--;
        return;
    }
    switch (tb->type) {
        case json_array:
            print_array(tb);
            break;
        case json_integer:
            printf("%i\n", tb->u.integer);
            break;
        case json_double:
            printf("%lf\n", tb->u.dbl);
            break;
        case json_string:
            printf("%s\n", tb->u.string);
            break;
        case json_bool:
            printf((tb->u.boolean ? "true\n" : "false\n"));
            break;
        case json_null:
            printf("null\n");
            break;
    }
    deep--;
}
int main(int argc, char **argv) {
    if (argc == 1) {
        (void)printf("not fname\n");
        return 1;
    }

    char *filename = argv[1];
    char *content  = NULL;
    FILE *fp       = NULL;
    struct stat stat_file = {0};
 
    if ( stat(filename, &stat_file) != 0 ) {
        (void)fprintf(stderr, "File %s not found\n", filename);
        return -1;
    }
    fp = fopen(filename, "rt");
    if (fp == NULL) {
        (void)fprintf(stderr, "Unable to open %s\n", filename);
        return -1;
    }

    content = calloc(1, stat_file.st_size + 1);
    //printf("size of file %zu\n", stat_file.st_size);
    //printf("%zu\n", fread(content, stat_file.st_size, 1, fp));
    if ( fread(content, 1, stat_file.st_size, fp) != stat_file.st_size) {
        (void)fprintf(stderr, "Unable to read content of %s\n", filename);
        free(content);
        fclose(fp);
        return -1;
    }
    
    json_value *tb = parse2json(content);
    if (tb != NULL) {
        printf("==========================\n");
        print_json(tb);
        printf("==========================\n");
        destroy(tb);
    } else {
        
    }
    fclose(fp);
    free(content);
}
