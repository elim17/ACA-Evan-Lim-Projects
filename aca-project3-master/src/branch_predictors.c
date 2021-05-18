//
// This file contains all of the implementations of the branch predictor
// constructors from the branch_predictors.h file.
//
// It also contains stubs of all of the functions that are added to each
// branch_predictor struct at construction time.
//
// Feel free to create any additional structs or global variables that you
// want.
//
// ============================================================================
// NOTE: It is recommended that you read the comments in the
// branch_predictors.h file for further context on what each function is for.
// ============================================================================
//

#include "branch_predictors.h"

// ANT Branch Predictor
//Always Not Taken Branch Predictor (ANT)
// ============================================================================

enum branch_direction ant_branch_predictor_predict(struct branch_predictor *branch_predictor,
                                                   uint32_t address)
{
    // TODO: return this branch predictors prediction for the branch at the
    // given address.
    return NOT_TAKEN;
}

void ant_branch_predictor_handle_result(struct branch_predictor *branch_predictor, uint32_t address,
                                        enum branch_direction branch_direction)
{
    // TODO: use this function to update the state of the branch predictor
    // given the most recent branch direction.
}

void ant_branch_predictor_cleanup(struct branch_predictor *branch_predictor)
{
    // TODO cleanup if necessary
}

struct branch_predictor *ant_branch_predictor_new(uint32_t num_branches,
                                                  struct branch_metadata *branch_metadatas)
{
    struct branch_predictor *ant_bp = calloc(1, sizeof(struct branch_predictor));
    ant_bp->cleanup = &ant_branch_predictor_cleanup;
    ant_bp->predict = &ant_branch_predictor_predict;
    ant_bp->handle_result = &ant_branch_predictor_handle_result;

    // TODO allocate storage for any data necessary for this branch predictor

    return ant_bp;
}


// AT Branch Predictor
//Always Taken Predictor (AT)
// ============================================================================

enum branch_direction at_branch_predictor_predict(struct branch_predictor *branch_predictor,
                                                  uint32_t address)
{
    // TODO: return this branch predictors prediction for the branch at the
    // given address.
    return TAKEN;
}

void at_branch_predictor_handle_result(struct branch_predictor *branch_predictor, uint32_t address,
                                       enum branch_direction branch_direction)
{
    // TODO: use this function to update the state of the branch predictor
    // given the most recent branch direction.

}

void at_branch_predictor_cleanup(struct branch_predictor *branch_predictor)
{
    // TODO cleanup if necessary
}

struct branch_predictor *at_branch_predictor_new(uint32_t num_branches,
                                                 struct branch_metadata *branch_metadatas)
{
    struct branch_predictor *at_bp = calloc(1, sizeof(struct branch_predictor));
    at_bp->cleanup = &at_branch_predictor_cleanup;
    at_bp->predict = &at_branch_predictor_predict;
    at_bp->handle_result = &at_branch_predictor_handle_result;

    // TODO allocate storage for any data necessary for this branch predictor

    return at_bp;
}

// BTFNT Branch Predictor    Backwards taken forward not taken
// ============================================================================

enum branch_direction btfnt_branch_predictor_predict(struct branch_predictor *branch_predictor,
                                                     uint32_t address)
{
    // TODO: return this branch predictors prediction for the branch at the
    // given address.
    //cast data pointer
    uint32_t* targetArray = ((uint32_t*)branch_predictor->targets);
    uint32_t* addressTraceArray = ((uint32_t*)branch_predictor->addresses);

    //loop through to address trace to find the mathing address so we can index into the correct target.
    for(int i = 0; i < branch_predictor->numberOfBranches; i++)
    {
        if(address == addressTraceArray[i]) // when we find the correct address index into target and check if target is behind or infront of our current address
        {
            if(targetArray[i]< address) // if the target is less than the current address we know we are going backwards
            {
                //if backwards, take the branch
                return TAKEN;
            }
            break;
        }
    }
    //otherwise don't take the branch

    return NOT_TAKEN;
}

void btfnt_branch_predictor_handle_result(struct branch_predictor *branch_predictor,
                                          uint32_t address, enum branch_direction branch_direction)
{
    // TODO: use this function to update the state of the branch predictor
    // given the most recent branch direction.
}

void btfnt_branch_predictor_cleanup(struct branch_predictor *branch_predictor)
{
    // TODO cleanup if necessary
    free(branch_predictor->addresses);
    free(branch_predictor->targets);
}

struct branch_predictor *btfnt_branch_predictor_new(uint32_t num_branches,
                                                    struct branch_metadata *branch_metadatas)
{
    struct branch_predictor *btfnt_bp = calloc(1, sizeof(struct branch_predictor));
    btfnt_bp->cleanup = &btfnt_branch_predictor_cleanup;
    btfnt_bp->predict = &btfnt_branch_predictor_predict;
    btfnt_bp->handle_result = &btfnt_branch_predictor_handle_result;

    // TODO allocate storage for any data necessary for this branch predictor
    //Need the target
    btfnt_bp->addresses = (uint32_t*)calloc(num_branches,sizeof(uint32_t));
    btfnt_bp->targets = (uint32_t*)calloc(num_branches,sizeof(uint32_t));
    btfnt_bp->numberOfBranches = num_branches; // set the number of branches variables
    //cast void pointer so we can set values

    uint32_t* traceAddressArray = ((uint32_t*)btfnt_bp->addresses); // cast the void pointer
    uint32_t* targetAddressArray = ((uint32_t*)btfnt_bp->targets); // cast the void pointer
    for(int i = 0; i< num_branches; i++)
    {
        traceAddressArray[i] = branch_metadatas[i].address;
        targetAddressArray[i] = branch_metadatas[i].target;
    }

    return btfnt_bp;
}

// LTG Branch Predictor
// ============================================================================

enum branch_direction ltg_branch_predictor_predict(struct branch_predictor *branch_predictor,
                                                   uint32_t address)
{
    // TODO: return this branch predictors prediction for the branch at the
    // given address.
    //cast void data
    uint32_t* phtPredictionArray = ((uint32_t*)branch_predictor->phtPredictions);

    uint32_t prediction = 0;


    prediction = phtPredictionArray[branch_predictor->GHR.globalHistoryRegister];


    if(prediction == 1)
    {
        return TAKEN;
    } else{
        return NOT_TAKEN;
    }

}

void ltg_branch_predictor_handle_result(struct branch_predictor *branch_predictor, uint32_t address,
                                        enum branch_direction branch_direction)
{
    //cast void data
    uint32_t* phtPredictionArray = ((uint32_t*)branch_predictor->phtPredictions);

    // TODO: use this function to update the state of the branch predictor
    // given the most recent branch direction.
    uint32_t prediction = 0;
    if(branch_direction == TAKEN) {
        prediction = 1;
    }


    //update the pht and GHR

    //update the PHT
    phtPredictionArray[branch_predictor->GHR.globalHistoryRegister] = prediction; // 1 is taken 0 is not taken


    //update GHR
    // shift left by 1
    branch_predictor->GHR.globalHistoryRegister = branch_predictor->GHR.globalHistoryRegister << 1;
    if(branch_direction == TAKEN)
    {
        //or a 1 to the end
        branch_predictor->GHR.globalHistoryRegister = branch_predictor->GHR.globalHistoryRegister | 1;

    }
    else{
        // no need to do anything as the shift left already added a 0.
    }

    //print_n_lsb_as_binary(branch_predictor->GHR.globalHistoryRegister, 10);
    //printf(" and prediction = %d ", phtPredictions[branch_predictor->GHR.globalHistoryRegister]);

}

void ltg_branch_predictor_cleanup(struct branch_predictor *branch_predictor)
{
    // TODO cleanup if necessary
    free(branch_predictor->addresses);
    free(branch_predictor->targets);
    free((branch_predictor->phtPredictions));

}

struct branch_predictor *ltg_branch_predictor_new(uint32_t num_branches,
                                                  struct branch_metadata *branch_metadatas)
{
    struct branch_predictor *ltg_bp = calloc(1, sizeof(struct branch_predictor));
    ltg_bp->cleanup = &ltg_branch_predictor_cleanup;
    ltg_bp->predict = &ltg_branch_predictor_predict;
    ltg_bp->handle_result = &ltg_branch_predictor_handle_result;

    // TODO allocate storage for any data necessary for this branch predictor

    ltg_bp->phtPredictions = (uint32_t*)calloc(32,sizeof(uint32_t)); // tracking the one bit history table
    ltg_bp->phtIndex = 0; //set the index for the pht
    ltg_bp->GHR.globalHistoryRegister = 0; // initialize global history register
    ltg_bp->numberOfBranches = num_branches; // set the number of branches variables
    //cast void pointer so we can set values

    // initiallize pht

    uint32_t* phtPreditionArray = ((uint32_t*)ltg_bp->phtPredictions); // cast the void pointer
    for(int i = 0; i< 32; i++)
    {
        phtPreditionArray[i] = 0;
    }

    return ltg_bp;
}

// LTL Branch Predictor   Last Time Local
// ============================================================================

enum branch_direction ltl_branch_predictor_predict(struct branch_predictor *branch_predictor,
                                                   uint32_t address)
{
    // TODO: return this branch predictors prediction for the branch at the
    // given address.

    //cast void data
    uint32_t* localHistoryRegisterArray = ((uint32_t*)branch_predictor->localHistoryRegisterArray);
    uint32_t* phtStorage = ((uint32_t*)branch_predictor->patternHistoryTableStorage);


    // get index of LHR and use it to index into correct PHT using 4 least significant bits of address
    uint32_t  lhrIndex = address << 28;
    lhrIndex = lhrIndex >> 28; // isolate right 4 bits

    //get current LHR
    branch_predictor->LHR.currentLocalHistoryRegister = localHistoryRegisterArray[lhrIndex];

    uint32_t prediction = 0;
    prediction = phtStorage[branch_predictor->LHR.currentLocalHistoryRegister + (lhrIndex * 16)];


    if(prediction == 1)
    {
        return TAKEN;
    } else{
        return NOT_TAKEN;
    }
}

void ltl_branch_predictor_handle_result(struct branch_predictor *branch_predictor, uint32_t address,
                                        enum branch_direction branch_direction)
{
    // TODO: use this function to update the state of the branch predictor
    // given the most recent branch direction.

    //cast void data
    //cast void data
    uint32_t* localHistoryRegisterArray = ((uint32_t*)branch_predictor->localHistoryRegisterArray);
    uint32_t* phtStorage = ((uint32_t*)branch_predictor->patternHistoryTableStorage);

    //calculate prediction
    uint32_t prediction = 0;
    if(branch_direction == TAKEN) {
        prediction = 1;
    }

    // get index of LHR and use it to index into correct PHT using 4 least significant bits of address
    uint32_t  lhrIndex = address << 28;
    lhrIndex = lhrIndex >> 28; // isolate right 4 bits

    //get current LHR
    branch_predictor->LHR.currentLocalHistoryRegister = localHistoryRegisterArray[lhrIndex];

    //update the pht and LHR---------------

    //update the PHT
    // the pht index will be the lhr + (lhrIndex * 16)
    phtStorage[branch_predictor->LHR.currentLocalHistoryRegister + (lhrIndex * 16)] = prediction; // 1 is taken 0 is not taken


    //update LHR
    // shift left by 1
    branch_predictor->LHR.currentLocalHistoryRegister = branch_predictor->LHR.currentLocalHistoryRegister << 1;
    if(branch_direction == TAKEN)
    {
        //or a 1 to the end
        branch_predictor->LHR.currentLocalHistoryRegister = branch_predictor->LHR.currentLocalHistoryRegister | 1;

    }
    else{
        // no need to do anything as the shift left already added a 0.
    }

    // need to push the change to the array
    localHistoryRegisterArray[lhrIndex] = branch_predictor->LHR.currentLocalHistoryRegister;



}

void ltl_branch_predictor_cleanup(struct branch_predictor *branch_predictor)
{
    // TODO cleanup if necessary
    free(branch_predictor->localHistoryRegisterArray);
    free(branch_predictor->patternHistoryTableStorage);
}


struct branch_predictor *ltl_branch_predictor_new(uint32_t num_branches,
                                                  struct branch_metadata *branch_metadatas)
{
    struct branch_predictor *ltl_bp = calloc(1, sizeof(struct branch_predictor));
    ltl_bp->cleanup = &ltl_branch_predictor_cleanup;
    ltl_bp->predict = &ltl_branch_predictor_predict;
    ltl_bp->handle_result = &ltl_branch_predictor_handle_result;

    // TODO allocate storage for any data necessary for this branch predictor

    ltl_bp->localHistoryRegisterArray = (uint32_t *)calloc(16,sizeof(uint32_t)); // 4 bits of
    ltl_bp->patternHistoryTableStorage=(uint32_t *)calloc(16*16,sizeof(uint32_t)); // 4 bits of history for each register means 16 entries per register

    // initiallize pht

//    uint32_t* lhrArray = ((uint32_t*)ltl_bp->localHistoryRegisterArray); // cast the void pointer
//    uint32_t* phtStorage = ((uint32_t*)ltl_bp->localHistoryRegisterArray); // cast the void pointer
//
//    for(int i = 0; i< 16; i++)
//    {
//        lhrArray[i] = 0;
//    }
//    for(int i = 0; i<(16*16); i++)
//    {
//        phtStorage[i] = 0;
//    }


    return ltl_bp;
}

// 2BG Branch Predictor
// ============================================================================

enum branch_direction tbg_branch_predictor_predict(struct branch_predictor *branch_predictor,
                                                   uint32_t address)
{
    // TODO: return this branch predictors prediction for the branch at the
    // given address.
    //cast void data
    uint32_t* phtPredictionArray = ((uint32_t*)branch_predictor->phtPredictions);

    uint32_t prediction = 0;


    prediction = phtPredictionArray[branch_predictor->GHR.globalHistoryRegister];


    if(prediction == 2 || prediction == 3)// weakly taken or strongly taken
    {
        return TAKEN;
    } else{
        return NOT_TAKEN;
    }
}

void tbg_branch_predictor_handle_result(struct branch_predictor *branch_predictor, uint32_t address,
                                        enum branch_direction branch_direction)
{
    // TODO: use this function to update the state of the branch predictor
    // given the most recent branch direction.
    //cast void data
    uint32_t* phtPredictionArray = ((uint32_t*)branch_predictor->phtPredictions);

    uint32_t prediction = 0;
    if(branch_direction == TAKEN) {
        prediction = 1;
    }


    //update the pht and GHR

    //update the PHT state
    uint32_t currentState = phtPredictionArray[branch_predictor->GHR.globalHistoryRegister];
    //implement state change 0 = strongly not taken, 1= weakly not taken, 2 = weakly taken, 3 = strongly taken
    if(currentState == 0){
        if(prediction == 1){ // move to 1 weakly not taken
            phtPredictionArray[branch_predictor->GHR.globalHistoryRegister]= 1; //0 is not taken, 1 is taken
        }
        else{ // stay strongly not taken
            phtPredictionArray[branch_predictor->GHR.globalHistoryRegister] = 0;
        }
    }
    else if(currentState == 1){
        if(prediction == 1){ // move to weakly taken
            phtPredictionArray[branch_predictor->GHR.globalHistoryRegister]= 2; //0 is not taken, 1 is taken
        }
        else{ // go to strongly not taken
            phtPredictionArray[branch_predictor->GHR.globalHistoryRegister] = 0;
        }
    }
    else if(currentState == 2){
        if(prediction == 1){ // move to strongly taken
            phtPredictionArray[branch_predictor->GHR.globalHistoryRegister] = 3; //0 is not taken, 1 is taken
        }
        else{ // go to weakly not taken
            phtPredictionArray[branch_predictor->GHR.globalHistoryRegister] = 1;
        }
    }
    else if(currentState == 3){
        if(prediction == 1){ // stay strongly taken
            phtPredictionArray[branch_predictor->GHR.globalHistoryRegister] = 3; //0 is not taken, 1 is taken
        }
        else{ // go to weakly taken
            phtPredictionArray[branch_predictor->GHR.globalHistoryRegister] = 2;
        }
    }


    //update GHR
    // shift left by 1
    branch_predictor->GHR.globalHistoryRegister = branch_predictor->GHR.globalHistoryRegister << 1;
    if(branch_direction == TAKEN)
    {
        //or a 1 to the end
        branch_predictor->GHR.globalHistoryRegister = branch_predictor->GHR.globalHistoryRegister | 1;

    }
    else{
        // no need to do anything as the shift left already added a 0.
    }

    //print_n_lsb_as_binary(branch_predictor->GHR.globalHistoryRegister, 10);
    //printf(" and prediction = %d ", phtPredictions[branch_predictor->GHR.globalHistoryRegister]);
}

void tbg_branch_predictor_cleanup(struct branch_predictor *branch_predictor)
{
    // TODO cleanup if necessary
    free((branch_predictor->phtPredictions));
}

struct branch_predictor *tbg_branch_predictor_new(uint32_t num_branches,
                                                  struct branch_metadata *branch_metadatas)
{
    struct branch_predictor *tbg_bp = calloc(1, sizeof(struct branch_predictor));
    tbg_bp->cleanup = &tbg_branch_predictor_cleanup;
    tbg_bp->predict = &tbg_branch_predictor_predict;
    tbg_bp->handle_result = &tbg_branch_predictor_handle_result;

    // TODO allocate storage for any data necessary for this branch predictor
    tbg_bp->phtPredictions = (uint32_t*)calloc(32,sizeof(uint32_t)); // tracking the one bit history table
    tbg_bp->phtIndex = 0; //set the index for the pht
    tbg_bp->GHR.globalHistoryRegister = 0; // initialize global history register
    tbg_bp->numberOfBranches = num_branches; // set the number of branches variables
    //cast void pointer so we can set values

    // initiallize pht

    uint32_t* phtPreditionArray = ((uint32_t*)tbg_bp->phtPredictions); // cast the void pointer
    for(int i = 0; i< 32; i++)
    {
        phtPreditionArray[i] = 0;
    }
    return tbg_bp;
}

// 2BL Branch Predictor
// ============================================================================

enum branch_direction tbl_branch_predictor_predict(struct branch_predictor *branch_predictor,
                                                   uint32_t address)
{
    // TODO: return this branch predictors prediction for the branch at the
    // given address.
    //cast void data
    uint32_t* localHistoryRegisterArray = ((uint32_t*)branch_predictor->localHistoryRegisterArray);
    uint32_t* phtStorage = ((uint32_t*)branch_predictor->patternHistoryTableStorage);


    // get index of LHR and use it to index into correct PHT using 4 least significant bits of address
    uint32_t  lhrIndex = address << 28;
    lhrIndex = lhrIndex >> 28; // isolate right 4 bits

    //get current LHR
    branch_predictor->LHR.currentLocalHistoryRegister = localHistoryRegisterArray[lhrIndex];

    uint32_t prediction = 0;
    prediction = phtStorage[branch_predictor->LHR.currentLocalHistoryRegister + (lhrIndex * 16)];


    if(prediction == 2 || prediction == 3)// weakly taken or strongly taken
    {
        return TAKEN;
    } else{
        return NOT_TAKEN;
    }
}

void tbl_branch_predictor_handle_result(struct branch_predictor *branch_predictor, uint32_t address,
                                        enum branch_direction branch_direction)
{
    // TODO: use this function to update the state of the branch predictor
    // given the most recent branch direction.
    //cast void data
    //cast void data
    uint32_t* localHistoryRegisterArray = ((uint32_t*)branch_predictor->localHistoryRegisterArray);
    uint32_t* phtStorage = ((uint32_t*)branch_predictor->patternHistoryTableStorage);

    //calculate prediction
    uint32_t prediction = 0;
    if(branch_direction == TAKEN) {
        prediction = 1;
    }

    // get index of LHR and use it to index into correct PHT using 4 least significant bits of address
    uint32_t  lhrIndex = address << 28;
    lhrIndex = lhrIndex >> 28; // isolate right 4 bits

    //get current LHR
    branch_predictor->LHR.currentLocalHistoryRegister = localHistoryRegisterArray[lhrIndex];

    //update the pht and LHR---------------

    //update the PHT
    // the pht index will be the lhr + (lhrIndex * 16)
    //get current state of pht
    uint32_t currentState = phtStorage[branch_predictor->LHR.currentLocalHistoryRegister + (lhrIndex * 16)];
    //implement state change 0 = strongly not taken, 1= weakly not taken, 2 = weakly taken, 3 = strongly taken
    if(currentState == 0){
        if(prediction == 1){ // move to 1 weakly not taken
            phtStorage[branch_predictor->LHR.currentLocalHistoryRegister + (lhrIndex * 16)] = 1; //0 is not taken, 1 is taken
        }
        else{ // stay strongly not taken
            phtStorage[branch_predictor->LHR.currentLocalHistoryRegister + (lhrIndex * 16)] = 0;
        }
    }
    else if(currentState == 1){
        if(prediction == 1){ // move to weakly taken
            phtStorage[branch_predictor->LHR.currentLocalHistoryRegister + (lhrIndex * 16)] = 2; //0 is not taken, 1 is taken
        }
        else{ // go to strongly not taken
            phtStorage[branch_predictor->LHR.currentLocalHistoryRegister + (lhrIndex * 16)] = 0;
        }
    }
    else if(currentState == 2){
        if(prediction == 1){ // move to strongly taken
            phtStorage[branch_predictor->LHR.currentLocalHistoryRegister + (lhrIndex * 16)] = 3; //0 is not taken, 1 is taken
        }
        else{ // go to weakly not taken
            phtStorage[branch_predictor->LHR.currentLocalHistoryRegister + (lhrIndex * 16)] = 1;
        }
    }
    else if(currentState == 3){
        if(prediction == 1){ // stay strongly taken
            phtStorage[branch_predictor->LHR.currentLocalHistoryRegister + (lhrIndex * 16)] = 3; //0 is not taken, 1 is taken
        }
        else{ // go to weakly taken
            phtStorage[branch_predictor->LHR.currentLocalHistoryRegister + (lhrIndex * 16)] = 2;
        }
    }



    //update LHR
    // shift left by 1
    branch_predictor->LHR.currentLocalHistoryRegister = branch_predictor->LHR.currentLocalHistoryRegister << 1;
    if(branch_direction == TAKEN)
    {
        //or a 1 to the end
        branch_predictor->LHR.currentLocalHistoryRegister = branch_predictor->LHR.currentLocalHistoryRegister | 1;

    }
    else{
        // no need to do anything as the shift left already added a 0.
    }

    // need to push the change to the array
    localHistoryRegisterArray[lhrIndex] = branch_predictor->LHR.currentLocalHistoryRegister;


}

void tbl_branch_predictor_cleanup(struct branch_predictor *branch_predictor)
{
    // TODO cleanup if necessary
    free(branch_predictor->localHistoryRegisterArray);
    free(branch_predictor->patternHistoryTableStorage);
}

struct branch_predictor *tbl_branch_predictor_new(uint32_t num_branches,
                                                  struct branch_metadata *branch_metadatas)
{
    struct branch_predictor *tbl_bp = calloc(1, sizeof(struct branch_predictor));
    tbl_bp->cleanup = &tbl_branch_predictor_cleanup;
    tbl_bp->predict = &tbl_branch_predictor_predict;
    tbl_bp->handle_result = &tbl_branch_predictor_handle_result;

    // TODO allocate storage for any data necessary for this branch predictor
    tbl_bp->localHistoryRegisterArray = (uint32_t *)calloc(16,sizeof(uint32_t)); // 4 bits of
    tbl_bp->patternHistoryTableStorage=(uint32_t *)calloc(16*16,sizeof(uint32_t)); // 4 bits of history for each register means 16 entries per register

    return tbl_bp;
}
