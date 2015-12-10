#include "varset.h"

int varset_get(varset_t var, int index){
    varset_t mask = 1 << index;
    return var & mask;
}

void varset_set(varset_t *var, int index){
    *var |= 1 << index;
}
