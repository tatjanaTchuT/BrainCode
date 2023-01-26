#ifndef _SIMPLE_PLASTICITY_ONLY_BRANCHES_
#define _SIMPLE_PLASTICITY_ONLY_BRANCHES_

#include "../BranchedMorphology.hpp"

class SimplePlasticityOnlyBranch : public BranchedMorphology {

protected:

    double availableBranchResources;

public:

    explicit SimplePlasticityOnlyBranch(GlobalSimInfo * info);
    virtual ~SimplePlasticityOnlyBranch() = default;
    
    virtual void SaveParameters(std::ofstream * stream, std::string neuronPreId) override;
    virtual void LoadParameters(std::vector<std::string> *input) override;

    virtual const std::string getType() = 0;
    
    virtual void advect()=0;
    virtual void setUpSynapseSlots(std::shared_ptr<Branch> branch) = 0;
};

#endif