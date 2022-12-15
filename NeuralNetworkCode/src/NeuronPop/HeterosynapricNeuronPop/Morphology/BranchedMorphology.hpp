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
    Branch();
};

class BranchedMorphology : public Morphology {

protected:
    int ramifications;
    int totalNoBranches;

    std::vector<std::shared_ptr<Branch>> branches;

public:
    explicit BranchedMorphology(GlobalSimInfo * info);
    virtual ~BranchedMorphology() = default; //what does default mean here?

    virtual std::shared_ptr<SynapseExt> allocateNewSynapse() = 0;
    virtual std::string getType() = 0;
    virtual void advect();
    virtual void recordPostSpike();
    virtual void recordExcitatoryPreSpike(unsigned long synSpikerId);
    virtual std::valarray<double> getIndividualSynapticProfile(unsigned long synapseId) const = 0;
    virtual std::valarray<double> getOverallSynapticProfile() const = 0;



    virtual void SaveParameters(std::ofstream * stream, std::string neuronPreId);
    virtual void LoadParameters(std::vector<std::string> *input);
};



#endif