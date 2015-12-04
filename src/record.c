#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "record.h"

int compare_string(const void *a, const void *b){
    return strcmp(*(const char **)a, *(const char **)b);
}

// recordの値を数値に変える
int *get_cardinality(record_t record, int instances, int attributes){
    int *cardinality = (int *)malloc(attributes * sizeof(int));

    char **columns = (char **)malloc(instances * sizeof(char *));
    for(int i = 0; i < instances; i++)
        columns[i] = (char *)malloc(sizeof(char *));

    for(int i = 0; i < attributes; i++){
        int index = 0;
        for(int j = 0; j < instances; j++)
            //strcpy(columns[j], record[j][i]);
            columns[j] = record[j][i];
        qsort(columns, instances, sizeof(char *), compare_string);
        int count = 1;
        int k = 0;
        for(k = 1; k < instances; k++){
            if(strcmp(columns[k - 1],columns[k]) != 0){
                count++;
                index++;
                sprintf(columns[k - 1], "%d", index - 1);
            }else
                sprintf(columns[k - 1], "%d", index);
        }
        sprintf(columns[k - 1], "%d", index);
        cardinality[i] = count;
    }

    /*
    for(int i = 0; i < instances; i++){
        free(columns[i]);
    }
    */
    free(columns);
    return cardinality;
}

int count_attributes(char *row){
    int count = 0;
    for(int i = 0; row[i] != '\0'; i++)
        if(row[i] == ',')
            count++;
    return count + 1;
}

record_t read_record(char *filename){
    FILE *fstream = fopen(filename, "r");
    if(fstream == NULL){
        fprintf(stderr, "ERROR: Record file open failed.\n");
        exit(1);
    }

    char buffer[1024];
    int instances = 0;
    while(fgets(buffer, sizeof(buffer), fstream) != NULL){
        instances++;
    }
    record_t record = (char ***)malloc(instances * sizeof(char **));
    fclose(fstream);
    fstream = fopen(filename, "r");

    char *cell;
    int row = 0;
    int column = 0;
    int attributes = 0;
    while(fgets(buffer, sizeof(buffer), fstream) != NULL){
        buffer[strlen(buffer) - 1] = '\0';
        attributes = count_attributes(buffer);
        record[row] = (char **)malloc(attributes * sizeof(char *));
        column = 0;
        cell = strtok(buffer, ",");
        while(cell != NULL){
            record[row][column] = (char *)malloc(sizeof(char[1024]));
            strcpy(record[row][column++], cell);
            cell = strtok(NULL, ",");
        }
        row++;
    }
    fclose(fstream);

    record_info.instances = instances;
    record_info.attributes = attributes;
    record_info.cardinality = get_cardinality(record, instances, attributes);
    return record;
}
