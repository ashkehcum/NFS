#ifndef _CACHE_HANDLING_H_
#define _CACHE_HANDLING_H_
#include "headers.h"

#define CACHE_SIZE 15
#define MAX_CACHE_ENTRY_SIZE 100

typedef struct cache_entry{
    request key;
    response value;
} cache_entry;

typedef struct my_cache{
    cache_entry cache_arr[CACHE_SIZE];
    int curr_size;
} my_cache;

my_cache* initialize_cachw();
bool is_cache_full(my_cache* cache);
bool is_cache_empty(my_cache* cache);
void move_to_front(my_cache* cache, int index);
void add_to_cache(my_cache* cache, request key, response value);
response get_from_cache(my_cache* cache, request key);
void remove_from_cache(my_cache* cache, request key);


#endif