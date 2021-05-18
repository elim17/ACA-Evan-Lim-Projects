//
// This file contains the implementations for the functions defined in
// memory_system.h.
//

#include "memory_system.h"

struct cache_system *cache_system_new(uint32_t line_size, uint32_t sets, uint32_t associativity)
{
    struct cache_system *cs = malloc(sizeof(struct cache_system));
    cs->line_size = line_size;
    cs->num_sets = sets;
    cs->associativity = associativity;
    struct cache_system_stats stats = {0, 0, 0, 0};
    cs->stats = stats;


    cs->index_bits = 0;
    cs->offset_bits = 0;
    cs->tag_bits = 0;
    //-------------------------------------------
    // TODO: calculate the index bits, offset bits and tag bits.
        cs->index_bits = log2(cs->num_sets); // num_sets = number of blocks/associativity
        cs->offset_bits = log2(cs->line_size);// line_size = block size
        cs->tag_bits = 32- (cs->index_bits + cs->offset_bits);
    //-------------------------------------------


    cs->offset_mask = 0xffffffff >> (32 - cs->offset_bits);
    cs->set_index_mask = 0xffffffff >> cs->tag_bits;

    printf("\nCache System Geometry:\n");
    printf("Index bits: %d\n", cs->index_bits);
    printf("Offset bits: %d\n", cs->offset_bits);
    printf("Tag bits: %d\n", cs->tag_bits);
    printf("Offset mask: 0x%x\n", cs->offset_mask);
    printf("Set index mask: 0x%x\n", cs->set_index_mask);

    // We need to allocate an array of cache lines representing the cache lines
    // across all of the sets in the cache. We are using a single 1-D array
    // where every "cs->associativity"-sized block of elements represents one
    // set.
    //
    // For example, to access the 2nd element in the 3rd set (assuming
    // associativity = 4), you would access the element at index 3*4 + 1.
    cs->cache_lines = calloc(cs->num_sets * cs->associativity, sizeof(struct cache_line));
    return cs;
}

void cache_system_cleanup(struct cache_system *cache_system)
{
    free(cache_system->cache_lines);
    cache_system->replacement_policy->cleanup(cache_system->replacement_policy);
    free(cache_system->replacement_policy);
}

void cache_system_mem_access(struct cache_system *cache_system, uint32_t address, char rw)
{
    cache_system->stats.accesses++;

    uint32_t offset = (address & cache_system->offset_mask);
    uint32_t set_idx = (address & cache_system->set_index_mask) >> cache_system->offset_bits;
    uint32_t tag = address >> (cache_system->offset_bits + cache_system->index_bits);
    // TODO Modified by adding these vars
    //-----------------------------------
    bool hadCacheHit = false;
    bool hadEviction = false;
    int inSetIndex = -1;
    //---------------------------------
    struct cache_line *cl = cache_system_find_cache_line(cache_system, set_idx, tag);
    if (cl == NULL || cl->status == INVALID) { // cache miss
        hadCacheHit = false;
        printf("  0x%x miss\n", address);
        cache_system->stats.misses++;

        // See if there's an open index.
        int insert_index = -1;
        int set_start = set_idx * cache_system->associativity;
        struct cache_line *start = &cache_system->cache_lines[set_start];
        for (int i = 0; start + i < start + cache_system->associativity; i++) {
            if ((start + i)->status == INVALID) {
                insert_index = i;
                break;
            }
        }

        if (insert_index < 0) {
            // An eviction is necessary. Call the replacement policy's eviction
            hadEviction = true; // TODO bool to pass through
            // index function.
            int evicted_index = (*cache_system->replacement_policy->eviction_index)(
                cache_system->replacement_policy, cache_system, set_idx);

            // Check if the eviction requires writeback.
            struct cache_line evicted = cache_system->cache_lines[set_start + evicted_index];
            if (evicted.status == MODIFIED) {
                cache_system->stats.dirty_evictions++;
            }

            printf("  evict %s cache line from set %d index %d\n",
                   (evicted.status == MODIFIED ? "dirty" : "clean"), set_idx, evicted_index);

            // Use the evicted index as the insert index.
            insert_index = evicted_index;
        }

        printf("  store cache line with tag 0x%x in set %d index %d\n", tag, set_idx, insert_index);

        // Change the tag of the cache line, and set cl to this cache line.
        cl = &cache_system->cache_lines[set_start + insert_index];
        cl->tag = tag;
        cl->status = (rw == 'W') ? MODIFIED : EXCLUSIVE;
        inSetIndex = insert_index;//TODO
    } else { // cache hit
        // get index to pass on
    // TODO -------------------------------------------
        int set_start = set_idx * cache_system->associativity;
        inSetIndex = 0;
        for(int i = set_start; i < (set_start + cache_system-> associativity); i++)
        {
            if(cache_system->cache_lines[i].tag == tag)
            {
                break; // we found the index of the hit
            }
            inSetIndex++; // increment index //TODO
        }
        hadCacheHit = true; //TODO bool to pass through
    //TODO-----------------------------------------------------------------------

        printf("  0x%x hit: set %d, tag 0x%x, offset %d\n", address, set_idx, tag, offset);
        cache_system->stats.hits++;
        if (rw == 'W') cl->status = MODIFIED;

    }

    // Let the replacement policy know that the cache line was accessed. //TODO MODIFIED-> Extra info passed through
    (*cache_system->replacement_policy->cache_access)(cache_system->replacement_policy,
                                                      cache_system, set_idx, tag,hadCacheHit, hadEviction, inSetIndex);
}

struct cache_line *cache_system_find_cache_line(struct cache_system *cache_system, uint32_t set_idx,
                                                uint32_t tag)
{
    // TODO Return a pointer to the cache line within the given set that has the given tag. If no such element exists, then return NULL.
    //-------------------------------------------------------
    uint32_t numCacheLines = cache_system->num_sets * cache_system->associativity;
    uint32_t setCacheLineStartingIndex = set_idx * cache_system->associativity;
    if ((setCacheLineStartingIndex + cache_system-> associativity) > numCacheLines) // check to make sure the line exists
    {
        return NULL;
    }
    for(int i = setCacheLineStartingIndex; i < (setCacheLineStartingIndex + cache_system-> associativity); i++)
    {
        if(cache_system->cache_lines[i].tag == tag)
        {
            return &cache_system->cache_lines[i];
        }

    }

    //-------------------------------------------------------

    return NULL;
}
