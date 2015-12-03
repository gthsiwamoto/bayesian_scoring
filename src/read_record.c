#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "read_record.h"


int count_attributes(char *row){
    int count = 0;
    for(int i = 0; row[i] != '\0'; i++)
        if(row[i] == ',')
            count++;
    return count + 1;
}

void read_record(char *filename){
    FILE *fstream = fopen(filename, "r");
    if(fstream == NULL){
        fprintf(stderr, "ERROR: Record file open failed.\n");
        exit(1);
    }

    char buffer[1024];
    int n = 0;
    while(fgets(buffer, sizeof(buffer), fstream) != NULL){
        n++;
    }
    record = (char ***)malloc(n * sizeof(char **));
    fclose(fstream);
    fstream = fopen(filename, "r");

    char *cell;
    int attributes = 0;
    int row = 0;
    int column = 0;
    while(fgets(buffer, sizeof(buffer), fstream) != NULL){
        buffer[strlen(buffer) - 1] = '\0';
        attributes = count_attributes(buffer);
        record[row] = (char **)malloc(attributes * sizeof(char *));
        column = 0;
        cell = strtok(buffer, ",");
        while(cell != NULL){
            record[row][column] = (char *)malloc(sizeof(cell));
            strcpy(record[row][column++], cell);
            cell = strtok(NULL, ",");
        }
        row++;
    }
    fclose(fstream);
}
