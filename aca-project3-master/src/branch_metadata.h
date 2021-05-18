//
// This file defines the branch_metadata struct.
//

#ifndef BRANCH_METADATA_H
#define BRANCH_METADATA_H

#include <stdint.h>
#include <string.h>

enum branch_direction { NOT_TAKEN, TAKEN };

struct branch_metadata {
    uint32_t address;
    uint32_t target;
};

#endif
