#include <gtest/gtest.h>
extern "C" {
#include "ai/neuron.h"
}

TEST(neuron_test, basic)
{
    neuron_t *neuron = nullptr;
    const unsigned int num_inputs = 3;
    double inputs[num_inputs + 1];
    double weights[num_inputs + 1];

    /* Test initialization. */
    EXPECT_NE(nullptr, neuron = neuron_create(num_inputs));

    /* Test neuron_set_weights. */
    for (unsigned int i = 0; i < num_inputs + 1; ++i) {
        weights[i] = i;
    }
    neuron_set_weights(neuron, weights);

    /* Test neuron_get_weights. */
    for (unsigned int i = 0; i < num_inputs + 1; ++i) {
        weights[i] = 0;
    }
    neuron_get_weights(neuron, weights);
    for (unsigned int i = 0; i < num_inputs + 1; ++i) {
        EXPECT_EQ(i, weights[i]);
    }

    /* Test neuron_output. */
    for (unsigned i = 0; i < num_inputs; ++i) {
        inputs[i] = 1.0;
    }
    EXPECT_EQ(0.5, neuron_output(neuron, inputs));

    neuron_print(neuron);
    neuron_destroy(neuron);
}
