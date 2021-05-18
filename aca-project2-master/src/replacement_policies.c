//
// This file contains all of the implementations of the replacement_policy
// constructors from the replacement_policies.h file.
//
// It also contains stubs of all of the functions that are added to each
// replacement_policy struct at construction time.
//
// ============================================================================
// NOTE: It is recommended that you read the comments in the
// replacement_policies.h file for further context on what each function is
// for.
// ============================================================================
//

#include "replacement_policies.h"

// LRU Replacement Policy
// ============================================================================
// TODO feel free to create additional structs/enums as necessary

void lru_cache_access(struct replacement_policy *replacement_policy,
                      struct cache_system *cache_system, uint32_t set_idx, uint32_t tag, bool hadCacheHit, bool hadEviction, int inSetIndex)
{
    // TODO update the LRU replacement policy state given a new memory access
    // Update information in data pointer. eg. array to keep track of LRU, I'm using it like a clock


    uint32_t setStartingIndex = set_idx * cache_system->associativity; // create starting index for the given set
    uint32_t* dataArrayClock = ((uint32_t*)replacement_policy->data); // cast the void pointer

    //if there was a hit then we simply update the rest of the non zero cash data clocks in the set, the hit index clock should be reset
    if(hadCacheHit)
    {
        for(int i = setStartingIndex; i < (setStartingIndex + cache_system-> associativity); i++)
        {
            if(dataArrayClock[i] != 0){ // non zero as 0 is default and means empty line
                // update the clocks
                dataArrayClock[i] = dataArrayClock[i] + 1;
            }
        }
        //reset the hit index to one
        dataArrayClock[setStartingIndex + inSetIndex] = 1;
    }
    else
    {
        // else if there was a miss then we have to see if there was an eviction or an open spot
        //if there was an eviction then we need to update the data clocks and reset the evicted index
        if(hadEviction){
            for(int i = setStartingIndex; i < (setStartingIndex + cache_system-> associativity); i++)
            {
                // update the clocks// should not be any 0's because an eviction means the set is full
                dataArrayClock[i] = dataArrayClock[i] + 1;
            }
            //reset the hit index to one
            dataArrayClock[setStartingIndex + inSetIndex] = 1;
        }
        else{
            //otherwise we need to set this index in the data as most recently used(eg = 1) and everything that isn't 0 gets increased in the data clock
            for(int i = setStartingIndex; i < (setStartingIndex + cache_system-> associativity); i++)
            {
                if(dataArrayClock[i] != 0){ // non zero as 0 is default and means empty line
                    // update the clocks
                    dataArrayClock[i] = dataArrayClock[i] + 1;
                }
            }
            //reset the hit index to one
            dataArrayClock[setStartingIndex + inSetIndex] = 1;

        }


    }
}

uint32_t lru_eviction_index(struct replacement_policy *replacement_policy,
                            struct cache_system *cache_system, uint32_t set_idx)
{
    // TODO return the index within the set that should be evicted.
    uint32_t setStartingIndex = set_idx * cache_system->associativity; // create starting index for the given set
    uint32_t* dataArrayClock = ((uint32_t*)replacement_policy->data); // cast the void pointer

    uint32_t setIndexCounter = 0; // this keeps track of the index in the set while i is the index in all of the cache lines
    uint32_t indexStore = 0; // this stores the final largest index
    uint32_t tempLongestTime = dataArrayClock[setStartingIndex]; // set the temp to the first element in the set
    // iterate through the set and find the largest clock time. This has been untouched the longest and shoudl be evicted
    for(int i = setStartingIndex; i < (setStartingIndex + cache_system-> associativity); i++)
    {
        if(dataArrayClock[i] > tempLongestTime)
        {
            tempLongestTime = dataArrayClock[i];
            indexStore = setIndexCounter;
        }
        setIndexCounter++;
    }

    return indexStore;
}

void lru_replacement_policy_cleanup(struct replacement_policy *replacement_policy)
{
    // TODO cleanup any additional memory that you allocated in the
    // lru_replacement_policy_new function.
    // dealocate the lru_rp-> data
    free(replacement_policy->data);
    //-----------------------------
}

struct replacement_policy *lru_replacement_policy_new(uint32_t sets, uint32_t associativity)
{
    struct replacement_policy *lru_rp = calloc(1, sizeof(struct replacement_policy));
    lru_rp->cache_access = &lru_cache_access;
    lru_rp->eviction_index = &lru_eviction_index;
    lru_rp->cleanup = &lru_replacement_policy_cleanup;

    // TODO allocate any additional memory to store metadata here and assign to
    // lru_rp->data.
    // creating the table to use. malloc
    lru_rp->data = (uint32_t*)calloc(sets * associativity,sizeof(uint32_t));

    // --------------------------------------------------------
    return lru_rp;
}

// RAND Replacement Policy
// ============================================================================
void rand_cache_access(struct replacement_policy *replacement_policy,
                       struct cache_system *cache_system, uint32_t set_idx, uint32_t tag, bool hadCacheHit, bool hadEviction, int inSetIndex)
{
    // TODO update the RAND replacement policy state given a new memory access
    //No reason to keep data-> no need to keep track of anything.
}

uint32_t rand_eviction_index(struct replacement_policy *replacement_policy,
                             struct cache_system *cache_system, uint32_t set_idx)
{
    // TODO return the index within the set that should be evicted.

    return rand() % cache_system->associativity; // pick random number between 0 and accociativity
}

void rand_replacement_policy_cleanup(struct replacement_policy *replacement_policy)
{
    // TODO cleanup any additional memory that you allocated in the
    // rand_replacement_policy_new function.
}

struct replacement_policy *rand_replacement_policy_new(uint32_t sets, uint32_t associativity)
{
    // Seed randomness
    srand(time(NULL));

    struct replacement_policy *rand_rp = malloc(sizeof(struct replacement_policy));
    rand_rp->cache_access = &rand_cache_access;
    rand_rp->eviction_index = &rand_eviction_index;
    rand_rp->cleanup = &rand_replacement_policy_cleanup;

    // TODO allocate any additional memory to store metadata here and assign to
    // rand_rp->data.

    return rand_rp;
}

// LRU_PREFER_CLEAN Replacement Policy
// ============================================================================
void lru_prefer_clean_cache_access(struct replacement_policy *replacement_policy,
                                   struct cache_system *cache_system, uint32_t set_idx, uint32_t tag, bool hadCacheHit, bool hadEviction, int inSetIndex)
{
    // TODO update the LRU_PREFER_CLEAN replacement policy state given a new
    // memory access
    // Update information in data pointer. eg. array to keep track of LRU, I'm using it like a clock


    uint32_t setStartingIndex = set_idx * cache_system->associativity; // create starting index for the given set
    uint32_t* dataArrayClock = ((uint32_t*)replacement_policy->data); // cast the void pointer

    //if there was a hit then we simply update the rest of the non zero cash data clocks in the set, the hit index clock should be reset
    if(hadCacheHit)
    {
        for(int i = setStartingIndex; i < (setStartingIndex + cache_system-> associativity); i++)
        {
            if(dataArrayClock[i] != 0){ // non zero as 0 is default and means empty line
                // update the clocks
                dataArrayClock[i] = dataArrayClock[i] + 1;
            }
        }
        //reset the hit index to one
        dataArrayClock[setStartingIndex + inSetIndex] = 1;
    }
    else
    {
        // else if there was a miss then we have to see if there was an eviction or an open spot
        //if there was an eviction then we need to update the data clocks and reset the evicted index
        if(hadEviction){
            for(int i = setStartingIndex; i < (setStartingIndex + cache_system-> associativity); i++)
            {
                // update the clocks// should not be any 0's because an eviction means the set is full
                dataArrayClock[i] = dataArrayClock[i] + 1;
            }
            //reset the hit index to one
            dataArrayClock[setStartingIndex + inSetIndex] = 1;
        }
        else{
            //otherwise we need to set this index in the data as most recently used(eg = 1) and everything that isn't 0 gets increased in the data clock
            for(int i = setStartingIndex; i < (setStartingIndex + cache_system-> associativity); i++)
            {
                if(dataArrayClock[i] != 0){ // non zero as 0 is default and means empty line
                    // update the clocks
                    dataArrayClock[i] = dataArrayClock[i] + 1;
                }
            }
            //reset the hit index to one
            dataArrayClock[setStartingIndex + inSetIndex] = 1;

        }
    }
}

uint32_t lru_prefer_clean_eviction_index(struct replacement_policy *replacement_policy,
                                         struct cache_system *cache_system, uint32_t set_idx) {
    // TODO return the index within the set that should be evicted.
    uint32_t setStartingIndex = set_idx * cache_system->associativity; // create starting index for the given set
    uint32_t *dataArrayClock = ((uint32_t *) replacement_policy->data); // cast the void pointer
    uint32_t indexStore = 0; // this stores the final largest index

    bool cleanLines = false;
    //figure out if there are clean cashe lines and create new arrays of clean cache lines old index in the set, their curent clock,
    int* cleanCasheLineIndexArray = (int*)calloc(cache_system->associativity,sizeof(int));
    int* cleanCasheClockArray = (int*)calloc(cache_system->associativity,sizeof(int));

    for(int i=0; i < cache_system->associativity; i++)// initiallize arrays to -1
    {
        cleanCasheLineIndexArray[i]= -1;
        cleanCasheClockArray[i] = -1;
    } // now they are initiallized to -1
    int indexCounter1 = 0;
    int arrayIndexCounter1=0;
    for (int i = setStartingIndex; i < (setStartingIndex + cache_system->associativity); i++)
    {
        struct cache_line cl = cache_system->cache_lines[i];//get the cashe line
        if (cl.status != MODIFIED)//check to see if the cashe line is dirty and if
        {
            // there is a non MODIFIED line
            cleanLines = true;
            // create the new arrays
            cleanCasheLineIndexArray[arrayIndexCounter1] = indexCounter1;
            cleanCasheClockArray[arrayIndexCounter1] = (int)dataArrayClock[i];
            arrayIndexCounter1 ++;
        }
        indexCounter1 ++;
    }
    if(cleanLines == true) // if there are clean lines to evict
    {

        uint32_t tempLongestTime = cleanCasheClockArray[0]; // set the temp to the first element in the set
        indexStore = cleanCasheLineIndexArray[0]; // this stores the final largest index
        // iterate through the set and find the largest clock time that has not been modified.
        for (int i = 0; i < cache_system->associativity; i++)
        {
            if(cleanCasheLineIndexArray[i] == -1)
            {break;} // we have reached the end of our clean cashe
            if (cleanCasheClockArray[i] > tempLongestTime )//if the line is clean and has been in longer
            {
                tempLongestTime = cleanCasheClockArray[i];
                indexStore = cleanCasheLineIndexArray[i];// set it back to its correct index in the set
            }
        }
    }
        // if there are no clean, then we run just like LRU normal
    else
    {

        uint32_t setIndexCounter = 0; // this keeps track of the index in the set while i is the index in all of the cache lines
        uint32_t tempLongestTime = dataArrayClock[setStartingIndex]; // set the temp to the first element in the set
        //reset variables
        indexStore = 0; // this stores the final largest index
        // iterate through the set and find the largest clock time. This has been untouched the longest and shoudl be evicted
        for(int i = setStartingIndex; i < (setStartingIndex + cache_system-> associativity); i++)
        {
            if(dataArrayClock[i] > tempLongestTime)
            {
                tempLongestTime = dataArrayClock[i];
                indexStore = setIndexCounter;
            }
            setIndexCounter++;
        }
    }
    // return memory
    free(cleanCasheLineIndexArray);
    free(cleanCasheClockArray);
    return indexStore;
}

void lru_prefer_clean_replacement_policy_cleanup(struct replacement_policy *replacement_policy)
{
    // TODO cleanup any additional memory that you allocated in the
    // lru_prefer_clean_replacement_policy_new function.
    free(replacement_policy->data);
}

struct replacement_policy *lru_prefer_clean_replacement_policy_new(uint32_t sets,
                                                                   uint32_t associativity)
{
    struct replacement_policy *lru_prefer_clean_rp = malloc(sizeof(struct replacement_policy));
    lru_prefer_clean_rp->cache_access = &lru_prefer_clean_cache_access;
    lru_prefer_clean_rp->eviction_index = &lru_prefer_clean_eviction_index;
    lru_prefer_clean_rp->cleanup = &lru_prefer_clean_replacement_policy_cleanup;

    // TODO allocate any additional memory to store metadata here and assign to
    // lru_prefer_clean_rp->data.
    lru_prefer_clean_rp->data = (uint32_t*)calloc(sets * associativity,sizeof(uint32_t));

    return lru_prefer_clean_rp;
}


