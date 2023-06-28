#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "json.h"
int main(int argc, char **argv) {
    if (argc == 1) {
        (void)printf("not fname\n");
        return 1;
    }

    char *filename = argv[1];
    char *content  = NULL;
    FILE *fp       = NULL;
    struct stat stat_file = {0};
 
    json_value tb;
    if ( stat(filename, &stat_file) != 0 ) {
        (void)fprintf(stderr, "File %s not found\n", filename);
    }
    fp = fopen(filename, "rt");
    if (fp == NULL) {
        (void)fprintf(stderr, "Unable to open %s\n", filename);
    }

    content = calloc(1, stat_file.st_size + 1);
    if ( fread(content, stat_file.st_size + 1, 1ull, fp) != 1) {
        (void)fprintf(stderr, "Unable to read content of %s\n", filename);
    }
    
    parse2json(content, &tb);

    fclose(fp);
    free(content);
}
