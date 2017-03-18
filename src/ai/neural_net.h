#ifndef __NEURAL_NET_H__
#define __NEURAL_NET_H__

#include "neuron_layer.h"

typedef struct {
    unsigned int num_inputs;
    unsigned int num_outputs;
    unsigned int num_hidden_layers;
    unsigned int num_neurons_per_layer;
    unsigned int num_layers;
    unsigned int num_neurons;
    unsigned int num_weights;
    neuron_layer_t **layers;
} neural_net_t;

/* Neural Net constructor. */
neural_net_t *neural_net_create(unsigned int num_inputs,
                                unsigned int num_outputs,
                                unsigned int num_hidden_layers,
                                unsigned int num_neurons_per_layer);

/* Neural Net destructor. */
void neural_net_destroy(neural_net_t *nn);

/* Get total number of neurons in the net. */
unsigned int neural_net_get_num_neurons(neural_net_t *nn);

/* Get total number of weights in the net. */
unsigned int neural_net_get_num_weights(neural_net_t *nn);

/* Gets the weights from the neural network. */
void neural_net_get_weights(neural_net_t *nn, double *weights,
                            unsigned int length);

/* Replaces the weights with new ones. */
void neural_net_set_weights(neural_net_t *nn, double *weights,
                            unsigned int length);

/* Calculates the outputs from a set of inputs. */
const double *neural_net_output(neural_net_t *nn, const double *inputs);

/* Prints the network. */
void neural_net_print(neural_net_t *nn);

#endif /* __NEURAL_NET_H__ */
