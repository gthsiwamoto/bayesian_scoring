#include <stdio.h>
#include <stdlib.h>
#include "score_cache.h"

score_cache_t create_score_cache(int attributes){
    score_cache_t score_cache;
    score_cache.scores = (double *)malloc((1 << attributes) * sizeof(double));
    return score_cache;
}

void free_score_cache(score_cache_t cache){
    free(cache.scores);
}
