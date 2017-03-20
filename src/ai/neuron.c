#include "neuron.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "random.h"

#define BIAS (-1.0)
#define SHAPE_OF_CURVE (1.0)

neuron_t *neuron_create(unsigned int num_inputs)
{
    neuron_t *n = (neuron_t *)malloc(sizeof(neuron_t));
    /* We need an additional weight for the bias. */
    n->num_inputs = num_inputs + 1;
    n->weight_array = malloc(sizeof(double) * n->num_inputs);
    for (unsigned int i = 0; i < n->num_inputs; ++i) {
        /* Set up the weights with an initial random value. */
        n->weight_array[i] = rand_clamp();
    }
    return n;
}

void neuron_destroy(neuron_t *n)
{
    free(n->weight_array);
    free(n);
}

void neuron_get_weights(neuron_t *n, double *weights)
{
    for (unsigned int i = 0; i < n->num_inputs; ++i) {
        weights[i] = n->weight_array[i];
    }
}

void neuron_set_weights(neuron_t *n, const double *weights)
{
    for (unsigned int i = 0; i < n->num_inputs; ++i) {
        n->weight_array[i] = weights[i];
    }
}

static double sigmoid(double netinput)
{
    return (1.0 / (1.0 + exp(-netinput / SHAPE_OF_CURVE)));
}

double neuron_output(neuron_t *n, const double *inputs)
{
    double activation = 0;
    /* For every weight. */
    for (unsigned int i = 0; i < n->num_inputs - 1; ++i) {
        /* Sum the weights x inputs. */
        activation += n->weight_array[i] * inputs[i];
    }
    /* Add in the bias. */
    activation += n->weight_array[n->num_inputs - 1] * BIAS;
    return sigmoid(activation);
}

void neuron_print(neuron_t *n)
{
    printf("    Neuron: w={");
    for (unsigned int i = 0; i < n->num_inputs - 1; ++i) {
        printf("%.2lf, ", n->weight_array[i]);
    }
    printf("\b\b}, t={%.2lf}\n", n->weight_array[n->num_inputs - 1]);
}
