#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "bdeu_score.h"

scratch_t *initialize_scratch(record_info_t info){
    scratch_t *scratch_space = (scratch_t *)malloc(info.attributes * sizeof(scratch_t));
    for(int i = 0; i < info.attributes; i++){
        scratch_t scratch;
        scratch.variable = i;
        scratch.l_r_i = log(info.cardinality[i]);
        scratch_space[i] = scratch;
    }
    return scratch_space;
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

    for(int i = info.attributes - 1; i >= 0; i--)
        if(i != variable && varset_get(variables, i)){
            if(index[start] < info.cardinality[i] - 1){
                index[start++]++;
                return 0;
            }else
                index[start++] = 0;
        }
    
    return -1;
}

int next_index_by_map_index(int index[], int variable, varset_t variables, record_info_t info, int map_index, contingency_table_t ct, int *difference, int *pa_difference){
    if(map_index == ct.unassigned_index)
        return -1;

    int *index_before = (int *)malloc(info.attributes * sizeof(int));
    unsigned long long key = ct.index_map[map_index].key;
    int current_index = 0;
    for(int i = 0; i < info.attributes; i++){
        if(!varset_get(variables, i)){
            index_before[current_index] = index[current_index];
            index[current_index++] = 0;
            if(key != 0)
                key /= info.cardinality[i];
        }
    }
    int index_memo = current_index;
    index_before[current_index] = index[current_index];
    index[current_index++] = key % info.cardinality[variable];
    if(key != 0)
        key /= info.cardinality[variable];
    for(int i = info.attributes - 1; i >= 0; i--){
        if(varset_get(variables, i) && i != variable){
            index_before[current_index] = index[current_index];
            index[current_index++] = key % info.cardinality[i];
            if(key != 0)
                key /= info.cardinality[i];
        }
    }

    /*
    for(int i = 0; i < info.attributes; i++)
        printf("%d", index_before[i]);
    printf("\n");
    for(int i = 0; i < info.attributes; i++)
        printf("%d", index[i]);
    printf("\n");
    */

    // calculate difference
    current_index = index_memo;
    int borrow = 0;
    int pa_borrow = 0;
    *difference = index[current_index] - index_before[current_index++];
    *pa_difference = 0;
    if(*difference < 0){
        borrow = 1;
        *difference += info.cardinality[variable];
    }
    int digit = info.cardinality[variable];
    int pa_digit = 1;
    for(int i = info.attributes - 1; i >= 0; i--){
        if(varset_get(variables, i) && i != variable){
            int tmp = index[current_index] - index_before[current_index] - borrow;
            int pa_tmp = index[current_index] - index_before[current_index++] - pa_borrow;
            if(tmp < 0){
                tmp += info.cardinality[i];
                borrow = 1;
            }else{
                borrow = 0;
            }

            if(pa_tmp < 0){
                pa_tmp += info.cardinality[i];
                pa_borrow = 1;
            }else{
                pa_borrow = 0;
            }
            *difference += digit * tmp;
            *pa_difference += pa_digit * pa_tmp;
            digit *= info.cardinality[i];
            pa_digit *= info.cardinality[i];
        }
    }
    if(*difference > info.cardinality[variable] && *pa_difference > 1){
        /*
        printf("==================================\n");
        for(int i = 0; i < info.attributes; i++)
            printf("%d", index_before[i]);
        printf("\n");
        for(int i = 0; i < info.attributes; i++)
            printf("%d", index[i]);
        printf("\n");
        printf("cardinality: %d\n", info.cardinality[variable]);
        printf("difference: %d, pa_difference: %d\n", *difference, *pa_difference);
        printf("要修正\n");
        printf("----------------------------------\n");
        */
        *pa_difference = *pa_difference - 1;
        *difference = info.cardinality[variable] * *pa_difference;
        /*
        printf("difference: %d, pa_difference: %d\n", *difference, *pa_difference);
        printf("==================================\n");
        */
    }else{
        *difference = 0;
        *pa_difference = 0;
    }


    for(int i = index_memo + 1; i < info.attributes; i++){
        if(index_before[i] != index[i]){
            free(index_before);
            return 0;
        }
    }

    free(index_before);
    return 1;

}

void last_index(int last[], int first_index[], varset_t variables, record_info_t info){
    int current_index = 0;
    for(int i = 0; i < info.attributes; i++){
        last[i] = first_index[i];
        if(varset_get(variables, i))
            last[current_index++] = info.cardinality[i] - 1;
    }
}

double calculate_bdeu_score(int variable, varset_t parents, double ess, record_info_t info, contingency_table_t ct, scratch_t scratch_space[], score_cache_t *cache, int bound_type){
    // branh and bound
    double lower_bound = 0;
    for(int i = 0; i < info.attributes; i++){
        if(varset_get(parents, i)){
            varset_clear(&parents, i);
            double tmp = cache->scores[parents];
            if((tmp > lower_bound && tmp < 0) || lower_bound == 0)
                lower_bound = tmp;
            if(tmp == 1)
                return 0;
            varset_set(&parents, i);
        }
    }

    scratch_t scratch = scratch_space[variable];
    lg(parents, &scratch, info, ess);

    varset_t variables = parents;
    varset_set(&variables, variable);

    int *index = (int *)malloc(info.attributes * sizeof(int));
    int *last = (int *)malloc(info.attributes * sizeof(int));
    for(int i = 0; i < info.attributes; i++){
        index[i] = 0;
        last[i] = 0;
    }

    update_keys(&ct, variable, variables, info);

    int pa_count = 0;
    int total = 0;
    double score = 0;
    double new_bound = 0;
    double bound = 0;
    int map_index = 0;
    int map_index_tmp = 0;
    int hoge = 0;
    while(1){
        hoge++;
        map_index_tmp = map_index;
        last_index(last, index, ~variables, info);

        int count = contingency_table_sum(ct, variable, variables, index, last, info, &map_index);
        if(count >= 1){
            bound -= scratch.l_r_i;
            for(int i = 1; i <= count; i++)
                new_bound += log(i - 1 + scratch.a_ijk) - log(i - 1 + info.cardinality[variable] * scratch.a_ijk);
        }


        score += lgamma(scratch.a_ijk + count);
        score -= scratch.lg_ijk;
        pa_count += count;
        total += count;
        //printf("coucount: %d\n", count);

        int next_status = 5;
        if(map_index == map_index_tmp)
            next_status = next_index(index, variable, variables, info);
        else{
            int difference = 0;
            int pa_difference = 0;
            next_status = next_index_by_map_index(index, variable, variables, info, map_index, ct, &difference, &pa_difference);
            for(int i = 0; i < difference; i++){
                score += lgamma(scratch.a_ijk);
                score -= scratch.lg_ijk;
            }
            for(int i = 0; i < pa_difference; i++){
                score += scratch.lg_ij;
                score -= lgamma(scratch.a_ij);
            }
        }

        if(next_status != 1){
            //printf("pacount: %d\n", pa_count);
            score += scratch.lg_ij;
            score -= lgamma(scratch.a_ij + pa_count);
            
            if(next_status == 0)
                pa_count = 0;
            else if(next_status == -1)
                break;
        }
    }
    //printf("hoge: %d\n", hoge);
    
    // prune
    // bound と new_boundは単調減少
    //printf("bound: %f, new_bound: %f\n", bound, new_bound);
    if(bound_type == 1 && scratch.a_ij <= 0.8349 && lower_bound > bound && lower_bound < 0)
        return 0;

    if(bound_type == 0 && scratch.a_ij <= 0.8349 && lower_bound > new_bound && lower_bound < 0)
        return 0;

    // error check
    if(total != info.instances){
        for(int i = 0; i < info.attributes; i++)
            if(varset_get(variables, i))
                printf("1");
            else
                printf("0");
        printf("\n");
        printf("total: %d, variables: %ld\n", total, variables);
        printf("Error\n");
        exit(1);
    }


    free(index);
    //free(last);

    if(score > 0){
        printf("score: %f\n", score);
        printf("error\n");
        exit(1);
    }
    return score;
}

double calculate_bdeu_score_cp(int variable, varset_t parents, double ess, record_info_t info, contingency_table_t ct, scratch_t scratch_space[], score_cache_t *cache, int bound_type){
    // branh and bound
    double lower_bound = 0;
    /*
    for(int i = 0; i < info.attributes; i++){
        if(varset_get(parents, i)){
            varset_clear(&parents, i);
            double tmp = cache->scores[parents];
            if((tmp > lower_bound && tmp < 0) || lower_bound == 0)
                lower_bound = tmp;
            if(tmp == 1)
                return 0;
            varset_set(&parents, i);
        }
    }
    */

    scratch_t scratch = scratch_space[variable];
    lg(parents, &scratch, info, ess);

    varset_t variables = parents;
    varset_set(&variables, variable);

    int *index = (int *)malloc(info.attributes * sizeof(int));
    int *last = (int *)malloc(info.attributes * sizeof(int));
    for(int i = 0; i < info.attributes; i++){
        index[i] = 0;
        last[i] = 0;
    }

    update_keys(&ct, variable, variables, info);

    int pa_count = 0;
    int total = 0;
    double score = 0;

    double new_bound = 0;
    double bound = 0;
    //int sn = 0; // for bound
    double score_tmp = 0;
    double new_bound_tmp = 0;
    int hoge = 0;
    int foo = 0;
    while(1){
        hoge++;
        last_index(last, index, ~variables, info);

        int count = contingency_table_sum(ct, variable, variables, index, last, info, &foo);
        if(count >= 1){
            //sn++;
            //double bound = -1.0 * sn * scratch.l_r_i;
            bound -= scratch.l_r_i;
            for(int i = 1; i <= count; i++)
                new_bound += log(i - 1 + scratch.a_ijk) - log(i - 1 + info.cardinality[variable] * scratch.a_ijk);
        }

        // prune
        // bound と new_boundは単調減少
        /*
        if(bound_type == 1 && scratch.a_ij <= 0.8349 && lower_bound > bound && lower_bound < 0)
            return 0;

        if(bound_type == 0 && scratch.a_ij <= 0.8349 && lower_bound > new_bound && lower_bound < 0)
            return 0;
        */

        score += lgamma(scratch.a_ijk + count);
        score -= scratch.lg_ijk;

        int next_status = next_index(index, variable, variables, info);
        pa_count += count;
        printf("count: %d\n", count);
        total += count;
        if(next_status != 1){
            printf("pacount: %d\n", pa_count);
            score += scratch.lg_ij;
            score -= lgamma(scratch.a_ij + pa_count);

            // scoreは単調減少より
            /*
            if(lower_bound > score && lower_bound < 0){
                //printf("lower_bound: %f, score: %f\n", lower_bound, score);
                return lower_bound;
            }
            */
            
            if(next_status == 0)
                pa_count = 0;
            else if(next_status == -1)
                break;
        }
    }
    printf("hoge: %d\n", hoge);

    // error check
    if(total != info.instances){
        for(int i = 0; i < info.attributes; i++)
            if(varset_get(variables, i))
                printf("1");
            else
                printf("0");
        printf("\n");
        printf("total: %d, variables: %ld\n", total, variables);
        exit(1);
    }


    free(index);
    //free(last);

    printf("score: %f\n", score);
    return score;
}

