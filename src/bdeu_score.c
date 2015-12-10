#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "bdeu_score.h"

void initialize_scratch(record_info_t info){
    scratch_space = (scratch_t *)malloc(info.attributes * sizeof(scratch_t));
    for(int i = 0; i < info.attributes; i++){
        scratch_t scratch;
        scratch.variable = i;
        scratch.l_r_i = log(info.cardinality[i]);
        scratch_space[i] = scratch;
    }
}

void lg(varset_t parents, scratch_t *scratch, record_info_t info, double ess){
    int r = 1;
    for(int pa = 0; pa < info.attributes; pa++)
        if(varset_get(parents, pa))
            r *= info.cardinality[pa];

    scratch->a_ij = ess / r;
    scratch->lg_ij = lgamma(scratch->a_ij);

    r *= info.cardinality[scratch->variable];
    scratch->a_ijk = ess / r;
    scratch->lg_ijk = lgamma(scratch->a_ijk);
}

void calculate_internal(varset_t variables, scratch_t *scratch){
}

int next_index(int index[], int variable, varset_t variables, record_info_t info){
    int start = info.attributes;
    for(int i = 0; i < info.attributes; i++)
        if(varset_get(variables, i))
            start--;

    if(index[start] < info.cardinality[variable] - 1){
        index[start]++;
        return 1;
    }else
        index[start++] = 0;
    for(int i = info.attributes - 1; i >= 0; i--){
        if(i != variable)
            if(varset_get(variables, i)){
                if(index[start] < info.cardinality[i] - 1){
                    index[start++]++;
                    return 0;
                }else
                    index[start++] = 0;
            }
    }
    return -1;
}

void last_index(int last[], int first_index[], varset_t variables, record_info_t info){
    int current_index = 0;
    for(int i = 0; i < info.attributes; i++){
        last[i] = first_index[i];
        if(varset_get(variables, i))
            last[current_index++] = info.cardinality[i] - 1;
    }
}

double calculate_bdeu_score(int variable, varset_t parents, double ess, record_info_t info, contingency_table_t ct){
    scratch_t scratch = scratch_space[variable];
    lg(parents, &scratch, info, ess);
    varset_t variables = parents;
    varset_set(&variables, variable);

    scratch.score = 0;
    int *index = (int *)malloc(info.attributes * sizeof(int));
    int *last = (int *)malloc(info.attributes * sizeof(int));
    for(int i = 0; i < info.attributes; i++){
        index[i] = 0;
        last[i] = 0;
    }
    update_key(&ct, variable, variables, info);
    int pa_count = 0;
    int total = 0;
    while(1){
        last_index(last, index, ~variables, info);
        printf("\n");
        for(int i = 0; i < info.attributes; i++)
            printf("%d", index[i]);
        printf("\n");
        for(int i = 0; i < info.attributes; i++)
            printf("%d", last[i]);
        printf("\n");
        int count = contingency_table_sum(ct, variable, variables, index, last, info.attributes, info.cardinality);
        scratch.score += lgamma(scratch.a_ijk + count);
        scratch.score -= scratch.lg_ijk;
        printf("count: %d, score %f\n", count, scratch.score);
        int next_status = next_index(index, variable, variables, info);
        pa_count += count;
        total += count;
        if(next_status == 1)
            ;
        else if(next_status == 0){
            printf("pa_count: %d\n", pa_count);
            scratch.score += scratch.lg_ij;
            scratch.score -= lgamma(scratch.a_ij + pa_count);
            pa_count = 0;
        }else{
            printf("pa_count: %d\n", pa_count);
            scratch.score += scratch.lg_ij;
            scratch.score -= lgamma(scratch.a_ij + pa_count);
            break;
        }
    }

    /*
    if(parents != variables)
        while(1){
            last_index(last, index, ~parents, info);
            sort_index(index, sorted_index, parents, info);
            sort_index(last, sorted_last, parents, info);
            for(int i = 0; i < info.attributes; i++)
                printf("%d", sorted_index[i]);
            printf("\n");
            for(int i = 0; i < info.attributes; i++)
                printf("%d", sorted_last[i]);
            printf("\n");
            int count = contingency_table_sum(ct, sorted_index, sorted_last, info.attributes, info.cardinality);
            scratch.score += scratch.lg_ij;
            scratch.score -= lgamma(scratch.a_ij + count);
            printf("count: %d, score %f\n", count, scratch.score);
            if(!next_index(index, parents, info))
                break;
        }
    else{
        scratch.score += scratch.lg_ij;
        scratch.score -= lgamma(scratch.a_ij + info.instances);
    }
    */
    printf("total: %d, score: %f\n", total, scratch.score);

    free(index);
    free(last);
    /*
    printf("l_r_i: %f, lg_ij: %f, lg_ijk: %f, a_ij: %f, a_ijk:%f\n", scratch.l_r_i, scratch.lg_ij, scratch.lg_ijk, scratch.a_ij, scratch.a_ijk);
    */
    return scratch.score;
}
