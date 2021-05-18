#Custom Prefetcher README   
###Author: Evan Lim
#1. Describe how your prefetcher works.  
My prefetcher works on three levels:
    
1. **Memory Reference Pattern Predictor**   
I used a memory pattern table and two bit predictor to  
    predict the next block of memory based on the current address's historical successor. This process works on two levels:   
    
    1. **Reference Prediction Table(RPT) Update or Initiation Stage**  
    This stage determines if there is already a row in the RPT. If there is,
            the row is updated. If not, an entry is created for the current address. 
            
          * Initiation  
            A new row is initialized with the current address, the previous address, and the state for the two bit predictor is set to initial.
            A single address is stored at the end of each prefetch to preserve the previous address for use.   
          
          * Update  
            Updating the row consists of checking the prediction then updating the state and previous address accordingly. If the
            prediction is wrong the previous address is updated. To account for accessing the same memory address multiple times
            an update only happens if the current and previous addresses are different. 
            The full algorithm for the two bit prediction can be found in 
            ["Effective Hardware-Based Data Prefetching for High-Performance Processors"](https://www.cs.utah.edu/~rajeev/cs7810/papers/chen95.pdf)
            by Tein-Fu Chen. 
            
    2. **Reference Prediction Table(RPT) Generate Prefetch Stage**  
    The generation stage accesses the RPT differently than the update stage. Instead of checking the current address against the 
    address(tag), it looks for a match in the previous addresses. In this way, the address field becomes the predicted next address.
    Then if the state for the prediction is in initial, transient, or steady, and if the predicted next memory block isn't already in the cache, a prefetch is initiated.
    
2. **Stride Prediction**  
To reduce compulsory misses created when the RPT is initializing, a stride predictor will try to predict the next needed memory block
based on the number of addresses jumped with each access. If the Memory Reference Predictor decided not to prefetch, the stride predictor will similarly make a decision. Decisions are also made with
a two bit state machine as described in Chen's Paper.   
   
      1. **Update**  
    Unlike the above predictor which worked off of a deep history, the stride predictor is "almost realtime" based on the stride from the previous address to the current address.
    Updates happen each memory access. The previous prediction is checked against the current stride to determine correctness.
    The state machine is then updated based on if the prediction was correct or not. Finally, the prediction is updated if there was a change.
   
    2. **Prefetch Generation**  
    Like the reference predictor the stride predictor determines a prefetch based on its state machine. A prefetch occurs only from the steady or transient state. Basically, if the stride has stayed the same for a while, it is more likely to prefetch 
     than if the stride has been unpredictable.  

3. **Adjacent Prefetch Mode- Optional**    
To increase the hit ratio, an adjacent prefetch is initiated if both the reference and stride predictors elect not to make a 
prefetch. As discussed below, this significantly increases the number of prefetches. I have created a variable in prefetchers.cpp in the custom prefetcher
that controls this feature. Below are the lines of code to be modified to turn this feature on and off.   

'    

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
'
    

    
          
#2. Explain how you chose that prefetch strategy.

 Both predictors were based on the reference 
        prediction table as described by Tein-Fu Chen in her paper "Effective Hardware-Based Data Prefetching for High-Performance Processors".
        Unfortunately, after finishing most of the implementation, I realized that our simulation does not
        keep a program counter or specify branch instructions as specified in Chen's paper. Instead of predicting based on the program counter and instruction address,
        and calculating the stride, I developed my prefetch based on predicting memory access patterns. This method 
        significantly reduced conflict misses and dirty evictions, but did not significantly impact compulsory misses. This makes sense
        because the reference cache needs an entry before making a prediction. 
        From this point, I realized that I could utilize a block stride predictor to reduce capacity misses created while waiting for a prediction from the reference table. 
        I chose to implement it with the two bit predictor from Chen's paper as well. From there, I found that making the 
        adjacent prefetch in the case were both the reference table prediction and the stride prediction decided not to make a prefetch
        increased the hit rate by removing further compulsory misses. As mentioned below there are some downsides to this feature 
        which is why I clearly marked a variable for manual tuning as explained above. 
        

#3. Discuss the pros and cons of your prefetch strategy.
   * **PROS**
        + This strategy works best with **smaller cache size and smaller cache line size** in comparison to sequential because of the accuracy of this strategy.
        + Detects repeated loops and strides
        + Combined strategy complements each other's weaknesses 
        + Timing for the prefetch is **almost always on time** because it prefetches for the next instruction
        + With Adjacent Turned on:
            * **Hit ratio is better** than both adjacent and sequential in most situations.
            * All three miss types are typically lower than adjacent and sequential. 
            * There are always **less prefetches** than sequential > 1. The number of prefetches always equals adjacent.
        + With Adjacent Turned off: 
            * Number of **prefetches reduces significantly**
            * Hit ratio stays high and beats adjacent and sequential when stride pattern, or loop pattern is prevalent
            * Number of **dirty evictions reduces significantly** because of highly intentional prefetches 
 
   
   * **CONS**
        + Requires **more hardware** than adjacent or sequential
        + Does not catch on to complex stride patterns
        + RPT Takes up a lot of **memory space** so the RPT cache could be **expensive**. 
        + RPT Tag/Index checks **increases number of transistors** to check.
        + RPT Tag checks **increase latencies** as RPT gets bigger. 
        + a bigger RPT can better predict for larger programs using more varied memory, but this increases many of the cons already stated here
        + With Adjacent Turned on:
            * number of prefetches can't be lower than number of memory accesses
            * number of dirty evictions increases 
        + With Adjacent Turned off:
            * **More compulsory misses**
            * Lower hit ratio

#4. Demonstrate that the prefetcher could be implemented in hardware 
   (this can be
   as simple as pointing to an existing hardware prefetcher using the strategy
   or a paper describing a hypothetical hardware prefetcher which implements
   your strategy).
   
   For the most part the hardware needed to implement my prefetcher is exactly the same as the hardware described in
   ["Effective Hardware-Based Data Prefetching for High-Performance Processors"](https://www.cs.utah.edu/~rajeev/cs7810/papers/chen95.pdf)
   by Tein-Fu Chen. The differences are as follows:  
   
   1. Additional hardware/single line cache to store single memory address for previous address
   2. Additional hardware/single line cache to store stride prediction line (stride, two bit state)
   

#5. Cite any additional sources that you used to develop your prefetcher.
S. Byna, Y. Chen, and X.-H. Sun, “A Taxonomy of Data Prefetching Mechanisms,” 2008 International Symposium on Parallel Architectures, Algorithms, and Networks (i-span 2008), 2008.
["A Taxonomy of Data Prefetching Mechanisms"](https://discl.cs.ttu.edu/lib/exe/fetch.php?media=wiki:docs:ispan08.pdf)
