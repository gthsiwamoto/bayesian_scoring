#ifndef VARSET_H
#define VARSET_H

#include <stdint.h>

typedef uint64_t varset_t;

int varset_get(varset_t var, int index);
void varset_set(varset_t *var, int index);

#endif //VARSET_H
