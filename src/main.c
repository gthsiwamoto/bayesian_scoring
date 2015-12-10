#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "record.h"
#include "contingency_table.h"
#include "bdeu_score.h"
#include "varset.h"
#include "score_cache.h"

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

    record_file = argv[optind];

    printf("record_file: %s\n", record_file);
    printf("bound: %s\n", bound);

    record_t record = read_record(record_file);
    /*
    for(int i = 0; i < record_info.instances; i++){
        for(int j = 0; j < record_info.attributes; j++)
            printf("%s ", record[i][j]);
        printf("\n");
    }
    */

    contingency_table_t ct = create_contingency_table(record, record_info);

    //score_cache_t cache = create_score_cache(record_info.attributes);
    /*
    for(int i = 0; i < record_info.instances; i++)
        printf("%ld\n", ct.index_map[i].key);
    */

    initialize_scratch(record_info);

    varset_t parents = 0;
    varset_set(&parents, 1);
    calculate_bdeu_score(0, parents, ess, record_info, ct);

    /*
    printf("\n");
    parents = 0;
    varset_set(&parents, 0);
    calculate_bdeu_score(0, parents, ess, record_info, ct);

    printf("\n");
    parents = 0;
    varset_set(&parents, 1);
    varset_set(&parents, 2);
    calculate_bdeu_score(0, parents, ess, record_info, ct);

    printf("\n");
    parents = 0;
    varset_set(&parents, 1);
    calculate_bdeu_score(1, parents, ess, record_info, ct);
    */
    printf("attributes: %d, instances: %d, unassigned: %d, max_cardinality: %d\n", record_info.attributes, record_info.instances, ct.unassigned_index, ct.max_cardinality);
    return 0;
}
