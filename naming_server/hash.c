#include"headers.h"

node* Create_hastable(int itablesize){
    node* temp = (node*) malloc(sizeof(node) * itablesize);
    for(int i = 0; i < itablesize; ++i){
        temp[i] = (node) malloc(sizeof(st_node));
        temp[i]->next = NULL;
    }
    return temp;
}

int isPrime(int x){
    for(int i = 2; i * i <= x; ++i){
        if(x % i == 0)
            return 0;
    }
    return 1;
}

int findnextprime(int x){
    for(int i = x + 1; ; ++i){
        if(isPrime(i))
            return i;
    }
}

long long int create_hash(char* x, int* primes, int n){
    long long int ans = 1, mod = 100003;
    for(int i = 0; i < n; ++i)
        ans = (ans * primes[x[i] - 'a']) % mod;
    return ans;
}

void Insert(char* path, int l, int pos, node* hashtable, int s_i){
    node cur = (node) malloc(sizeof(st_node));
    cur->len = l;
    cur->path = (char*) malloc(strlen(path) + 1);
    strcpy(cur->path, path);
    cur->s_index = s_i;
    cur->next = hashtable[pos]->next;
    hashtable[pos]->next = cur;
}

int get(node* hashtable, char* cmp, int pos){
    node cur = hashtable[pos]->next;
    while(cur != NULL){
        if(strcmp(cur->path, cmp) == 0)
            return cur->s_index;
        cur = cur->next;
    }
    return -1;
}

void Delete(char* path, int pos, node* hashtable) {
    node cur = hashtable[pos]->next;
    node pre = NULL;

    while (cur != NULL) {
        if (!strcmp(cur->path, path)) {
            if (pre == NULL) {
                hashtable[pos] = cur->next;
            } else {
                pre->next = cur->next;
            }

            return;
        }
        pre = cur;
        cur = cur->next;
    }
}