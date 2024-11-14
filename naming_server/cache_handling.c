#include "headers.h"

// helper finctions to copy data from one request/response to another to cache it
void copy_request(request dest, request src){
    dest->request_type = src->request_type;
    strcpy(dest->data, src->data);
}
void copy_response(response dest, response src){
    dest->response_type = src->response_type;
    strcpy(dest->message, src->message);
}

my_cache* initialize_cache(){
    my_cache* cache = (my_cache*)malloc(sizeof(my_cache));
    cache->curr_size = 0;
    return cache;
}   

bool is_cache_full(my_cache* cache){
    return cache->curr_size == CACHE_SIZE;
}

bool is_cache_empty(my_cache* cache){
    return cache->curr_size == 0;
}

void move_to_front(my_cache* cache, int index){
    cache_entry temp = cache->cache_arr[index];
    for(int i = index; i > 0; i--){
        cache->cache_arr[i] = cache->cache_arr[i-1];
    }
    cache->cache_arr[0] = temp;
}

void add_to_cache(my_cache* cache, request key, response value){
    if(is_cache_full(cache)){
        for(int i = cache->curr_size-1; i > 0; i--){
            cache->cache_arr[i] = cache->cache_arr[i-1];
        }
        cache->cache_arr[0].key = (response)malloc(sizeof(response));
        copy_request(cache->cache_arr[0].key, key);
        cache->cache_arr[0].value = (response)malloc(sizeof(response));
        copy_response(cache->cache_arr[0].value, value);
    } else {
        cache->cache_arr[cache->curr_size].key = (response)malloc(sizeof(response));
        copy_request(cache->cache_arr[cache->curr_size].key, key);
        cache->cache_arr[cache->curr_size].value = (response)malloc(sizeof(response));
        copy_response(cache->cache_arr[cache->curr_size].value, value);
        cache->curr_size++;
    }
}

response get_from_cache(my_cache* cache, request key){
    for(int i = 0; i < cache->curr_size; i++){
        if(strcmp(cache->cache_arr[i].key->data, key->data) == 0){
            move_to_front(cache, i);
            return cache->cache_arr[0].value;
        }
    }
    return NULL;
}

void remove_from_cache(my_cache* cache, request key){
    // check if present and delete along with freeing memory
    for(int i = 0; i < cache->curr_size; i++){
        if(strcmp(cache->cache_arr[i].key->data, key->data) == 0){
            free(cache->cache_arr[i].key);
            free(cache->cache_arr[i].value);
            for(int j = i; j < cache->curr_size-1; j++){
                cache->cache_arr[j] = cache->cache_arr[j+1];
            }
            cache->curr_size--;
            return;
        }
    }
}
