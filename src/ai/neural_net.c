#include "neural_net.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

neural_net_t *neural_net_create(unsigned int num_inputs,
                                unsigned int num_outputs,
                                unsigned int num_hidden_layers,
                                unsigned int num_neurons_per_layer)
{
    if (num_inputs == 0) {
        fprintf(stderr, "ERROR: %s: num_inputs == 0\n", __func__);
        return NULL;
    }
    if (num_outputs == 0) {
        fprintf(stderr, "ERROR: %s: num_outputs == 0\n", __func__);
        return NULL;
    }
    if (num_hidden_layers == 0) {
        fprintf(stderr, "ERROR: %s: num_hidden_layers == 0\n", __func__);
        return NULL;
    }
    if (num_neurons_per_layer == 0) {
        fprintf(stderr, "ERROR: %s: num_neurons_per_layer == 0\n", __func__);
        return NULL;
    }
    neural_net_t *nn = (neural_net_t *)malloc(sizeof(neural_net_t));
    nn->num_inputs = num_inputs;
    nn->num_outputs = num_outputs;
    nn->num_hidden_layers = num_hidden_layers;
    nn->num_neurons_per_layer = num_neurons_per_layer;
    nn->num_layers = num_hidden_layers + 1u;
    nn->num_neurons = 0;
    nn->num_weights = 0;
    /* Allocate space for all layers of the network. */
    nn->layers =
        (neuron_layer_t **)malloc(sizeof(neuron_layer_t *) * nn->num_layers);
    /* Create the layers of the network. Create first the hidden layer. */
    nn->layers[0] = neuron_layer_create(num_neurons_per_layer, num_inputs);

    /* Create the hidden layers. */
    for (unsigned int i = 1; i < num_hidden_layers; ++i) {
        nn->layers[i] =
            neuron_layer_create(num_neurons_per_layer, num_neurons_per_layer);
    }

    /* Create the output layer. */
    nn->layers[nn->num_layers - 1] =
        neuron_layer_create(num_outputs, num_neurons_per_layer);

    /* Initialize num_neurons_. */
    for (unsigned int i = 0; i < nn->num_layers; ++i) {
        nn->num_neurons += neuron_layer_get_num_neurons(nn->layers[i]);
    }

    /* Initialize num_weights_. */
    for (unsigned int i = 0; i < nn->num_layers; ++i) {
        nn->num_weights += neuron_layer_get_num_weights(nn->layers[i]);
    }
    return nn;
}

void neural_net_destroy(neural_net_t *nn)
{
    for (unsigned int i = 0; i < nn->num_layers; ++i) {
        neuron_layer_destroy(nn->layers[i]);
    }
    free(nn->layers);
    free(nn);
}

/*
 * Returns the total number of neurons in the net.
 */
unsigned int neural_net_get_num_neurons(neural_net_t *nn)
{
    return nn->num_neurons;
}

/*
 * Returns the total number of weights in the net.
 */
unsigned int neural_net_get_num_weights(neural_net_t *nn)
{
    return nn->num_weights;
}

/*
 * Returns a vector containing the weights.
 */
void neural_net_get_weights(neural_net_t *nn, double *weights,
                            unsigned int length)
{
    assert(length == nn->num_weights);
    for (unsigned int i = 0; i < nn->num_layers; ++i) {
        weights = neuron_layer_get_weights(nn->layers[i], weights);
    }
}

/*
 * This function replaces the weights in the NN with the new values.
 */
void neural_net_set_weights(neural_net_t *nn, double *weights,
                            unsigned int length)
{
    assert(length == nn->num_weights);
    for (unsigned int i = 0; i < nn->num_layers; ++i) {
        weights = neuron_layer_set_weights(nn->layers[i], weights);
    }
}

/*
 * Given an input vector this function calculates the output vector.
 */
const double *neural_net_output(neural_net_t *nn, const double *inputs)
{
    const double *outputs = inputs;
    /* For each layer calculate the new output array. */
    for (unsigned int i = 0; i < nn->num_hidden_layers + 1; ++i) {
        outputs = neuron_layer_output(nn->layers[i], outputs);
    }
    /* The last array is the net output. */
    return outputs;
}

void neural_net_print(neural_net_t *nn)
{
    printf(
        "Neural Net: inputs=%u, outputs=%u, hidden_layers=%u, "
        "neurons_per_hidden_layer=%u\n",
        nn->num_inputs, nn->num_outputs, nn->num_hidden_layers,
        nn->num_neurons_per_layer);
    for (unsigned int i = 0; i < nn->num_hidden_layers + 1u; ++i) {
        neuron_layer_print(nn->layers[i]);
    }
}
