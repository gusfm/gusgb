#include "neuron_layer.h"
#include <stdio.h>
#include <stdlib.h>

neuron_layer_t *neuron_layer_create(unsigned int num_neurons,
                                    unsigned int intputs_per_neuron)
{
    if (num_neurons == 0) {
        return NULL;
    }
    neuron_layer_t *nl = (neuron_layer_t *)malloc(sizeof(neuron_layer_t));
    nl->num_neurons = num_neurons;
    nl->inputs_per_neuron = intputs_per_neuron;
    nl->neurons = (neuron_t **)malloc(sizeof(neuron_t) * num_neurons);
    for (unsigned int i = 0; i < num_neurons; ++i) {
        nl->neurons[i] = neuron_create(intputs_per_neuron);
    }
    nl->outputs = (double *)malloc(sizeof(double) * num_neurons);
    return nl;
}

void neuron_layer_destroy(neuron_layer_t *nl)
{
    for (unsigned int i = 0; i < nl->num_neurons; ++i) {
        neuron_destroy(nl->neurons[i]);
    }
    free(nl->outputs);
    free(nl);
}

unsigned int neuron_layer_get_num_neurons(neuron_layer_t *nl)
{
    return nl->num_neurons;
}

unsigned int neuron_layer_get_num_weights(neuron_layer_t *nl)
{
    return nl->num_neurons * (nl->inputs_per_neuron + 1);
}

void neuron_layer_get_weights(neuron_layer_t *nl, double *weights)
{
    for (unsigned int i = 0; i < nl->num_neurons; ++i) {
        neuron_get_weights(nl->neurons[i], weights);
        weights += nl->inputs_per_neuron + 1;
    }
}

void neuron_layer_set_weights(neuron_layer_t *nl, double *weights)
{
    for (unsigned int i = 0; i < nl->num_neurons; ++i) {
        neuron_set_weights(nl->neurons[i], weights);
        weights += nl->inputs_per_neuron + 1;
    }
}

const double *neuron_layer_output(neuron_layer_t *nl, const double *inputs)
{
    /* For each neuron. */
    for (unsigned int i = 0; i < nl->num_neurons; ++i) {
        /* Calculate output. */
        nl->outputs[i] = neuron_output(nl->neurons[i], inputs);
    }
    return nl->outputs;
}

void neuron_layer_print(neuron_layer_t *nl)
{
    printf("  Neuron Layer: num_neurons=%u, inputs_per_neuron=%u\n",
           nl->num_neurons, nl->inputs_per_neuron);
    for (unsigned int i = 0; i < nl->num_neurons; ++i) {
        neuron_print(nl->neurons[i]);
    }
}
