#ifndef _SIMPLE_PLASTICITY_ONLY_BRANCHES_
#define _SIMPLE_PLASTICITY_ONLY_BRANCHES_

#include "../BranchedMorphology.hpp"
#include <numeric>

class BranchedMorphology;

class BranchedResourceSTDPAsymmetric : public BranchedMorphology {

protected:

    //Resource model.
    double availableBranchResources{};

    //Counters
    int STDPDepressionCount{};
    int maxCount{100}; //dependent on dt?, default 10 ms assuming dt=1e-4

public:

    BranchedResourceSTDPAsymmetric()=default;
    explicit BranchedResourceSTDPAsymmetric(GlobalSimInfo * info);
    ~BranchedResourceSTDPAsymmetric() = default;
    
    void LoadParameters(std::vector<std::string> *input) override;    
    void SaveParameters(std::ofstream * stream, std::string neuronPreId) override;


    const std::string GetType() override {return str_SimplePlasticityOnlyBranches;};
    
    void advect() override;
};

#endif