#include <gtest/gtest.h>
extern "C" {
#include "ai/neural_net.h"
}

TEST(neural_net_test, basic)
{
    const unsigned int num_inputs = 5;
    const unsigned int num_outputs = 3;
    const unsigned int num_hidden_layers = 3;
    const unsigned int num_neurons_per_layer = 5;
    unsigned int num_weights =
        (num_neurons_per_layer * (num_inputs + 1)) +
        ((num_neurons_per_layer + 1) * num_neurons_per_layer *
         (num_hidden_layers - 1)) +
        ((num_neurons_per_layer + 1) * num_outputs);
    neural_net_t *nn = nullptr;
    double inputs[num_inputs];
    double weights1[num_weights];
    double weights2[num_weights];

    /* Test neural net creation. */
    EXPECT_EQ(nullptr, neural_net_create(0, num_outputs, num_hidden_layers,
                                         num_neurons_per_layer));
    EXPECT_EQ(nullptr, neural_net_create(num_inputs, 0, num_hidden_layers,
                                         num_neurons_per_layer));
    EXPECT_EQ(nullptr, neural_net_create(num_inputs, num_outputs, 0,
                                         num_neurons_per_layer));
    EXPECT_EQ(nullptr,
              neural_net_create(num_inputs, num_outputs, num_hidden_layers, 0));
    EXPECT_NE(nullptr,
              nn = neural_net_create(num_inputs, num_outputs, num_hidden_layers,
                                     num_neurons_per_layer));

    /* Test neural_net_get_num_neurons. */
    unsigned int num_neurons =
        num_neurons_per_layer * num_hidden_layers + num_outputs;
    EXPECT_EQ(num_neurons, neural_net_get_num_neurons(nn));

    /* Test neural_net_get_num_weights. */
    EXPECT_EQ(num_weights, neural_net_get_num_weights(nn));

    /* Test neural_net_set_weights. */
    for (unsigned int i = 0; i < num_weights; ++i) {
        weights1[i] = 1.0;
    }
    neural_net_set_weights(nn, weights1, num_weights);

    /* Test neural_net_get_weights. */
    neural_net_get_weights(nn, weights2, num_weights);
    for (unsigned int i = 0; i < num_weights; ++i) {
        EXPECT_EQ(weights1[i], weights2[i]);
    }

    /* Test CalcOutput. */
    for (unsigned i = 0; i < num_inputs; ++i) {
        inputs[i] = 1.0;
    }
    neural_net_output(nn, inputs);

    /* Test Print. */
    EXPECT_NO_THROW(neural_net_print(nn));

    /* Test destruction. */
    neural_net_destroy(nn);
}
