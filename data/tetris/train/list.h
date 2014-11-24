#include "utils.h"

typedef struct{
    void *header;
    int elem_size;
    int used_len;
    int alloc_len;
    void(*free_fn)(void*);
} LIST;

void list_new(LIST *list, int elem_size, void(*free_fn)(void*));
void* list_add_elem(LIST *list, void *data);
void* list_add_elem_with_unique(LIST *list, void *data, BOOL(*cmp_fn)(void* exist, void* data));
void* list_get_elem_by_idx(LIST *list, int idx);
int list_find_idx(LIST *list, void *data, BOOL(*predicate_fn)(void* exist, void* data));

void list_each_elem_do(LIST *list, void(*do_fn)(void*, int));
void list_free(LIST *list);
