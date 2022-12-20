#ifndef _BRANCHED_MORPHOLOGY_HEADER_
#define _BRANCHED_MORPHOLOGY_HEADER_

#include "Morphology.hpp"
#include <string>
#include <numeric>

struct Branch{
    //ID
    int branch_id{};
    //For setup
    std::vector<int> anterior_branches{};

    int synapticGap{};
    int branchLength{};

    std::vector<int> openSynapsesSlots{};//We will pop_back() from here the position of the synapse being setup
    //For the actual checks
    std::vector<bool> spikedSyn{};//Here, with the size of the branch open positions, we will store the bool indicating if the preneuron fired in the timestep
    std::vector<int> branchSynapsesID{}; // Here we store the actual positions of our synapses. Non-occupied will be -1? Or 0 if the openSynapsesPositions starts from 1.

    //Formed in constructor. The idea is to .pop_back() ids that have been ordered inside here
    Branch(size_t synSlots, int gap, int branch_length);
};

class BranchedMorphology : public Morphology {

protected:
//MonoDendriteSTDP moved
    unsigned long synapseIdGenerator{}; // variable used to allocate new synapses. type is legacy compatible
    
    bool distributeWeights{false};
    bool stepWeights{false};
    std::vector<unsigned long> weightStepBoundary{};
    std::vector<double> weightStepValue{};
    unsigned long currWightStepId{};
    
    std::vector<bool> integratePostSpike{};
    std::vector<bool> integratePreSpike{};
    bool postSpiked{false};

    //Weight normalization vars

    int synapticGap{};
    int branchLength{};

//Branched specific
    int ramifications{1};
    int totalNoBranches{2};

    int branchIdGenerator{1};

    std::vector<std::shared_ptr<Branch>> branches{};//unique_ptr's constructor is explicit, so you either need to use emplace_back or stuff.push_back(std::unique_ptr<int>(new int(i)));. Between the two, emplace_back is much cleaner.
    //initialize with the pointer, push_back() the pointer
public:
    explicit BranchedMorphology(GlobalSimInfo * info);
    virtual ~BranchedMorphology() = default;

    //Methods derived from the MonoDendriteSTDP and Morphology classes

    virtual void SaveParameters(std::ofstream * stream, std::string neuronPreId) override;//define
    virtual void LoadParameters(std::vector<std::string> *input) override; //define
    virtual std::string getType() = 0;

    virtual std::shared_ptr<SynapseExt> allocateNewSynapse() = 0; //VERY IMPORTANT that the SynapseExt pointer poiints to a SynapseExtBranched

    virtual void recordPostSpike();// define
    virtual void recordExcitatoryPreSpike(unsigned long synSpikerId); //define
    virtual std::valarray<double> getIndividualSynapticProfile(unsigned long synapseId) const; //define
    virtual std::valarray<double> getOverallSynapticProfile() const;//define
//This has to come from STDP, how they do it. I think that all the base things a dendrite can do, they have to be done by  this abstract class.
//Other methods like allocateSynapse, or allocateBranch, can be specified by each derived class.

    //Branched specific methods
    virtual void SetUpSynapseSlots(int branch_id) = 0; //This function will set up the open synapse slots of a branch object with its id.This one I have to define in the parallel synaptic connectivity masks
    virtual void SetUpBranch (int branch_id);//?
    int GenerateBranchId(){return branchIdGenerator++;}

    virtual bool IsBranched() override {return true;}

    //
    double generateSynapticWeight();// Here we generate the synaptic weight to be allocated when 
};



#endif