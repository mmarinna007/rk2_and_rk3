#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "convert.h"
static int strend(char const *s, char const *t)
{
    // Семинар 8 - задание 1
    size_t len_s = strlen(s);
    size_t len_t = strlen(t);
    if (len_t <= len_s) {
        for (int i = len_t - 1; i >= 0; i--){
            if (t[i] != s[i-len_t+len_s]) {
                return 0;
            }
        } 
        return 1;
    }
    return 0;
}
int main(int argc, char **argv)
{
    if (argc == 1 || argc > 3) {
        (void)fprintf(stderr, "Missing [position] arg-filename (*.json && *.csv)\n");
        return 1;
    }

    // проверка исходного файла
    char *file1 = argv[1];
    int is_csv = strend(file1, ".csv");
    if (is_csv == 0 && strend(file1, ".json") == 0) {
        (void)fprintf(stderr, "Undefined format file %s\n", file1);
        return 2;
    }
    
    struct stat stat_file = {0};
    if ( stat(file1, &stat_file) != 0 ) {
        (void)fprintf(stderr, "Source-File %s not found\n", file1);
        return 3;
    }

    // проверка конечного файла
    char buf[64] = {0};
    char *file2 = argv[2];
    if (file2 != NULL) {
        if (strend(file2, (is_csv == 0 ? ".json" : ".csv")) == 1) {
            (void)fprintf(stderr, "File format must be different [%s - %s]\n", file1, file2);
            return 5;
        }
        else if (strend(file2, (is_csv ? ".json" : ".csv")) == 0) {
            (void)fprintf(stderr, "Incorrect file format [%s]\n", file2);
            return 6;
        }

        struct stat tmp = {0};
        if (stat(file2, &tmp) == 0) {
            (void)fprintf(stderr, "Exist dest-file %s (can't re-record)\n", file2);
            return 4;
        }
    }
    else { // если не был пропущено второе имя файла, тогда сохраняем имя исходного файла, заменяя расширение на противоположное
        strcpy(buf, file1);
        strcpy(strchr(buf, '.'), (is_csv == 0 ? ".csv" : ".json"));
        file2 = buf;
    }
    // Статусом завершения программы зависит от статуса конвертирования.
    return (is_csv == 1) ? csv2json(file1, file2) : json2csv(file1, file2);
}
