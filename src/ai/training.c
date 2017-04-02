#include <doublefann.h>
#include <fann.h>
#include <stdio.h>
#include "ai.h"

typedef struct {
    struct fann_train_data *train_data;
} tr_t;

static void tr_get_input_norm(uint8_t *screen, double *inputs)
{
    for (unsigned int i = 0; i < NUM_INPUTS; ++i) {
        inputs[i] = (double)screen[i] / 255.0;
    }
}

static void tr_get_output(uint8_t keys, double *outputs)
{
    for (unsigned int i = 0; i < NUM_OUTPUTS; ++i) {
        outputs[i] = (double)((keys >> i) & 0x1);
    }
}

static int tr_load_file(FILE *f, unsigned int num_data, double *inputs,
                        double *outputs)
{
    uint8_t screen[NUM_INPUTS];
    uint8_t keys;
    for (unsigned int i = 0; i < num_data; ++i) {
        if (fread(screen, sizeof(screen), 1, f) != 1) {
            fprintf(stderr, "ERROR: could not load screen %u\n", i);
            return -1;
        }
        tr_get_input_norm(screen, inputs);
        inputs += NUM_INPUTS;
        if (fread(&keys, sizeof(keys), 1, f) != 1) {
            fprintf(stderr, "ERROR: could not load keys %u\n", i);
            return -1;
        }
        tr_get_output(keys, outputs);
        outputs += NUM_OUTPUTS;
    }
    return 0;
}

static tr_t *tr_create(const char *training_file)
{
    FILE *f = fopen(training_file, "r");
    if (f == NULL)
        return NULL;
    /* Get file size. */
    int ret = fseek(f, 0L, SEEK_END);
    if (ret != 0) {
        fclose(f);
        fprintf(stderr, "ERROR: fseek\n");
        return NULL;
    }
    long file_size = ftell(f);
    if (file_size < 0) {
        fclose(f);
        fprintf(stderr, "ERROR: ftell\n");
        return NULL;
    }
    rewind(f);
    tr_t *tr = malloc(sizeof(tr_t));
    /* Get number of data sets and validate. */
    unsigned int num_data;
    if (fread(&num_data, sizeof(num_data), 1, f) != 1) {
        fclose(f);
        fprintf(stderr, "ERROR: could not load num_data\n");
        return NULL;
    }
    if (num_data != (unsigned int)(file_size - 4) / sizeof(training_set_t)) {
        fclose(f);
        fprintf(stderr, "ERROR: %u != %ld / %lu)\n", num_data, file_size,
                sizeof(training_set_t));
        return NULL;
    }
    double *inputs = malloc(num_data * NUM_INPUTS * sizeof(double));
    double *outputs = malloc(num_data * NUM_OUTPUTS * sizeof(double));
    ret = tr_load_file(f, num_data, inputs, outputs);
    if (ret != 0) {
        fclose(f);
        free(inputs);
        free(outputs);
        return NULL;
    }
    fclose(f);
    tr->train_data = fann_create_train_array(num_data, NUM_INPUTS, inputs,
                                             NUM_OUTPUTS, outputs);
    free(inputs);
    free(outputs);
    printf("File %s loaded with %u training data\n", training_file, num_data);
    return tr;
}

static void tr_destroy(tr_t *tr)
{
    fann_destroy_train(tr->train_data);
    free(tr);
}

static void tr_main(tr_t *tr, unsigned int max_epochs)
{
    const unsigned int num_layers = 3;
    const unsigned int num_inputs = NUM_INPUTS;
    const unsigned int num_outputs = NUM_OUTPUTS;

    struct fann *ann =
        fann_create_standard(num_layers, num_inputs, 20, num_outputs);

    const float desired_error = (const float)0.001;
    const unsigned int epochs_between_reports = 100;
    fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
    fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);
    fann_set_training_algorithm(ann, FANN_TRAIN_RPROP);

    fann_print_parameters(ann);
    fann_train_on_data(ann, tr->train_data, max_epochs, epochs_between_reports,
                       desired_error);

    fann_save(ann, "test.net");
    fann_destroy(ann);
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Syntax: %s <training data file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    tr_t *tr = tr_create(argv[1]);
    if (tr == NULL) {
        fprintf(stderr, "ERROR: could not load training file %s\n", argv[1]);
    }
    tr_main(tr, 1000);
    tr_destroy(tr);
    return 0;
}
