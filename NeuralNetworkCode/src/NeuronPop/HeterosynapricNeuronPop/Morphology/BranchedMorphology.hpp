#ifndef _BRANCHED_MORPHOLOGY_HEADER_
#define _BRANCHED_MORPHOLOGY_HEADER_

#include "Morphology.hpp"

struct Branch{
    int branch_id;
    std::vector<int> anterior_branches;

    double synaptic_gap;
    double branch_length;

    std::vector<int> openSynapsesIds;
    //Formed in constructor. The idea is to .pop_back() ids that have been ordered inside here
    
    
    //Methods
    explicit Branch();
};

class BranchedMorphology : public Morphology {

protected:
    int ramifications;
    int totalNoBranches;

    std::vector<std::shared_ptr<Branch>> branches;



};



#endif