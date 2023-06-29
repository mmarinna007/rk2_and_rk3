#ifndef _JSON_H
#define _JSON_H

#include <stdio.h>

typedef enum
{
   json_none = 0,
   json_object, // { }
   json_array,
   json_integer,
   json_double,
   json_string,
   json_bool, 
   json_null
} json_type;

typedef struct _json_object_entry
{
    char * key;
    struct _json_value * value;
} json_object_entry;

typedef struct _json_value
{
   json_type type;
   union
   {
      char * string;
      int boolean;
      double dbl;
      long long integer;

      struct 
      {
        json_object_entry * values;
        int length;
      } object;

      struct
      {
         struct _json_value ** values;
         int length;
      } array;
   } u;
} json_value;

json_value* parse2json(char const * content);// json_value * value);
void destroy(json_value * value);

void print_json(FILE * outstream, json_value const * value);

#endif 
