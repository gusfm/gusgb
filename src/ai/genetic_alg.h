#ifndef __GENETIC_ALG_H__
#define __GENETIC_ALG_H__

typedef struct {
    unsigned int chromo_length;
    const double *mom;
    const double *dad;
    double *child1;
    double *child2;
} crossover_t;

void ga_crossover(crossover_t *cross);

#endif /* __GENETIC_ALG_H__ */
