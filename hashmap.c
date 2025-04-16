#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "hashmap.h"


typedef struct HashMap HashMap;
int enlarge_called=0;

struct HashMap {
    Pair ** buckets;
    long size; //cantidad de datos/pairs en la tabla
    long capacity; //capacidad de la tabla
    long current; //indice del ultimo dato accedido
};

Pair * createPair( char * key,  void * value) {
    Pair * new = (Pair *)malloc(sizeof(Pair));
    new->key = key;
    new->value = value;
    return new;
}

long hash( char * key, long capacity) {
    unsigned long hash = 0;
     char * ptr;
    for (ptr = key; *ptr != '\0'; ptr++) {
        hash += hash*32 + tolower(*ptr);
    }
    return hash%capacity;
}

int is_equal(void* key1, void* key2){
    if(key1==NULL || key2==NULL) return 0;
    if(strcmp((char*)key1,(char*)key2) == 0) return 1;
    return 0;
}


void insertMap(HashMap * map, char * key, void * value) {
    unsigned long index = hash(key, map->capacity);
    unsigned long originalIndex = index; //Guarda la pos en donde debería insertarse
    
    do{
        Pair *bucket = map->buckets[index]; //sea accede al bucket que esta en esa pos
        
        if(bucket == NULL || bucket->key == NULL){
            Pair *newPair = malloc(sizeof(Pair));
            newPair->key = strdup(key);
            newPair->value = value;

            map->buckets[index] = newPair;
            map->current = index;
            map->size++;
            return;
        }

        if(strcmp(bucket->key, key) == 0) return; //La clave ya existia

        index = (index + 1) % map->capacity; //En caso de colision se le busca el siguiente
    }while(originalIndex != index);
}

void enlarge(HashMap * map) {
    enlarge_called = 1; //no borrar (testing purposes)

    Pair **old_bucket = map->buckets;
    map->capacity *= 2;

    map->buckets = (Pair **) calloc(map->capacity, sizeof(Pair*));

    map->size = 0;
    map->current = -1;

    for(long i = 0; i < (map->capacity/2); i++){
        if(old_bucket[i] != NULL || old_bucket[i]->key != NULL) insertMap(map, old_bucket[i]->key, old_bucket[i]->value);     
    }
    free(old_bucket);
}


HashMap * createMap(long capacity) {
    HashMap *map = (HashMap *) malloc(sizeof(HashMap));
    map->buckets = (Pair **) calloc(capacity, sizeof(Pair *));
    map->capacity = capacity;
    map->current = -1;
    map->size = 0;
    return map;
}

void eraseMap(HashMap * map,  char * key) {    
    
    Pair *par = searchMap(map, key);

    if(par != NULL ){
        par->key = NULL;
        map->size--;
    }
}

Pair * searchMap(HashMap * map,  char * key) {   
    unsigned long index = hash(key, map->capacity);
    unsigned long originalIndex = index; //Guarda la pos en donde debería estar la key buscada

    while(1){
        Pair *bucket = map->buckets[index];
        if(bucket == NULL) return NULL; //No esta la key

        if(bucket != NULL && strcmp(bucket->key, key) == 0){
            map->current = index;
            return bucket;
        }

        index = (index+1) % map->capacity;

        if(index == originalIndex) break;
    }
    
    return NULL;
}

Pair * firstMap(HashMap * map) {

    for(unsigned long i = 0; i < map->capacity;i++){
        Pair *bucket = map->buckets[i];
        if(bucket != NULL && bucket->key != NULL){
            map->current = i;
            return bucket;
        }    
    }
    return NULL;
}

Pair * nextMap(HashMap * map) {

    for(unsigned long i = (map->current + 1); i < map->capacity;i++){ // recorremos desde el current en vez desde la pos 0
        Pair *bucket = map->buckets[i];
        if(bucket != NULL && bucket->key != NULL){
            map->current = i;
            return bucket;
        }    
    }

    return NULL;
}
