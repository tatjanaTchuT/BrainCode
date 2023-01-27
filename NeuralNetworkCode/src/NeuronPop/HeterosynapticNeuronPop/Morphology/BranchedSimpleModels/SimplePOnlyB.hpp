#ifndef _SIMPLE_PLASTICITY_ONLY_BRANCHES_
#define _SIMPLE_PLASTICITY_ONLY_BRANCHES_

#include "../BranchedMorphology.hpp"
#include <numeric>

class BranchedMorphology;

class SimplePlasticityOnlyBranch : public BranchedMorphology {

protected:

    double availableBranchResources{};

public:

    SimplePlasticityOnlyBranch()=default;
    explicit SimplePlasticityOnlyBranch(GlobalSimInfo * info);
    ~SimplePlasticityOnlyBranch() = default;
    
    virtual void LoadParameters(std::vector<std::string> *input) override;    
    virtual void SaveParameters(std::ofstream * stream, std::string neuronPreId) override;


    virtual const std::string getType(){return str_SimplePlasticityOnlyBranches;};
    
    virtual void advect();
};

#endif