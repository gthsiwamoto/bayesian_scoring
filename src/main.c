#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "record.h"
#include "contingency_table.h"
#include "bdeu_score.h"
#include "varset.h"
#include "score_cache.h"
#include "score_calculator.h"

int main(int argc, char **argv){
    double ess = 1.0;
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

    clock_t start = clock();

    record_file = argv[optind];

    printf("record_file: %s\n", record_file);
    printf("bound: %s\n", bound);

    record_t record = read_record(record_file);

    contingency_table_t ct = create_contingency_table(record, record_info);

    calculate_scores(record_info, ct, ess, bound);

    clock_t end = clock();
    printf("\ntime: %f\n", (double)(end - start) / CLOCKS_PER_SEC);

    return 0;
}
