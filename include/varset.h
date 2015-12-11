#ifndef VARSET_H
#define VARSET_H

#include <stdint.h>

typedef unsigned long varset_t;

int varset_get(varset_t var, int index);
void varset_set(varset_t *var, int index);
void varset_clear(varset_t *var, int index);
void varset_next(varset_t *var);

#endif //VARSET_H
