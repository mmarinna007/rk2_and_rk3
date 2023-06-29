#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "csv.h"
int main(int argc, char **argv) {
    if (argc == 1) {
        printf("not fname\n");
        return 1;
    }

    char *filename = argv[1];
    char *content  = NULL;
    FILE *fp       = NULL;
    struct stat stat_file = {0};
 
    csv_table tb;
    if ( stat(filename, &stat_file) != 0 ) {
        fprintf(stderr, "File %s not found\n", filename);
    }
    fp = fopen(filename, "rt");
    if (fp == NULL) {
        fprintf(stderr, "Unable to open %s\n", filename);
    }

    content = malloc(stat_file.st_size);
    if ( fread(content, stat_file.st_size + 1, 1ull, fp) != 1) {
        fprintf(stderr, "Unable to read content of %s\n", filename);
    }
    content2csv(content, &tb);
    for (int i = 0; i < tb.length; i++) {
        printf("[%i] Keys: %s\n", i + 1, tb.data[i].key);
        for (int j = 0; j < tb.data[i].length; j++) {
            printf("[%i] value: %s\n", j + 1, tb.data[i].values[j]);
        }
        printf("======================\n");
    }

    fclose(fp);
    free(content);
}
