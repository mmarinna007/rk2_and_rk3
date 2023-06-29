#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <ctype.h>
#include "json.h"
#include "csv.h"

static void
print_value(FILE * file, char const * str)
{
    while (str[0] == ' ') {
        str++;
    }
    int i = str[0] == '-' ? 1 : 0;
    int is_str = 0;
    for (; str[i]; i++) { 
        if (!isdigit(str[i])) {
            is_str = 1;
            break;
        }
    }

    if (is_str) {
        fprintf(file, "\"");
    }
    fprintf(file, "%s", str);
    if (is_str) {
        fprintf(file, "\"");
    }
}
static int
tojson(csv_table const * tb, char const * f2)
{
    FILE * file = fopen(f2, "wt");
    if (file == NULL) {
        fprintf(stderr, "File-result %s unable create\n", f2);
        return 2;
    }

    if (tb->length == 1) {
        fprintf(file, "{\n");
        for (int i = 0; i < tb->data[0].length; i++) {
            if (i != 0) {
                fprintf(file, ", ");
            }
            fprintf(file, "\"%s\" : \"%s\"", tb->data[0].key, tb->data[0].values[i]);
        }
        fprintf(file, "\n}");
    }
    else {
        fprintf(file, "\"%s\" : [\n", tb->data[0].key);
        for (int i = 0; i < tb->data[0].length; i++) {
            if (i > 0) {
                fprintf(file, ",\n");
            }

            fprintf(file, "\t\"%s\" : {\n", tb->data[0].values[i]);
            for (int j = 1; j < tb->length; j++) {
                if (j > 1) {
                    fprintf(file, ",\n");
                }
                fprintf(file, "\t\"%s\" : ", tb->data[j].key);
                print_value(file, tb->data[j].values[i]);
            }
            fprintf(file, "\n\t}");
        }
        fprintf(file, "\n]\n");
    }

    fclose(file);
    return 0;
}










static void
tocsv(json_value const *v, char const * f2)
{
    FILE * file = fopen(f2, "wt");
    if (file == NULL) {
        fprintf(stderr, "File-result %s unable create\n", f2);
        return;
    }
    
    fclose(file);
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
    char *content = get_content_file(f1);
    if (content == NULL) {
        return 1;
    } 
    json_value *tb = parse2json(content);
    //tocsv(tb, f2);
    destroy(tb);
    return 0;
}
