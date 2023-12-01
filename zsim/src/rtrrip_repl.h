#ifndef RTRRIP_REPL_H_
#define RTRRIP_REPL_H_
#include "repl_policies.h"
#include "cache_arrays.h"
#include "stdio.h"
#include <stdlib.h>
#include <vector>
#include <ctime>
// #include "math.h"
// Static RRIP
// template <class T>
class RTRRIPReplPolicy : public ReplPolicy {
    protected:
        // add class member variables here
        uint32_t *rrpv_array;
        uint32_t* rtvalarray;	// array for storing block recency time values
		uint32_t* RTarray;	// array for storing subset of cache blocks from RT filter
        uint32_t numLines;
        uint32_t RRPV;
        uint32_t replacement_indx;
        float rt_threshold = 0;
        uint32_t rt_sum = 0;

    public:
        explicit RTRRIPReplPolicy(uint32_t _numLines,uint32_t _RRPV) : numLines(_numLines), RRPV(_RRPV) {
            rrpv_array = gm_calloc<uint32_t>(numLines);
            // set intial rrpv values of all cache lines to 2
            for(uint32_t i=0;i<numLines;i++){
                rrpv_array[i] = RRPV-1;
            }
            rtvalarray = gm_calloc<uint32_t>(numLines);
            for(uint32_t i=0;i<numLines;i++){
			    rtvalarray[i] = 0;
		    }
            RTarray = gm_calloc<uint32_t>(numLines);
            for(uint32_t i=0;i<numLines;i++){
			    RTarray[i] = -1;
		    }
        }

        ~RTRRIPReplPolicy() {
            gm_free(rrpv_array);
            gm_free(rtvalarray);
            gm_free(RTarray);
        }

        // template <typename C> inline void rtfilter(const MemReq* req, C cands){	
        //     info("your string here from rtfilter... %d", 200);
        //     for(auto ci = cands.begin(); ci != cands.end(); ci.inc())
        //     {   info("%d", rtvalarray[*ci]);
        //         if(rtvalarray[*ci] >= rt_threshold){
        //             RTarray[*ci] = rrpv_array[*ci];                  
        //         } 
        //     }
        //     //exit(3);
	    // }

        template <typename C> inline void rtfilter(const MemReq* req, C cands){
            rt_threshold = 0;
            rt_sum = 0;
            uint32_t numcands = 0;
            for(auto ci = cands.begin(); ci != cands.end(); ci.inc()){
                  numcands++;
                  rt_sum = rt_sum + rtvalarray[*ci];
            }
            rt_threshold = rt_sum/numcands;
            for(auto ci = cands.begin(); ci != cands.end(); ci.inc())
            {
                if(rtvalarray[*ci] >= rt_threshold){
                    RTarray[*ci] = rrpv_array[*ci];                 
                }
                else{
                    RTarray[*ci] = -1;
                } 
            }
            // for(uint32_t j=0; j<numLines; j++){
            //     if(rtvalarray[j] >= rt_threshold){
            //         //info("hello from here");
            //         RTarray[j] = rrpv_array[j];                 
            //     }
            //     else{
            //         RTarray[j] = -1;
            //     } 
            // }
	    }

        // void rtfilter(const MemReq* req, uint32_t it) {
        //     rt_threshold = 0;
        //     rt_sum = 0;
        //     for(uint32_t i=0; i<cands.numCands(); i++){
        //           rt_sum = rt_sum + rtvalarray[i];
        //     }
        //     rt_threshold = rt_sum/cands.numCands();
        //     for(uint32_t j=0; j<cands.numCands(); j++){
        //         if(rtvalarray[j] >= rt_threshold){
        //             RTarray[j] = rrpv_array[j];                 
        //         }
        //         else{
        //             RTarray[j] = 4;
        //             // RTarray[j] = rrpv_array[j];
        //         } 
        //     }
        // }

        void update(uint32_t id, const MemReq* req) {
            //T Cands = SetAssocCands(0,numLines);
            // checking if its a miss (then RRPV value of the new cacheline will be set to 2)
            if(replacement_indx == id){
                rrpv_array[id] = RRPV-1;
                rtvalarray[id] = 0;
                for(uint32_t i=0; i<numLines; i++)
		        {
			        if(i!=id){
                        rtvalarray[i]++;
			        }  
		        }
            }
            // checking if its a hit (then RRPV value of the old cacheline will be set to 0)
            else{
                rrpv_array[id] = 0;
                rtvalarray[id] = 0;
                for(uint32_t i=0; i<numLines; i++)
		        {
			        if(i!=id){
                        rtvalarray[i]++;
			        }  
		        }
            }
        }


        void replaced(uint32_t id) {
            // setting the replacement index to the cache that is going to be replaced
            replacement_indx = id;
        }

        template <typename C> inline uint32_t rank(const MemReq* req, C cands) {
            uint32_t bestCand = -1;
            rtfilter(req, cands);
            //rtfilter(req, cands);
            // this loop runs until it finds the candiate to be evicted, if not then increments the rrpv value until it finds one
            while(true){
                // uint32_t flag = 0;
                // for (uint32_t i =0;i<numLines;i++){
                //     if(RTarray[i] == 5){
                //         flag++;
                //         info("flag:%d",flag);
                //     }
                // }
                
                // if(flag == numLines){
                //     exit(0);
                // }
                //info("outside RTfilter inside rank");
                for (auto ci = cands.begin(); ci != cands.end(); ci.inc()) {
                    bestCand = *ci;
                    //if(rrpv_array[*ci] == RRPV && RTarray[*ci] < 4){
                    if(RTarray[*ci] == RRPV){
                        //info("%d", uint32_t(bestCand));
			            //info("RTarray[bestcands] before return = %d", uint32_t(RTarray[bestCand]));
                        return (bestCand);
                    }
                }
                for(auto ci = cands.begin(); ci != cands.end(); ci.inc()){
                    bestCand = *ci;
                    if(RTarray[*ci] != -1){
                        //info("RTarray[bestcands] before increment = %d", uint32_t(RTarray[bestCand]));
                        RTarray[*ci]++;
                        rrpv_array[*ci]++;
                    }
                }
            }
        }

        DECL_RANK_BINDINGS;
};
#endif // RTRRIP_REPL_H_
