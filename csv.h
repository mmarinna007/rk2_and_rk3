#ifndef CSV_FORMAT
#define CSV_FORMAT

#include <stddef.h>

typedef struct 
{
    char const *key;
    char const **values;
    int  length;
} csv_format;

typedef struct 
{
    csv_format *data;
    int length;

    char *memory; // full content
} csv_table;

int content2csv(char const *content, csv_table *tb);
//void destroy_csvtable(csv_table *tb);

#endif // CSV_FORMAT
