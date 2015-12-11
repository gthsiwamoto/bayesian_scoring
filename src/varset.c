#include "varset.h"

int varset_get(varset_t var, int index){
    varset_t mask = 1 << index;
    return var & mask;
}

void varset_set(varset_t *var, int index){
    *var |= 1 << index;
}

void varset_clear(varset_t *var, int index){
    *var -= 1 << index;
}

void varset_next(varset_t *var){
    varset_t variables = *var;
    varset_t tmp = (variables | (variables - 1)) + 1;
    *var = tmp | ((((tmp & -tmp ) / (variables & -variables)) >> 1) - 1);
}
