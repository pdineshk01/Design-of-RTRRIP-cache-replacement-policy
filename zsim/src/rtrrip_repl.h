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
	//vector <uint32_t> rtvalarray;
	
	int* RTarray;	// array for storing subset of cache blocks from RT filter
	//vector <uint32_t> RTarray;
//	vector <uint32_t> index;
	
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

	 template <typename C> inline void rtfilter(const MemReq* req, C cands)
	{       rt_threshold=0;
		sum=0;


		//info("calling from rtfilter");
		 for(auto ci = cands.begin(); ci != cands.end(); ci.inc())
		{
			sum = sum+rtvalarray[*ci];
		}
		rt_threshold = sum/numLines;

		
		for(auto ci = cands.begin(); ci != cands.end(); ci.inc())
		
		{	//info("iter = %d",iter);
			if(rtvalarray[*ci] >= rt_threshold)
				{	
				RTarray[*ci] = valarray[*ci];	
				}
			else
				{
				RTarray[*ci] = -1;
				}
			
		}

	}



        void update(uint32_t id, const MemReq* req) {
	
	//info("id in update = %d",id);
 
	if(miss)
	{
	valarray[id]  = rrpvmax-1;		// set RRPV value as 2^M - 2, which is 2 in our case
	//info("calling from update miss");
	for(uint32_t i=0;i<numLines;i++)
	{
			rtvalarray[i]++;
	}
		rtvalarray[id] = 0;

	}
	else
	{valarray[id] = 0;
	//  info("calling from update hit");
	for(uint32_t i=0;i<numLines;i++)
	{
			rtvalarray[i]++;
	}
	rtvalarray[id] = 0;

	
	}
	miss = false;   

	
        } 

       

	 void replaced(uint32_t id) {
            miss = true;
//		info("calling from replaced");
			// set as true if a miss occured
        }




        template <typename C> inline uint32_t rank(const MemReq* req, C cands) {
            uint32_t bestCands = -1;
 	


	
	rtfilter(req, cands);
	
	//for (auto ci = cands.begin(); ci != cands.end(); ci.inc()) {
              
	// 	info("RT array = %d", RTarray[*ci]);
	
//	}
//	info("next iter");


				
//	info("outside RTfilter inside rank");
	//info("RT size = %d", uint32_t(RTarray.size()));

          while(true) 
	{   
			//info("inside rank while loop");
		
            for (auto ci = cands.begin(); ci != cands.end(); ci.inc()) {
                bestCands = *ci;
		if((valarray[bestCands] == rrpvmax) &&(RTarray[bestCands]!=-1))
		{
					
				return bestCands;
		        
		}
		
		}

	     for (auto ci = cands.begin(); ci != cands.end(); ci.inc()) {
              
		
		
		if(RTarray[*ci]!= -1) 	
		{	RTarray[*ci]++;
		
			valarray[*ci]++;	
		}
		}
		  	
		
        }

        }

        DECL_RANK_BINDINGS;
        
};
#endif // RTRRIP_REPL_NEW_H_

