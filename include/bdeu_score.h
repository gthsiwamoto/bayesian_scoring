#ifndef BDEU_SCORE_H
#define BDEU_SCORE_H

#include "varset.h"
#include "record.h"
#include "contingency_table.h"
#include "score_cache.h"

typedef struct{
    int variable;
    double l_r_i;
    double lg_ij;
    double lg_ijk;
    double a_ij;
    double a_ijk;
} scratch_t;

scratch_t *initialize_scratch(record_info_t info);

double calculate_bdeu_score(int variable, varset_t parents, double ess, record_info_t info, contingency_table_t ct, scratch_t scratch_space[], score_cache_t *cache, int bound_type);

#endif //BDEU_SCORE_H
