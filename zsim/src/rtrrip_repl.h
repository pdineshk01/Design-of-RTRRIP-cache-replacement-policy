#ifndef RTRRIP_REPL_NEW_H_
#define RTRRIP_REPL_NEW_H_
#include "math.h"
#include "repl_policies.h"

// Static RRIP
class RTRRIPReplPolicynew : public ReplPolicy {
    protected:
        // add class member variables here

	uint32_t id;	    // variable for iterating


        uint32_t numLines;  // no. of blocks in the cache
        uint32_t rrpvmax;   // maximum value of RRPV 
        uint32_t* valarray;	// array for storing block RRPV values
	bool miss = false;	// a flag for knowing whether a miss occured


	uint32_t* rtvalarray;	// array for storing block recency time values
	
	int* RTarray;	// array for storing subset of cache blocks from RT filter
	
	float rt_threshold=0;
	uint32_t sum=0;
		

    public:
       explicit RTRRIPReplPolicynew(uint32_t _numLines,uint32_t _rpvMax) :numLines(_numLines), rrpvmax(_rpvMax)

 {
        
    valarray = gm_calloc<uint32_t>(numLines);
          for(uint32_t i=0;i<numLines;i++)
		{ valarray[i] = rrpvmax-1;
		}

        

 rtvalarray = gm_calloc<uint32_t>(numLines);
          for(uint32_t i=0;i<numLines;i++)
		{ rtvalarray[i] = 0;
		}

        


 RTarray = gm_calloc<int>(numLines);
          for(uint32_t i=0;i<numLines;i++)
		{ RTarray[i] =-1;
		}

        }


        ~RTRRIPReplPolicynew() {
       
     gm_free(valarray);
     gm_free(rtvalarray);
     gm_free(RTarray);

        }

	 template <typename C> inline void rtfilter(const MemReq* req, C cands)		// calculating the threshold value and creating a subset of blocks
	{       rt_threshold=0;
		sum=0;


		 for(auto ci = cands.begin(); ci != cands.end(); ci.inc())
		{
			sum = sum+rtvalarray[*ci];
		}
		rt_threshold = sum/numLines;

		
		for(auto ci = cands.begin(); ci != cands.end(); ci.inc())
		
		{	//info("iter = %d",iter);
			if(rtvalarray[*ci] >= rt_threshold)
				{	
				RTarray[*ci] = valarray[*ci];			        //  Assigning the RRPV value for blocks that have RT value greater than or equal to threshold 
				}
			else
				{
				RTarray[*ci] = -1;					// Set RRPV value as -1 if the data block is not a part of the subset 
				}
			
		}

	}



        void update(uint32_t id, const MemReq* req) {
	
 
	if(miss)
	{
	valarray[id]  = rrpvmax-1;		// set RRPV value as 2^M - 2 for incoming new block, which is 2 in our case
	for(uint32_t i=0;i<numLines;i++)
	{
			rtvalarray[i]++;
	}
		rtvalarray[id] = 0;		// set recency time value as 0 for the new block

	}
	else
	{valarray[id] = 0;			// set RRPV value as 0 for the block which has a hit
	for(uint32_t i=0;i<numLines;i++)
	{
			rtvalarray[i]++;	// increment the recency time value of all blocks 
	}
	rtvalarray[id] = 0;			

	
	}
	miss = false;   

	
        } 

       

	 void replaced(uint32_t id) {
            miss = true;			// set miss as true indicating its a miss scenario
        }




        template <typename C> inline uint32_t rank(const MemReq* req, C cands) {		// rank function for choosing the candidate for eviction
            uint32_t bestCands = -1;
 	


	
	rtfilter(req, cands);	// calling RTfiler for getting the subset of blocks

          while(true) 
	{   
			
		
            for (auto ci = cands.begin(); ci != cands.end(); ci.inc()) {
                bestCands = *ci;
		if((valarray[bestCands] == rrpvmax) &&(RTarray[bestCands]!=-1))		// choosing the candidate based on RRPV value from the subset
		{
					
				return bestCands;
		        
		}
		
		}

	     for (auto ci = cands.begin(); ci != cands.end(); ci.inc()) {
              
		
		
		if(RTarray[*ci]!= -1) 	
		{	RTarray[*ci]++;							// if no candidate is found, increment the RRPV value of each subset block 
		
			valarray[*ci]++;	
		}
		}
		  	
		
        }

        }

        DECL_RANK_BINDINGS;
        
};
#endif // RTRRIP_REPL_NEW_H_

