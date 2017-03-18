#ifndef __NEURON_H__
#define __NEURON_H__

typedef struct {
    /* Number of inputs. */
    unsigned int num_inputs;
    /* Weights for each input. */
    double *weight_array;
} neuron_t;

neuron_t *neuron_create(unsigned int num_inputs);
void neuron_destroy(neuron_t *n);
void neuron_get_weights(neuron_t *n, double *weights);
void neuron_set_weights(neuron_t *n, const double *weights);
double neuron_output(neuron_t *n, const double *inputs);
void neuron_print(neuron_t *n);

#endif /* __NEURON_H__ */
