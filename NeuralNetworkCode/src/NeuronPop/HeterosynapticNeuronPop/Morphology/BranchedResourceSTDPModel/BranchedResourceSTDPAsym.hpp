#ifndef _SIMPLE_PLASTICITY_ONLY_BRANCHES_
#define _SIMPLE_PLASTICITY_ONLY_BRANCHES_

#include "../BranchedMorphology.hpp"
#include "../SynapseSpines/SynapseSpineResource.hpp"
#include <numeric>


class BranchedMorphology;

class BranchedResourceSTDPAsymmetric : public BranchedMorphology {
//This class models a behaviour based on wi=beta*(alfai/(omega+sum(alfai))), where alfai represents the spine's resources as (Ks*expdt+Kbasal)/(Ns*expdt+Nbasal) with bumps on Ks and Ns
protected:

    //Resource model.
    double availableBranchResources{};

    //Counters
    int STDPDepressionCount{};
    int maxCount{100}; //dependent on dt?, default 10 ms assuming dt=1e-4

    std::vector<std::shared_ptr<SynapseSpineResource>> synapseDataResource;

public:

    //main Methods
    BranchedResourceSTDPAsymmetric()=default;
    explicit BranchedResourceSTDPAsymmetric(GlobalSimInfo * info);
    ~BranchedResourceSTDPAsymmetric() = default;
    
    void LoadParameters(std::vector<std::string> *input) override;    
    void SaveParameters(std::ofstream * stream, std::string neuronPreId) override;

    const std::string GetType() override {return str_BranchedResourceSTDPAsymmetric;};
    
    void advect() override;
    void Reset() override;

    //Outside methods
    void RecordPostSpike() override;
    void RecordExcitatoryPreSpike(unsigned long synSpikerId) override;
    //Getters
    std::valarray<double> GetIndividualSynapticProfile(unsigned long synapseId) const override;

    //Allocation methods
    std::shared_ptr<SynapseSpineBase> AllocateNewSynapse(HeteroCurrentSynapse& synapse) override; //Call the Branched one inside before setting all counters
};

#endif