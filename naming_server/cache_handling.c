#include "headers.h"

// helper finctions to copy data from one request/response to another to cache it
void copy_request(request dest, request src){
    dest->request_type = src->request_type;
    strcpy(dest->data, src->data);
    strcpy(dest->path, src->path);
    strcpy(dest->copy_to_path, src->copy_to_path);
    strcpy(dest->file_or_dir_name, src->file_or_dir_name);
    dest->socket = src->socket;
}
void copy_response(response dest, response src){
    dest->response_type = src->response_type;
    strcpy(dest->message, src->message);
    strcpy(dest->IP_Addr, src->IP_Addr);
    dest->Port_No = src->Port_No;
}

my_cache* initialize_cache(){
    my_cache* cache = (my_cache*)malloc(sizeof(my_cache));
    cache->curr_size = 0;
    return cache;
}   

bool is_cache_full(){
    return cache->curr_size == CACHE_SIZE;
}

bool is_cache_empty(){
    return cache->curr_size == 0;
}

void move_to_front(int index){
    cache_entry temp = cache->cache_arr[index];
    for(int i = index; i > 0; i--){
        cache->cache_arr[i] = cache->cache_arr[i-1];
    }
    cache->cache_arr[0] = temp;
}

void add_to_cache(request key, response value){
    if(is_cache_full()){
        remove_from_cache(cache->cache_arr[cache->curr_size-1].key);
    }
    cache_entry new_entry;
    new_entry.key = (request)malloc(sizeof(st_request));
    new_entry.value = (response)malloc(sizeof(st_response));
    copy_request(new_entry.key, key);
    copy_response(new_entry.value, value);
    for(int i = cache->curr_size; i > 0; i--){
        cache->cache_arr[i] = cache->cache_arr[i-1];
    }
    cache->cache_arr[0] = new_entry;
    cache->curr_size++;
}

response get_from_cache(request key){
    for(int i = 0; i < cache->curr_size; i++){
        if(strcmp(cache->cache_arr[i].key->data, key->data) == 0){
            move_to_front(i);
            return cache->cache_arr[0].value;
        }
    }
    return NULL;
}

void remove_from_cache(request key){
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
