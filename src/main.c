#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "read_record.h"

int main(int argc, char **argv){
    int c;
    char *bound = NULL;
    char *record_file;

    if(argc == 1){
        fprintf(stderr, "ERROR: You need arguments and options.\n");
        exit(1);
    }
    while((c = getopt(argc, argv, "b:")) != -1)
        switch(c){
            case 'b':
                bound = optarg;
                break;
            case '?':
                if(optopt == 'b')
                    fprintf(stderr, "ERROR: Option -%c requires an argument.\n", optopt);
                else
                    fprintf(stderr, "ERROR: Unknown option '-%c'.\n", optopt);
                exit(1);
            default:
                fprintf(stderr, "ERROR: You need option '-b bound'.\n");
                exit(1);
        }
    if(argc - optind != 1){
        fprintf(stderr, "ERROR: Argument error.");
        exit(1);
    }

    record_file = argv[optind];

    printf("record_file: %s\n", record_file);
    printf("bound: %s\n", bound);

    read_record(record_file);

    return 0;
}
