#include <gtest/gtest.h>
extern "C" {
#include "ai/neuron_layer.h"
}

TEST(neuron_layer_test, basic)
{
    const unsigned int num_neurons = 5;
    const unsigned int inputs_per_neuron = 3;
    const unsigned int num_weights = num_neurons * (inputs_per_neuron + 1);
    neuron_layer_t *nl = nullptr;
    double inputs[num_neurons];
    double weights[num_weights];
    double weights_tmp[num_weights];
    /* Test initialization. */
    EXPECT_EQ(nullptr, neuron_layer_create(0, 0));
    EXPECT_NE(nullptr,
              nl = neuron_layer_create(num_neurons, inputs_per_neuron));

    /* Test neuron_layer_get_num_neurons. */
    EXPECT_EQ(num_neurons, neuron_layer_get_num_neurons(nl));

    /* Test neuron_layer_get_num_weights. */
    EXPECT_EQ(num_neurons * (inputs_per_neuron + 1),
              neuron_layer_get_num_weights(nl));

    /* Test neuron_layer_set_weights. */
    for (unsigned int i = 0; i < num_weights; ++i) {
        /* Set threshold and weights to make output equal to 0.5. */
        if (i % (inputs_per_neuron + 1) == 3) {
            weights[i] = 3.0;
        } else {
            weights[i] = 1.0;
        }
    }
    neuron_layer_set_weights(nl, weights);

    /* Test neuron_layer_get_weights. */
    neuron_layer_get_weights(nl, weights_tmp);
    for (unsigned int i = 0; i < num_weights; ++i) {
        EXPECT_EQ(weights[i], weights_tmp[i]);
    }

    /* Test neuron_layer_output. */
    const double *out;
    for (unsigned i = 0; i < num_neurons; ++i) {
        inputs[i] = 1.0;
    }
    EXPECT_NE(nullptr, out = neuron_layer_output(nl, inputs));
    for (unsigned int i = 0; i < num_neurons; ++i) {
        EXPECT_EQ(0.5, out[i]);
    }

    /* Test Print. */
    neuron_layer_print(nl);

    /* Test destruction. */
    neuron_layer_destroy(nl);
}
