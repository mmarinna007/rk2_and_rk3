#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "file.h"
#include "csv.h"
#include "json.h"

#define MAX_SIZE 10

typedef struct Node node_t;
struct Node {
    node_t *next;
    char   *city;
    size_t p;
};

typedef struct {
    node_t **l;
    size_t msize;
} table_t;

size_t hash(char const *s)
{
    size_t h = 0;
    for (int i = 0; s[i]; i++) {
        h = h * 17 + s[i];
    }
    return h;
}
node_t* new_node(char *c, size_t p)
{
    node_t *n = malloc(sizeof(node_t));
    n->next = NULL;
    n->city = strdup(c);
    n->p = p;
    return n;
}
int add(table_t *t, char *c, size_t p)
{
    size_t h = hash(c) % t->msize;
    node_t *it = t->l[h];
    while (it) {
        if (strcmp(it->city, c) == 0) {
            printf("city has already been added\n");
            return 1;
        }
        it = it->next;
    }
    it = new_node(c, p);
    it->next = t->l[h];
    t->l[h] = it;
    return 0;
}
void erase(table_t *t, char const *s)
{
    size_t h = hash(s) % t->msize;
    _Bool found = 0;
    node_t  *it = t->l[h];

    if (it && strcmp(it->city, s) == 0) {
        found = 1;
        free(it->city);
        t->l[h] = it->next;
        free(it);
    }
    else {
        while (it->next) {
            if (strcmp(it->next->city, s) == 0) {
                node_t *n = it->next;
                free(it->next->city);
                it->next = it->next->next;
                free(n);

                found = 1;           
                break;
            }
            it = it->next;
        }
    }

    if (found == 0) {
        fprintf(stderr, "not found\n");
    }
}
void list(table_t const *t)
{
     for (size_t i = 0; i < t->msize; i++) {
        node_t  *it = t->l[i];
        while (it) {
            printf("%s %zu\n", it->city, it->p);
            it = it->next;
        }
     }
}
void init(table_t *t)
{
    t->l = calloc(sizeof(node_t*), MAX_SIZE);
    t->msize = MAX_SIZE;
}
void destroy_table(table_t *t)
{
    for (size_t i = 0; i < t->msize; i++) {
        while (t->l[i]) {
            node_t *n = t->l[i];
            t->l[i] = t->l[i]->next;
            free(n->city);
            free(n);
        }
    }
    free(t->l);
}

// json 
// csv
void csv_parse(table_t * t, char const * fname)
{
    int err = 0;
    char * content = get_content_file(fname);
    if (content == NULL) {
        return;
    }

    csv_table tb = {0};
    content2csv(content, &tb);
    
    free(content);
}
void json_parse(table_t * t, char const * fname)
{
    int err = 0;
    char * content = get_content_file(fname);
    if (content == NULL) {
        return;
    }
    json_value * value = parse2json(content);
    free(content);

    if (value->type == json_object) {
        for (int i = 0; i < value->u.object.length; i++) {
            json_object_entry * it = value->u.object.values + i;

            if (it->value->type == json_integer) {
                add(t, it->key, it->value->u.integer);
            }
            else {
                err = 1;
                break;
            }
        }
    }
 /*   else if (value->type == json_array) {
        for (int i = 0; i < value->u.array.length; i++) {
            json_value * it = value->u.array.values[i];
            if (it->type == json_object && it->u.object->type == json_integer) {
                add(t, it->u.obj  
            }
            else {
                err = 1;
                break;
            }
        }
    }
    */
    else {
        err = 1;
    }
    destroy(value);

    if (err) {
        fprintf(stderr, "Database doesn't support json-format data\n");
    }
}
void import_filedata(table_t *t, int argc, char **argv)
{
    char *filename = NULL;
    //char *content  = NULL;
    //FILE *fp       = NULL;
    //struct stat stat_file = {0};
    //for (int i = 1; i < argc; i++) {
        filename = argv[1];
        size_t length = strlen(filename);
        if (length > 5ull && strcmp(filename + length - 5, ".json") == 0) {
            json_parse(t, filename);
        }
        else if (length > 4ull && strcmp(filename + length - 4, ".csv") == 0) {
            csv_parse(t, filename);
        }
        else {
            fprintf(stderr, "Not recognize file format [%s]\n", filename);
            //continue;
        }
        
    //} // for
}

int main(int argc, char **argv)
{
    table_t t;
    init(&t);

    import_filedata(&t, argc, argv);

    char l[128] = {0};
    size_t p = 0;
    for (;;) {
        char *m[] = {"exit", "add", "erase", "list"};
        for (size_t i = 0; i < sizeof(m) / sizeof(m[0]); i++) {
            printf("%zu - %s\n", i, m[i]);
        }
        int ch;
        if (scanf("%d", &ch) < 1) {
            break;
        }
        if (ch == 0) {
            break;
        } 
        else if (ch == 1) {
            scanf("%s %zu",  l, &p);
            add(&t, l, p);
        }
        else if (ch == 2) {
            scanf("%s",  l);
            erase(&t, l);
        }
        else {
            list(&t);
        }
    }
    destroy_table(&t);
}
