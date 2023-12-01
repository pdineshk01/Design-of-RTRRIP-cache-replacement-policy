#ifndef RRIP_REPL_H_
#define RRIP_REPL_H_

#include "repl_policies.h"
// #include "math.h"
// Static RRIP
class SRRIPReplPolicy : public ReplPolicy {
    protected:
        // add class member variables here
        uint32_t *rrpv_array;
        uint32_t *rrpv_check;
        uint32_t numLines;
        uint32_t RRPV;
        uint32_t replacement_indx;

    public:
        explicit SRRIPReplPolicy(uint32_t _numLines,uint32_t _RRPV) : numLines(_numLines), RRPV(_RRPV) {
            rrpv_array = gm_calloc<uint32_t>(numLines);
            rrpv_check = gm_calloc<uint32_t>(numLines);
            // set intial rrpv values of all cache lines to 2
            for(uint32_t i=0;i<numLines;i++){
                rrpv_array[i] = RRPV-1;
                rrpv_check[i] = 0;
            }
        }

        ~SRRIPReplPolicy() {
            gm_free(rrpv_array);
            gm_free(rrpv_check);
        }

        void update(uint32_t id, const MemReq* req) {
            // checking if its a miss (then RRPV value of the new cacheline will be set to 2)
            if(replacement_indx == id){
                rrpv_array[id] = RRPV-1;
            }
            // checking if its a hit (then RRPV value of the old cacheline will be set to 0)
            else{
                rrpv_array[id] = 0;
            }
            for(uint32_t i = 0;i<numLines;i++){
                rrpv_check[i]++;
            }
        }

        void replaced(uint32_t id) {
            // setting the replacement index to the cache that is going to be replaced
            replacement_indx = id;
        }

        template <typename C> inline uint32_t rank(const MemReq* req, C cands) {
            uint32_t bestCand = -1;
            // this loop runs until it finds the candiate to be evicted, if not then increments the rrpv value until it finds one
            while(true){
                for (auto ci = cands.begin(); ci != cands.end(); ci.inc()) {
                    if(rrpv_array[*ci] == RRPV){
                        bestCand = *ci;
                        return (bestCand);
                    }
                }
                for(auto ci = cands.begin(); ci != cands.end(); ci.inc()){
                    rrpv_array[*ci]++;
                }
            }
        }

        DECL_RANK_BINDINGS;
};
#endif // RRIP_REPL_H_
