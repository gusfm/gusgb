#include "population.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "genetic_alg.h"
#include "random.h"

population_t *population_create(unsigned int num_players,
                                unsigned int num_inputs,
                                unsigned int num_outputs)
{
    if (num_players == 0 || num_players & 1) {
        fprintf(stderr,
                "%s: num_players should be an even number greater than zero\n",
                __func__);
        return NULL;
    }
    population_t *pop = (population_t *)malloc(sizeof(population_t));
    pop->num_players = num_players;
    pop->num_inputs = num_inputs;
    pop->num_outputs = num_outputs;
    pop->players = (player_t **)malloc(sizeof(player_t) * num_players);
    rand_start();
    player_t *player;
    for (unsigned int i = 0; i < num_players; ++i) {
        player = player_create(num_inputs, num_outputs);
        pop->players[i] = player;
    }
    pop->num_chromo = player_get_chromosome_lenght(player);
    pop->mom_weights = (double *)malloc(sizeof(double) * pop->num_chromo);
    pop->dad_weights = (double *)malloc(sizeof(double) * pop->num_chromo);
    pop->child1_weights = (double *)malloc(sizeof(double) * pop->num_chromo);
    pop->child2_weights = (double *)malloc(sizeof(double) * pop->num_chromo);
    return pop;
}

void population_destroy(population_t *pop)
{
    for (unsigned int i = 0; i < pop->num_players; ++i) {
        player_destroy(pop->players[i]);
    }
    free(pop->players);
    free(pop->mom_weights);
    free(pop->dad_weights);
    free(pop->child1_weights);
    free(pop->child2_weights);
}

static int compare_sort(const void *first, const void *second)
{
    unsigned int fitness1 = player_get_fitness(*(player_t **)first);
    unsigned int fitness2 = player_get_fitness(*(player_t **)second);
    /* When fitness values are equal, give preference to the second one. This
     * is useful for giving preference to children instead of parents. */
    if (fitness1 > fitness2) {
        return -1;
    } else {
        return 1;
    }
}

static void population_debug(population_t *pop)
{
    printf("Population fitness after round:\n");
    for (unsigned int i = 0; i < pop->num_players; ++i) {
        printf("Player %u: fitness %u age %u\n", i + 1,
               player_get_fitness(pop->players[i]),
               player_get_age(pop->players[i]));
    }
}

static void population_random_player(population_t *pop, unsigned int id)
{
    double *weights = pop->mom_weights;
    printf("Generating random player: %u\n", id + 1);
    player_get_chromosome(pop->players[id], weights, pop->num_chromo);
    /* Set random weights. */
    for (unsigned int w = 0; w < pop->num_chromo; ++w) {
        weights[w] = rand_clamp();
    }
    player_set_chromosome(pop->players[id], weights, pop->num_chromo);
}

void population_natural_selection(population_t *pop)
{
    /* Sort agents list. */
    qsort(pop->players, pop->num_players, sizeof(player_t *), compare_sort);
    population_debug(pop);
    /* Get half of population, and reproduce them. */
    unsigned int half_pop = pop->num_players / 2;
    for (unsigned int i = 0; i < half_pop; i += 2) {
        if (player_get_fitness(pop->players[i]) == 0 &&
            player_get_fitness(pop->players[i + 1]) == 0) {
            /* When the score of parents is zero, generate random players.*/
            population_random_player(pop, i);
            population_random_player(pop, i + 1);
            population_random_player(pop, half_pop + i);
            population_random_player(pop, half_pop + i + 1);
        } else {
            /* Get parent's weights. */
            player_get_chromosome(pop->players[i], pop->mom_weights,
                                  pop->num_chromo);
            player_get_chromosome(pop->players[i + 1], pop->dad_weights,
                                  pop->num_chromo);

            /* Crossover. */
            ga_crossover1(pop->num_chromo, pop->mom_weights, pop->dad_weights,
                          pop->child1_weights, pop->child2_weights);
            /* Save new child weights. */
            player_set_chromosome(pop->players[half_pop + i],
                                  pop->child1_weights, pop->num_chromo);
            player_set_chromosome(pop->players[half_pop + i + 1],
                                  pop->child2_weights, pop->num_chromo);
        }
    }
}

player_t *population_get_player(population_t *pop, unsigned int player_id)
{
    assert(player_id < pop->num_players);
    return pop->players[player_id];
}

void population_save(population_t *pop, const char *filename)
{
    FILE *f = fopen("population.out", "w");
    if (f == NULL) {
        fprintf(stderr, "ERROR: could not open %s\n", filename);
        return;
    }
    size_t ret = fwrite(&pop->num_players, sizeof(pop->num_players), 1, f);
    if (ret != 1) {
        fprintf(stderr, "ERROR: could not write num_players\n");
        fclose(f);
        return;
    }
    ret = fwrite(&pop->num_inputs, sizeof(pop->num_inputs), 1, f);
    if (ret != 1) {
        fprintf(stderr, "ERROR: could not write num_inputs\n");
        fclose(f);
        return;
    }
    ret = fwrite(&pop->num_outputs, sizeof(pop->num_outputs), 1, f);
    if (ret != 1) {
        fprintf(stderr, "ERROR: could not write num_outputs\n");
        fclose(f);
        return;
    }
    ret = fwrite(&pop->num_chromo, sizeof(pop->num_chromo), 1, f);
    if (ret != 1) {
        fprintf(stderr, "ERROR: could not write num_chromo\n");
        fclose(f);
        return;
    }
    for (unsigned int i = 0; i < pop->num_players; ++i) {
        player_t *p = pop->players[i];
        unsigned int age = player_get_age(p);
        ret = fwrite(&age, sizeof(age), 1, f);
        if (ret != 1) {
            fprintf(stderr, "ERROR: could not write age\n");
            fclose(f);
            return;
        }
        player_get_chromosome(pop->players[i], pop->mom_weights,
                              pop->num_chromo);
        for (unsigned w = 0; w < pop->num_chromo; ++w) {
            ret =
                fwrite(&pop->mom_weights[w], sizeof(pop->mom_weights[0]), 1, f);
            if (ret != 1) {
                fprintf(stderr, "ERROR: could not write weights %u\n", w);
                fclose(f);
                return;
            }
        }
    }
    fclose(f);
}

int population_load(population_t *pop, const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        fprintf(stderr, "ERROR: could not open %s\n", filename);
        return -1;
    }
    size_t ret = fread(&pop->num_players, sizeof(pop->num_players), 1, f);
    if (ret != 1) {
        fprintf(stderr, "ERROR: could not read num_players\n");
        fclose(f);
        return -1;
    }
    ret = fread(&pop->num_inputs, sizeof(pop->num_inputs), 1, f);
    if (ret != 1) {
        fprintf(stderr, "ERROR: could not read num_inputs\n");
        fclose(f);
        return -1;
    }
    ret = fread(&pop->num_outputs, sizeof(pop->num_outputs), 1, f);
    if (ret != 1) {
        fprintf(stderr, "ERROR: could not read num_outputs\n");
        fclose(f);
        return -1;
    }
    ret = fread(&pop->num_chromo, sizeof(pop->num_chromo), 1, f);
    if (ret != 1) {
        fprintf(stderr, "ERROR: could not read num_chromo\n");
        fclose(f);
        return -1;
    }
    for (unsigned int i = 0; i < pop->num_players; ++i) {
        player_t *p = pop->players[i];
        unsigned int age = player_get_age(p);
        ret = fread(&age, sizeof(age), 1, f);
        if (ret != 1) {
            fprintf(stderr, "ERROR: could not read age\n");
            fclose(f);
            return -1;
        }
        player_get_chromosome(pop->players[i], pop->mom_weights,
                              pop->num_chromo);
        for (unsigned w = 0; w < pop->num_chromo; ++w) {
            ret =
                fread(&pop->mom_weights[w], sizeof(pop->mom_weights[0]), 1, f);
            if (ret != 1) {
                fprintf(stderr, "ERROR: could not read weight %u\n", w);
                fclose(f);
                return -1;
            }
        }
        player_set_chromosome(p, pop->mom_weights, pop->num_chromo);
    }
    fclose(f);
    return 0;
}
