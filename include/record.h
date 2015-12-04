#ifndef RECORD_H
#define RECORD_H

typedef char *** record_t;

typedef struct{
    int instances;
    int attributes;
    int *cardinality;
} record_info_t;

record_info_t record_info;

record_t read_record(char *filename);

int compare_string(const void *a, const void *b);

#endif // RECORD_H
