//
// This is the branchsim main file. It is the entrypoint of the simulator.
//
// This file handles all of the argument and input parsing as well as the
// output printing. It initializes the branch predictor with the list of
// branches from the trace and then calls the active branch predictor for each
// of the branch instructions.
//

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "branch_metadata.h"
#include "branch_predictors.h"

int main(int argc, char **argv)
{
    // Parse the arguments.
    if (argc != 2) {
        fprintf(stderr, "Incorrect number of arguments.\n");
        return 1;
    }
    char *branch_predictor_str = argv[1];

    // Print out some parameter info
    printf("Parameter Info\n");
    printf("==============\n");
    printf("Branch Predictor: %s\n", branch_predictor_str);

    // Read in the branch metadata
    uint32_t num_branches;
    struct branch_metadata *branch_metadatas;
    {
        if (scanf("%u", &num_branches) == 0) return 2;

        branch_metadatas = calloc(num_branches, sizeof(struct branch_metadata));

        uint32_t address, target;
        printf("\n\nBranch Metadata\n");
        printf("===============\n");
        for (int i = 0; i < num_branches; i++) {
            if (scanf("%x %x", &address, &target) == 0) return 3;
            printf("Branch at 0x%x targets 0x%x\n", address, target);
            branch_metadatas[i].address = address;
            branch_metadatas[i].target = target;
        }
    }

    // Instantiate the branch predictor
    struct branch_predictor *branch_predictor;
    if (!strcmp("ANT", branch_predictor_str)) {
        branch_predictor = ant_branch_predictor_new(num_branches, branch_metadatas);
    } else if (!strcmp("AT", branch_predictor_str)) {
        branch_predictor = at_branch_predictor_new(num_branches, branch_metadatas);
    } else if (!strcmp("BTFNT", branch_predictor_str)) {
        branch_predictor = btfnt_branch_predictor_new(num_branches, branch_metadatas);
    } else if (!strcmp("LTG", branch_predictor_str)) {
        branch_predictor = ltg_branch_predictor_new(num_branches, branch_metadatas);
    } else if (!strcmp("LTL", branch_predictor_str)) {
        branch_predictor = ltl_branch_predictor_new(num_branches, branch_metadatas);
    } else if (!strcmp("2BG", branch_predictor_str)) {
        branch_predictor = tbg_branch_predictor_new(num_branches, branch_metadatas);
    } else if (!strcmp("2BL", branch_predictor_str)) {
        branch_predictor = tbl_branch_predictor_new(num_branches, branch_metadatas);
    } else {
        fprintf(stderr, "Unknown branch predictor %s", branch_predictor_str);
        return 1;
    }

    // Read the input and call the cache system mem_access function.
    uint32_t prediction_count = 0;
    uint32_t correct_prediction_count = 0;
    uint32_t address = 0;
    char branch_result[32];
    while (scanf("%x %s\n", &address, branch_result) >= 0) {
        prediction_count++;

        printf("Branch at 0x%x\n", address);
        enum branch_direction prediction = branch_predictor->predict(branch_predictor, address);
        printf("  Predicted: %s\n", (prediction == TAKEN ? "TAKEN" : "NOT_TAKEN"));

        bool actual_result = !strcmp(branch_result, "TAKEN");
        printf("  Actual:    %s\n", (actual_result ? "TAKEN" : "NOT_TAKEN"));

        if ((actual_result && prediction == TAKEN) || (!actual_result && prediction == NOT_TAKEN))
            correct_prediction_count++;

        branch_predictor->handle_result(branch_predictor, address, actual_result);
    }

    // Print the statistics
    printf("\n\nStatistics\n");
    printf("==========\n");
    printf("OUTPUT PREDICTIONS %d\n", prediction_count);
    printf("OUTPUT CORRECT %d\n", correct_prediction_count);
    printf("OUTPUT INCORRECT %d\n", prediction_count - correct_prediction_count);
    printf("OUTPUT BRANCH PREDICTION RATE %.8f\n",
           (double)correct_prediction_count / prediction_count);

    // Clean everything up.
    branch_predictor->cleanup(branch_predictor);
    free(branch_predictor);

    return 0;
}
