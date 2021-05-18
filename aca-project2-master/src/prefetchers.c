//
// This file defines the function signatures necessary for creating the three
// cache systems and defines the prefetcher struct.
//

#include "prefetchers.h"

// Nul Prefetcher
// ============================================================================
uint32_t null_handle_mem_access(struct prefetcher *prefetcher, struct cache_system *cache_system,
                                uint32_t address, bool is_miss)
{
    return 0; // No lines prefetched
}

void null_cleanup(struct prefetcher *prefetcher) {}

struct prefetcher *null_prefetcher_new()
{
    struct prefetcher *null_prefetcher = calloc(1, sizeof(struct prefetcher));
    null_prefetcher->handle_mem_access = &null_handle_mem_access;
    null_prefetcher->cleanup = &null_cleanup;
    return null_prefetcher;
}

// Sequential Prefetcher
// ============================================================================
// TODO feel free to create additional structs/enums as necessary----------------------------------------
uint32_t calculateNextMemoryBockAddress(struct cache_system *cache_system,uint32_t  address){
    uint32_t idxBits = (address & cache_system->set_index_mask) >> cache_system->offset_bits;
    uint32_t tag = address >> (cache_system->offset_bits + cache_system->index_bits);
    uint32_t newIdxBits;
    uint32_t newTag;
    uint32_t newAddress = 0;
    if (idxBits < cache_system->num_sets-1)
    {
        newIdxBits = idxBits + 1;
        newTag = tag;
    }
    else
    {
        newIdxBits = 0;
        newTag = tag +1;
    }
    // create new address
    //shift tag bits over left by the offset_bits + the idx bits
    newTag = newTag << (cache_system->offset_bits + cache_system->index_bits);
    //shift idx bits over left by the number of offset_bits
    newIdxBits = newIdxBits << cache_system->offset_bits;
    //or the tag and idx bits onto the new address which is all 00000000's
    newAddress = newAddress | newTag | newIdxBits;
    return newAddress;
}
//TODO -----------------------------------------------------------

uint32_t sequential_handle_mem_access(struct prefetcher *prefetcher,
                                      struct cache_system *cache_system, uint32_t address,
                                      bool is_miss) {
    // TODO: Return the number of lines that were prefetched.
    uint32_t * voidArrayInt = ((uint32_t *)prefetcher->data); // cast the void pointer
    uint32_t numberOfLinesToPrefetch = voidArrayInt[0];

    uint32_t nextAddress = calculateNextMemoryBockAddress(cache_system,address);
    for (int i = 0; i < numberOfLinesToPrefetch; i++)
    {
        // call mem access with an address in that cache line
        cache_system_mem_access(cache_system, nextAddress, 'R', true);
        //update the address to the next address
        nextAddress = calculateNextMemoryBockAddress(cache_system,nextAddress);
    }
    return numberOfLinesToPrefetch;
}


void sequential_cleanup(struct prefetcher *prefetcher)
{
    // TODO cleanup any additional memory that you allocated in the
    // sequential_prefetcher_new function.
    free(prefetcher->data);
}

struct prefetcher *sequential_prefetcher_new(uint32_t prefetch_amount)
{
    struct prefetcher *sequential_prefetcher = calloc(1, sizeof(struct prefetcher));
    sequential_prefetcher->handle_mem_access = &sequential_handle_mem_access;
    sequential_prefetcher->cleanup = &sequential_cleanup;

    // TODO allocate any additional memory needed to store metadata here and
    // assign to adjacent_prefetcher->data.
    sequential_prefetcher->data = (uint32_t *)calloc(1,sizeof(uint32_t));
    uint32_t* numPrefetch = ((uint32_t*)sequential_prefetcher->data); // cast the void pointer
    numPrefetch[0] = prefetch_amount; // pass along the prefetch amount
    return sequential_prefetcher;
}

// Adjacent Prefetcher
// ============================================================================
uint32_t adjacent_handle_mem_access(struct prefetcher *prefetcher,
                                    struct cache_system *cache_system, uint32_t address,
                                    bool is_miss)
{
    // TODO perform the necessary prefetches for the adjacent strategy.
    // This function should call the cache_system_mem_access function to prefetch lines.
    // It is important to pass `true` to the `is_prefetch` parameter so you don't end up in an infinite-prefetch loop.
    //-------------------------------------------------------------------------------
    // calculate new inx and tag based on current cache line. offset doesn't matter because we want to pull the entire cache line which can hold multiple adresses
    uint32_t idxBits = (address & cache_system->set_index_mask) >> cache_system->offset_bits;
    uint32_t tag = address >> (cache_system->offset_bits + cache_system->index_bits);
    uint32_t newIdxBits;
    uint32_t newTag;
    uint32_t newAddress = 0;
    if (idxBits < cache_system->num_sets-1)
    {
        newIdxBits = idxBits + 1;
        newTag = tag;
    }
    else
    {
        newIdxBits = 0;
        newTag = tag +1;
    }
    // create new address
    //shift tag bits over left by the offset_bits + the idx bits
    newTag = newTag << (cache_system->offset_bits + cache_system->index_bits);
    //shift idx bits over left by the number of offset_bits
    newIdxBits = newIdxBits << cache_system->offset_bits;
    //or the tag and idx bits onto the new address which is all 00000000's
    newAddress = newAddress | newTag | newIdxBits;
    // call mem access with an address in that cache line
    cache_system_mem_access(cache_system,newAddress,'R',true);


    // TODO: Return the number of lines that were prefetched.
    return 1;
    //-------------------------------------------------------------------
}

void adjacent_cleanup(struct prefetcher *prefetcher)
{
    // TODO cleanup any additional memory that you allocated in the
    // adjacent_prefetcher_new function.
}

struct prefetcher *adjacent_prefetcher_new()
{
    struct prefetcher *adjacent_prefetcher = calloc(1, sizeof(struct prefetcher));
    adjacent_prefetcher->handle_mem_access = &adjacent_handle_mem_access;
    adjacent_prefetcher->cleanup = &adjacent_cleanup;

    // TODO allocate any additional memory needed to store metadata here and
    // assign to adjacent_prefetcher->data.

    return adjacent_prefetcher;
}

// Custom Prefetcher
// ============================================================================
// TODO feel free to create additional structs/enums as necessary
uint32_t calculateStridedMemoryBockAddress(struct cache_system *cache_system,uint32_t  address, int stride, bool strideDirectionForward){
    uint32_t newAddress = address;
    if(strideDirectionForward == true)
    {
        newAddress = newAddress + stride;
    }
    else //we need to go  backward and we already checked if it was feasable
    {
        newAddress = newAddress - stride;
    }

    return newAddress;
}
//TODO ------------------------------------------------------------Above is new function
uint32_t custom_handle_mem_access(struct prefetcher *prefetcher, struct cache_system *cache_system,
                                  uint32_t address, bool is_miss)
{
    //===================================================================================================
    //===================================================================================================
    //Manual Tune
    // Option 1: Load next cache line as default if both previous address and strided prefetchers fall through
                //This optimizes the hit ratio by removing compulsory misses
                //This increases the number of prefetches drastically. The number of prefetches will always be equal to adjacent.

    //Option 2: Don't load the next cache line as default.
                // This optimizes the bandwidth and reduces the number of prefetches
                //This does not lower hit rate significantly but does preform worse than adjacent on some inputs.

    uint32_t ManualTuneOptimization = 1; // choose 1 or 2

    //===================================================================================================
    //===================================================================================================

    // TODO perform the necessary prefetches for your custom strategy.
    uint32_t sizeOfRPT = prefetcher->sizeOfReferancePredictionTable; // grab the size of the RPT
    uint32_t writeHead = prefetcher->rptWriteHead;
    uint32_t actualPreviousAddress = prefetcher->actualPreviousAddress;
    //cast the void pointers
    //cast the void pointers for initialization
    uint32_t* blockAddressArray = ((uint32_t*)prefetcher->blockAddressArray);
    uint32_t* previousAddressArray = ((uint32_t*)prefetcher->previousAddressArray);
    uint32_t* stateArray = ((uint32_t*)prefetcher->stateArray); // 0 =initial, 1=transient, 2 = steady state, 3 == no prediction
    bool isCorrect = false;
    bool foundMatchingAddress = false;
    uint32_t matchingAddressIndex = 0;
//TESTING ----
//    printf("blockAdress --- previous --- state\n");
//    for(int i = 0; i < sizeOfRPT; i++)
//    {
//        printf("%x --- %x --- %d \n", blockAddressArray[i], previousAddressArray[i], stateArray[i]);
//    }

//TESTING----
    //printf("This is the address %x \n", address);
    uint32_t zerroAddress = 0;
    //shift tag bits over left by the offset_bits + the idx bits
    uint32_t  newTag = (address >> (cache_system->offset_bits + cache_system->index_bits)) << (cache_system->offset_bits + cache_system->index_bits);
    //shift idx bits over left by the number of offset_bits
    uint32_t newIdxBits = ((address & cache_system->set_index_mask) >> cache_system->offset_bits) << cache_system->offset_bits;
    //or the tag and idx bits onto the new address which is all 00000000's
     uint32_t currentBlockAddress = zerroAddress | newTag | newIdxBits;

    //printf("This is the block address %x \n", currentBlockAddress);
    // STATE: 0 =initial, 1=transient, 2 = steady state, 3 == no prediction

    //REFERENCE PREDICTION ALGORITHM
    //------------------------------------------------------------------------------INIT and UPDATE RPT
    //check to see if the tag and index of the current address has a match in the block address array
    if(prefetcher->rptFull == false){ // speed up rpt check
        sizeOfRPT = writeHead + 2;
    }

    for (uint32_t i = 0; i < sizeOfRPT; i++)
    {
        if (currentBlockAddress == blockAddressArray[i]) // the address has already been used once before
        {

            foundMatchingAddress = true;
            matchingAddressIndex = i;
            //calculate if the entry is correct or incorrect by checking the actual previous against the entry previous
            if ( actualPreviousAddress == previousAddressArray[i] ) // correct
            {
                isCorrect = true;
            } else // incorrect
            {
                isCorrect = false;
            }
            break;
        }

    }

    //---------------------------- Updating an existing line in RPT
    if(foundMatchingAddress == true) // there is a matching entry so we need to update
    {
        //we only want to update if there is a change in address
        if(currentBlockAddress != actualPreviousAddress) //this prevents the table from guessing itself
        {

            uint32_t state = stateArray[matchingAddressIndex];

            //a Transition; incorrect and state = initial
            if (isCorrect == false && state == 0) {

                previousAddressArray[matchingAddressIndex] = actualPreviousAddress; // set previous address to actual previous address
                stateArray[matchingAddressIndex] = 1; // set state to transient
            }
            //b Moving to/being in steady state; correct and state = inital, transient, or steady
            if (isCorrect == true && (state == 0 || state == 1 || state == 2)) {
                // correct previous address so no change
                stateArray[matchingAddressIndex] = 2; // set state to steady
            }

            //c steady state is over, back to init; incorrect and state = steady
            if (isCorrect == false && state == 2) {

                previousAddressArray[matchingAddressIndex] = actualPreviousAddress; // set previous address to actual previous address
                stateArray[matchingAddressIndex] = 0; // set state to initial
            }

            //d detection of irregular pattern: incorrect and state == transient
            if (isCorrect == false && state == 1) {

                previousAddressArray[matchingAddressIndex] = actualPreviousAddress; // set previous address to actual previous address
                stateArray[matchingAddressIndex] = 3; // set state to no prediction
            }
            //e no Prediction state is over, back to transient; correct and state = no prediction
            if (isCorrect == true && state == 3) {
                // correct previous address so no change
                stateArray[matchingAddressIndex] = 1; // set state to transient
            }

            //f Irregular pattern ; incorrect and state = no prediction
            if (isCorrect == false && state == 3) {

                previousAddressArray[matchingAddressIndex] = actualPreviousAddress; // set previous address to actual previous address
                //leave state unchanged
            }
        }
    }
    else //--------------------------------------------INIT creating new line in RPT
    {
        //if we are at end of alloted memory set the write head back to 0 to start overwriting the begining
        if(writeHead == sizeOfRPT)
        {
            writeHead = 0;
            prefetcher->rptFull = true;
            //make entry at 0
            blockAddressArray[writeHead] = currentBlockAddress;
            previousAddressArray[writeHead] = actualPreviousAddress;
            stateArray[writeHead] = 0;
            //increment write Head
            writeHead+=1;
            prefetcher->rptWriteHead=writeHead;//update the prefetcher data
        }
        else //we are not at end of RPT so we can set the RPT at the write Head
        {
            blockAddressArray[writeHead] = currentBlockAddress;
            previousAddressArray[writeHead] = actualPreviousAddress;
            stateArray[writeHead] = 0;
            //increment write Head
            writeHead+=1;
            prefetcher->rptWriteHead=writeHead; // update it in the prefetcher
            //printf("set RPT current address : %x \n", currentBlockAddress);
        }
    }
    //========================================================================Update stride information
    //if there is a stride calculate the stride and if the guessed stride was correct
    bool isStridePrediction = false;
    if(currentBlockAddress != actualPreviousAddress)
    {
        isStridePrediction = true; // so we know we have a prediction later
        bool correctStride = false;
        if(currentBlockAddress-actualPreviousAddress == prefetcher->memAccessStride)
        {
            //the stride is correct
            correctStride = true;
        }
        else
        {
            // the stride is incorrect
            correctStride = false;
        }

        //------------------------------------------------------------------ state machine update
        //a Transition; incorrect and state = initial
        if (correctStride == false && prefetcher->strideState == 0) {

            prefetcher->memAccessStride = currentBlockAddress-actualPreviousAddress; // correct the stride
            prefetcher->strideState = 1; // set state to transient
        }
        //b Moving to/being in steady state; correct and state = inital, transient, or steady
        if (correctStride == true && (prefetcher->strideState == 0 || prefetcher->strideState == 1 || prefetcher->strideState == 2)) {
            // correct previous address so no change
            prefetcher->strideState = 2; // set state to steady
        }

        //c steady state is over, back to init; incorrect and state = steady
        if (correctStride == false && prefetcher->strideState == 2) {

            prefetcher->memAccessStride = currentBlockAddress-actualPreviousAddress; // correct the stride
            prefetcher->strideState = 0; // set state to initial
        }

        //d detection of irregular pattern: incorrect and state == transient
        if (correctStride == false && prefetcher->strideState == 1) {

            prefetcher->memAccessStride = currentBlockAddress-actualPreviousAddress; // correct the stride
            prefetcher->strideState = 3; // set state to no prediction
        }
        //e no Prediction state is over, back to transient; correct and state = no prediction
        if (correctStride == true && prefetcher->strideState == 3) {
            // correct previous address so no change
            prefetcher->strideState = 1; // set state to transient
        }

        //f Irregular pattern ; incorrect and state = no prediction
        if (correctStride == false && prefetcher->strideState == 3) {

            prefetcher->memAccessStride = currentBlockAddress-actualPreviousAddress; // correct the stride
            //leave state unchanged
        }
        //------------------------------
    }


    //=========================================================================Generation of prefetch
    //make a prefetch if in the init, transiant, or steady state. don't make one if there wasn't an entry
    //don't make a prefetch if the state == no prediction
    //first check to see if there is an entry with a prediction
    for (uint32_t i = 0; i < sizeOfRPT; i++)
    {
        if (currentBlockAddress == previousAddressArray[i]) //there is a prediction
        {
            if ( stateArray[i] == 0 || stateArray[i] == 1 ||stateArray[i] == 2 )
            {
                uint32_t newAddress = blockAddressArray[i];//block array is the next predicted address block
                //check that the new address isn't already in cache
                uint32_t set_idx = (newAddress & cache_system->set_index_mask) >> cache_system->offset_bits;
                uint32_t tag = newAddress >> (cache_system->offset_bits + cache_system->index_bits);
                if(cache_system_find_cache_line(cache_system, set_idx, tag) == NULL) // it isn't in cache yet
                {
                    //prefetch the prediction
                    cache_system_mem_access(cache_system,newAddress,'R',true);
                    prefetcher->actualPreviousAddress = currentBlockAddress;// last thing to do is update the actual previous address
                    return 1; // we made a prefetch
                }

            }
            break;
        }

    }
    //================================= stride prefetch generation
    //if we don't get a prefetch from a address patern we will try based on a stride and use a 2bit predictor to make the desission.
    if(isStridePrediction == true)// if we have a prediction
    {
        if (prefetcher->strideState == 1 ||prefetcher->strideState == 2 )
        {
            //determine if stride is forwards or backwards
            bool strideIsForward = true;
            bool isFeasable = false;
            if(prefetcher->memAccessStride <=0) // if the stride is 0 or less it is backward
            {
                strideIsForward = false;
                prefetcher->memAccessStride = abs(prefetcher->memAccessStride); //take the absolute value
                //check if it is going to take up back to negative adress values that don't exist
                if(currentBlockAddress - prefetcher->memAccessStride < 0)
                {
                    isFeasable = false;
                }
                else
                {
                    isFeasable = true;
                }

            }
            else // the stride is going forward in memory
            {
                isFeasable = true;
                strideIsForward= true;
            }

            if(isFeasable == true)
            {
                //calculate the next address
                uint32_t nextStrideAddress = calculateStridedMemoryBockAddress(cache_system, currentBlockAddress,
                                                                               prefetcher->memAccessStride,
                                                                               strideIsForward);
                uint32_t set_idx = (nextStrideAddress & cache_system->set_index_mask) >> cache_system->offset_bits;
                uint32_t tag = nextStrideAddress >> (cache_system->offset_bits + cache_system->index_bits);
                if (cache_system_find_cache_line(cache_system, set_idx, tag) == NULL) // it isn't in cache yet
                {
                    cache_system_mem_access(cache_system, nextStrideAddress, 'R', true);
                    prefetcher->actualPreviousAddress = currentBlockAddress;// last thing to do is update the actual previous address
                    return 1; // we made a prefetch
                }
            }
        }
    }

    prefetcher->actualPreviousAddress = currentBlockAddress;// last thing to do is update the actual previous address
    //-------------------------------------------------------------------------------
    //to get better hit ratio and reduce many compulsory misses
    if(ManualTuneOptimization == 1)
    {
        cache_system_mem_access(cache_system, calculateNextMemoryBockAddress(cache_system,currentBlockAddress), 'R', true);
        return 1;
    }
    //------------

    // TODO: Return the number of lines that were prefetched.
    return 0; // we didn't prefetch
}

void custom_cleanup(struct prefetcher *prefetcher)
{
    // TODO cleanup any additional memory that you allocated in the
    // adjacent_prefetcher_new function.
    free(prefetcher->data);
    free(prefetcher->blockAddressArray);
    free(prefetcher->previousAddressArray);
    free(prefetcher->stateArray);
}

struct prefetcher *custom_prefetcher_new()
{
    struct prefetcher *custom_prefetcher = calloc(1, sizeof(struct prefetcher));
    custom_prefetcher->handle_mem_access = &custom_handle_mem_access;
    custom_prefetcher->cleanup = &custom_cleanup;

    // TODO allocate any additional memory needed to store metadata here and
    // assign to adjacent_prefetcher->data.
    //assign memory for referance Prediction Table
    custom_prefetcher->sizeOfReferancePredictionTable= 1000000;
    uint32_t sizeOfReferancePredictionTable = custom_prefetcher->sizeOfReferancePredictionTable;
    custom_prefetcher->rptWriteHead = 0;//set the write head for the RPT
    custom_prefetcher->rptFull = false;
    custom_prefetcher->actualPreviousAddress = 5; //force the first table check to be incorrect because we don't have a value yet
    //create memory for the RPT

    custom_prefetcher->blockAddressArray = (uint32_t*)calloc(sizeOfReferancePredictionTable,sizeof(uint32_t));
    custom_prefetcher->previousAddressArray = (uint32_t*)calloc(sizeOfReferancePredictionTable,sizeof(uint32_t));
    custom_prefetcher->stateArray = (uint32_t*)calloc(sizeOfReferancePredictionTable,sizeof(uint32_t));

    //cast the void pointers for initialization
    uint32_t* addA = ((uint32_t*)custom_prefetcher->blockAddressArray);
    uint32_t* prevAddA = ((uint32_t*)custom_prefetcher->previousAddressArray);
    uint32_t* stateA = ((uint32_t*)custom_prefetcher->stateArray);

    //Initiallize the rpt
    for (int i = 0; i <  sizeOfReferancePredictionTable; i++)
    {
        addA[i]=0;
        prevAddA[i]=0;
        stateA[i]=0;
    }
//===============================Create stride predictor
    custom_prefetcher->memAccessStride = 0;
    custom_prefetcher->strideState = 0;

    return custom_prefetcher;
}
