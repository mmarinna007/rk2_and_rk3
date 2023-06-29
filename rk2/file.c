#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

// Чтение содержимого файла (функция из рк1)
char*
get_content_file(char const *filename)
{
    struct stat stat_file = {0};
    if ( stat(filename, &stat_file) != 0 ) {
        fprintf(stderr, "File %s not found\n", filename);
        return NULL;
    }

    FILE * fp = fopen(filename, "rt");
    if (fp == NULL) {
        fprintf(stderr, "Unable to open %s\n", filename);
        return NULL;
    }

    char * content = calloc(1, stat_file.st_size + 1);
    if ( fread(content, 1, stat_file.st_size, fp) != stat_file.st_size) {
        fprintf(stderr, "Unable to read content of %s\n", filename);
        free(content);
        content = NULL;
    }

    fclose(fp);
    return content;
}


