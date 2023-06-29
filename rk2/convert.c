#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "json.h"
#include "csv.h"

static int
tojson(char const *f2, csv_table const * tb)
{
    
}
static char* 
get_content_file(char const *filename)
{
    // прочитать содержимое файла (функция из рк1)
    char *content  = NULL;
    FILE *fp       = NULL;
    struct stat stat_file = {0};
 
    if ( stat(filename, &stat_file) != 0 ) {
        (void)fprintf(stderr, "File %s not found\n", filename);
        return NULL;
    }
    fp = fopen(filename, "rt");
    if (fp == NULL) {
        (void)fprintf(stderr, "Unable to open %s\n", filename);
        return NULL;
    }

    content = calloc(1, stat_file.st_size + 1);
    if ( fread(content, 1, stat_file.st_size, fp) != stat_file.st_size) {
        (void)fprintf(stderr, "Unable to read content of %s\n", filename);
        free(content);
        content = NULL;
    }

    fclose(fp);
    return content;
}
int 
csv2json(char const * f1, char const * f2)
{
    char *content = get_content_file(f1);
    if (content == NULL) {
        return 1;
    }
    csv_table tb = {0};
    if (content2csv(content, &tb)) {
        free(content);
        return 2;
    }
    tojson(&tb, f2);
    destroy_csvtable(&tb);
    return 0;
}
int 
json2csv(char const * f1, char const * f2)
{
    /*char *content = get_content_file(f1);
    if (content == NULL) {
        return 1;
    } 
    json_value *tb = parse2json(content);
    // перевод

    destroy(tb);
    */
    return 0;
}
