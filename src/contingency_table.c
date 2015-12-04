#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contingency_table.h"
#include "record.h"

int unassigned_index = 0;
index_map_t *index_map;

int compare_index_map(const void *a, const void *b){
    unsigned long a_key = (*(const index_map_t *)a).key;
    unsigned long b_key = (*(const index_map_t *)b).key;
    if(a_key < b_key)
        return -1;
    else if(a_key == b_key)
        return 0;
    else
        return 1;
}

int get_index_by_key(index_map_t *map, int size, unsigned long key){
    if(size == 0)
        return -1;
    int min = 0;
    int max = size;
    int mid;
    while(min <= max){
        mid = (min + max) / 2;
        if(map[mid].key == key){
            return map[mid].index;
        }else if(map[mid].key < key)
            min = mid + 1;
        else
            max = mid - 1;
    }
    return -1;
}

int convert_index(int attributes, int cardinality[], int index[]){
    unsigned long key = index[0];
    int max_cardinality = cardinality[0];
    for(int i = 1; i < attributes; i++){
        if(max_cardinality < cardinality[i])
            max_cardinality = cardinality[i];
    }
    for(int i = 1; i < attributes; i++)
        key = key * max_cardinality + index[i];
    int converted_index = get_index_by_key(index_map, unassigned_index, key);
    if(converted_index == -1){
        index_map[unassigned_index].key = key;
        index_map[unassigned_index].index = unassigned_index;
        unassigned_index++;
        qsort(index_map, unassigned_index, sizeof(index_map_t), compare_index_map);
        return unassigned_index - 1;
    }else{
        return converted_index;
    }
}

contingency_table_t create_contingency_table(record_t record, record_info_t record_info){
    contingency_table_t ct;
    int attributes = record_info.attributes;
    int instances = record_info.instances;
    /*
    unsigned long long s = 1;
    for(int i = 0; i < attributes; i++){
        s *= record_info.cardinality[i];
        printf("%llu\n", s);
    }
    */
    ct = malloc(instances * sizeof(int));
    index_map = malloc(instances * sizeof(index_map_t));
    for(int i = 0; i < instances; i++){
        ct[i] = 0;
    }

    int *index = (int *)malloc(attributes * sizeof(int));
    for(int i = 0; i < instances; i++){
        for(int j = 0; j < attributes; j++){
            index[j] = atoi(record[i][j]);
        }
        ct[convert_index(attributes, record_info.cardinality, index)]++;
    }
    free(index);
    return ct;
}
