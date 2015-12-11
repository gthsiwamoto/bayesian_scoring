#ifndef SCORE_CALCULATOR_H
#define SCORE_CALCULATOR_H

#include "bdeu_score.h"
#include "contingency_table.h"
#include "score_cache.h"
#include "varset.h"

void calculate_scores(record_info_t info, contingency_table_t ct, double ess, char *bound);

#endif //SCORE_CALCULATOR_H
