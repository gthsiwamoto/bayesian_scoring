#ifndef CONTINGENCY_TABLE_H
#define CONTINGENCY_TABLE_H

#include <stdint.h>
#include "record.h"
#include "varset.h"

typedef struct{
    unsigned long long key;
    unsigned long long base_key;
    int index;
} index_map_t;

typedef struct{
    int unassigned_index;
    int max_cardinality;
    index_map_t *index_map;
    int *table;
} contingency_table_t;

int convert_index(contingency_table_t *ct, int attributes, int cardinality[], int index[], int read_only);
contingency_table_t create_contingency_table(record_t record, record_info_t record_info);

int contingency_table_sum(contingency_table_t ct, int variable, varset_t variables, int first[], int last[], int attributes, int cardinality[]);

void update_key(contingency_table_t *ct, int variable, varset_t variables, record_info_t info);


#endif // CONTINGENCY_TABLE_H
