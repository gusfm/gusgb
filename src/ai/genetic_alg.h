#ifndef __GENETIC_ALG_H__
#define __GENETIC_ALG_H__

void ga_crossover1(unsigned int chromo_length, const double *mom,
                   const double *dad, double *child1, double *child2);
void ga_crossover2(unsigned int chromo_length, const double *mom,
                   const double *dad, double *child1, double *child2);

#endif /* __GENETIC_ALG_H__ */
