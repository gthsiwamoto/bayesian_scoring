#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "contingency_table.h"
#include "record.h"

int compare_index_map(const void *a, const void *b){
    unsigned long long a_key = (*(const index_map_t *)a).key;
    unsigned long long b_key = (*(const index_map_t *)b).key;
    if(a_key < b_key)
        return -1;
    else if(a_key == b_key)
        return 0;
    else
        return 1;
}

int get_index_by_base_key(index_map_t *map, int size, unsigned long long key){
    if(size == 0)
        return -1;
    int min = 0;
    int max = size;
    int mid;
    while(min <= max){
        mid = (min + max) / 2;
        if(map[mid].base_key == key){
            return map[mid].index;
        }else if(map[mid].base_key < key)
            min = mid + 1;
        else
            max = mid - 1;
    }
    return -1;
}

int convert_index(contingency_table_t *ct, int attributes, int cardinality[], int index[], int read_only){
    unsigned long long key = 0;
    unsigned long long digit = 1;
    for(int i = 0; i < attributes; i++){
        // edited
        key += digit * index[i];
        digit *= cardinality[i];
    }
    int converted_index = get_index_by_base_key(ct->index_map, ct->unassigned_index, key);
    if(converted_index == -1 && read_only)
        return -1;
    if(converted_index == -1){
        ct->index_map[ct->unassigned_index].key = key;
        ct->index_map[ct->unassigned_index].base_key = key;
        ct->index_map[ct->unassigned_index].index = ct->unassigned_index;
        (ct->unassigned_index)++;
        qsort(ct->index_map, ct->unassigned_index, sizeof(index_map_t), compare_index_map);
        return ct->unassigned_index - 1;
    }else{
        return converted_index;
    }
}

contingency_table_t create_contingency_table(record_t record, record_info_t record_info){
    int attributes = record_info.attributes;
    int instances = record_info.instances;
    contingency_table_t ct;

    ct.unassigned_index = 0;
    ct.max_cardinality = record_info.cardinality[0];
    for(int i = 1; i < attributes; i++){
        if(ct.max_cardinality < record_info.cardinality[i])
            ct.max_cardinality = record_info.cardinality[i];
    }
    ct.table = malloc(instances * sizeof(int));
    ct.index_map = malloc(instances * sizeof(index_map_t));
    for(int i = 0; i < instances; i++){
        ct.table[i] = 0;
    }

    int *index = (int *)malloc(attributes * sizeof(int));
    for(int i = 0; i < instances; i++){
        for(int j = 0; j < attributes; j++){
            index[j] = atoi(record[i][j]);
        }
        ct.table[convert_index(&ct, attributes, record_info.cardinality, index, 0)]++;
    }
    free(index);
    return ct;
}

int get_first_index_by_key(index_map_t *map, int size, unsigned long long key){
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
    return min;
}

int get_last_index_by_key(index_map_t *map, int size, unsigned long long key){
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
    return max;
}

int contingency_table_sum(contingency_table_t ct, int variable, varset_t variables, int first[], int last[], int attributes, int cardinality[]){
    unsigned long long key_first = 0;
    unsigned long long key_last = 0;
    unsigned long long digit = 1;

    int count = 0;
    for(int i = 0; i < attributes; i++){
        if(!varset_get(variables, i))
            count++;
    }
    
    for(int i = 0; i < count; i++){
        key_last += digit * last[i];
        digit *= last[i] + 1;
    }

    int parent_index = count + 1;
    unsigned long long digit_memo = 0;
    for(int i = attributes - 1; i >= 0; i--){
        if(i != variable){
            if(varset_get(variables, i)){
                key_first += digit * first[parent_index];
                key_last += digit * last[parent_index];
                digit *= last[i] + 1;
                parent_index++;
            }
        }else{
            digit_memo = digit;
            digit *= cardinality[variable];
        }
    }
    key_first += digit_memo * first[count];
    key_last += digit_memo * last[count];
    //printf("key_first: %lld, key_last: %lld\n", key_first, key_last);

    int sum = 0;
    int index = get_first_index_by_key(ct.index_map, ct.unassigned_index, key_first);
    int last_index = get_last_index_by_key(ct.index_map, ct.unassigned_index, key_last);
    //printf("index: %d, last_index: %d\n", index, last_index);
    while(index <= last_index && index < ct.unassigned_index)
        sum += ct.table[index++];
    return sum;
}

void update_key(contingency_table_t *ct, int variable, varset_t variables, record_info_t info){
    unsigned long long end_memo = 1;
    for(int i = 0; i < info.attributes; i++)
        end_memo *= info.cardinality[i];
    for(int i = 0; i < ct->unassigned_index; i++){
        unsigned long long base_key = ct->index_map[i].base_key;
        unsigned long long begin = 1;
        unsigned long long end = end_memo;
        unsigned long long variable_key = 0;
        ct->index_map[i].key = 0;
        for(int j = 0; j < info.attributes; j++){
            if(j != variable){
                if(varset_get(variables, j)){
                    end /= info.cardinality[j];
                    ct->index_map[i].key += end * (base_key % info.cardinality[j]);
                }else{
                    ct->index_map[i].key += begin * (base_key % info.cardinality[j]);
                    begin *= info.cardinality[j];
                }
            }else
                variable_key = base_key % info.cardinality[variable];
            base_key /= info.cardinality[j];
        }
        ct->index_map[i].key += begin * variable_key;
    }
    qsort(ct->index_map, ct->unassigned_index, sizeof(index_map_t), compare_index_map);
    /*
    for(int i = 0; i < ct->unassigned_index; i++){
        printf("new_key: %lld, base_key: %lld\n", ct->index_map[i].key, ct->index_map[i].base_key);
        if(ct->index_map[i].key == ct->index_map[i + 1].key)
            printf("hogehogehogehogehogehoge\n");
    }
    */
}
