#ifndef CONTINGENCY_TABLE_H
#define CONTINGENCY_TABLE_H

#include "record.h"

typedef struct{
    unsigned long key;
    int index;
} index_map_t;

typedef int * contingency_table_t;

int convert_index(int attributes, int cardinality[], int index[]);
contingency_table_t create_contingency_table(record_t record, record_info_t record_info);

#endif // CONTINGENCY_TABLE_H
