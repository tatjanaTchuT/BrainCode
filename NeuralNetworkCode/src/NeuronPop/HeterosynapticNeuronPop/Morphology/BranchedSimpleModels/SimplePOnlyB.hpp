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
    
    void LoadParameters(std::vector<std::string> *input) override;    
    void SaveParameters(std::ofstream * stream, std::string neuronPreId) override;


    const std::string getType() override {return str_SimplePlasticityOnlyBranches;};
    
    void advect() override;
};

#endif