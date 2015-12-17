#include <stdio.h>
#include <stdlib.h>
#include "score_calculator.h"


void calculate_scores(record_info_t info, contingency_table_t ct, double ess, char *bound){
    scratch_t *scratch_space = initialize_scratch(record_info);
    int bound_type = atoi(bound);
    for(int variable = 0; variable < info.attributes; variable++){
        printf("\n\nvariable: %d\n", variable);
        printf("layer: 0, pruned_count: 0\n");
        score_cache_t cache = create_score_cache(record_info.attributes);

        varset_t empty = 0;
        double score = calculate_bdeu_score(variable, empty, ess, info, ct, scratch_space, &cache, bound_type);

        if(score < 1)
            cache.scores[empty] = score;

        int pruned_count = 0;

        for(int layer = 1; layer < info.attributes; layer++){
            printf("layer: %d, pruned_count: %d\n", layer, pruned_count);
            varset_t variables = 0;
            for(int i = 0; i < layer; i++)
                varset_set(&variables, i);

            varset_t max = 0;
            varset_set(&max, info.attributes);

            while(variables < max){
                if(!varset_get(variables, variable)){
                    score = calculate_bdeu_score(variable, variables, ess, info, ct, scratch_space, &cache, bound_type);

                    if(score < 0)
                        cache.scores[variables] = score;
                    else{
                        pruned_count++;
                        cache.scores[variables] = 1;
                    }
                }
                varset_next(&variables);
            }
        }

        //free_score_cache(cache);
    }

    free(scratch_space);
}
