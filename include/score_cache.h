#ifndef SCORE_CACHE_H
#define SCORE_CACHE_H

typedef struct{
    unsigned long key;
    int index;
} score_cache_index_t;

typedef struct{
    score_cache_index_t *score_cache_index;
    double *scores;
} score_cache_t;

score_cache_t create_score_cache(int attributes);

#endif // SCORE_CACHE_H
