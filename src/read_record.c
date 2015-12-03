#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void read_record(char *filename){
    FILE *fstream = fopen(filename, "r");
    if(fstream == NULL){
        fprintf(stderr, "ERROR: Record file open failed.\n");
        exit(1);
    }
}
