#ifndef __NEURON_LAYER_H__
#define __NEURON_LAYER_H__

#include "neuron.h"

typedef struct {
    /* Number of neurons. */
    unsigned int num_neurons;
    /* Number of input per neurons. */
    unsigned int inputs_per_neuron;
    /* Array of neurons. */
    neuron_t **neurons;
    /* Output array. */
    double *outputs;
} neuron_layer_t;

neuron_layer_t *neuron_layer_create(unsigned int num_neurons,
                                    unsigned int intputs_per_neuron);
void neuron_layer_destroy(neuron_layer_t *nl);
unsigned int neuron_layer_get_num_neurons(neuron_layer_t *nl);
unsigned int neuron_layer_get_num_weights(neuron_layer_t *nl);
void neuron_layer_get_weights(neuron_layer_t *nl, double *weights);
void neuron_layer_set_weights(neuron_layer_t *nl, double *weights);
const double *neuron_layer_output(neuron_layer_t *nl, const double *inputs);
void neuron_layer_print(neuron_layer_t *nl);

#endif /* __NEURON_LAYER_H__ */
