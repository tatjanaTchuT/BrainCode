#ifndef _BRANCHED_MORPHOLOGY_HEADER_
#define _BRANCHED_MORPHOLOGY_HEADER_

#include "Morphology.hpp"
#include <string>
#include <numeric>

struct Branch{
    //ID
    int branchId{};
    //For setup
    std::vector<int> anteriorBranches{}; 

    int synapticGap{};
    int branchLength{};

    std::vector<int> openSynapsesSlots{};//We will pop_back() from here the position of the synapse being setup. Will push_back() in setup, so no default size
    //For the actual checks
    std::vector<bool> spikedSyn{};//Here, with the size of the branch discrete positions, we will store the bool indicating if the preneuron fired in the timestep
    std::vector<int> branchSynapsesID{}; // Here we store the actual positions of our synapses. Non-occupied will be -1

    //Formed in constructor. The idea is to .pop_back() ids that have been ordered inside here
    Branch()=default;
    Branch(int gap, int branchLength, std::vector<int>anteriorBranches, int branchId);
};

class BranchedMorphology : public Morphology {

protected:
//MonoDendriteSTDP moved
    unsigned long synapseIdGenerator{0}; // variable used to allocate new synapses. type is legacy compatible
    int branchIdGenerator{0}; //Same but in branches. Should be 1 or 0?

    bool distributeWeights{false};
    
    std::vector<bool> integratePostSpike{};//
    std::vector<bool> integratePreSpike{};//These two are created in allocateNewSynapse
    bool postSpiked{false};

    //Weight normalization vars

    int synapticGap{};
    int branchLength{};

//Branched specific
    int branchings{1};



    std::vector<std::shared_ptr<Branch>> branches{};//unique_ptr's constructor is explicit, so you either need to use emplace_back or stuff.push_back(std::unique_ptr<int>(new int(i)));. Between the two, emplace_back is much cleaner.
    //initialize with the pointer, push_back() the pointer
public:
    explicit BranchedMorphology(GlobalSimInfo * info);
    virtual ~BranchedMorphology() = default;

    //Methods derived from the MonoDendriteSTDP and Morphology classes

    virtual void SaveParameters(std::ofstream * stream, std::string neuronPreId) override;//defined
    virtual void LoadParameters(std::vector<std::string> *input) override; //defined
    virtual std::string getType() = 0;

    virtual std::shared_ptr<SynapseExt> allocateNewSynapse() = 0; //VERY IMPORTANT that the SynapseExt pointer poiints to a SynapseExtBranched (in derived classes)

    virtual void recordPostSpike();// defined
    virtual void recordExcitatoryPreSpike(unsigned long synSpikerId); //defined
    virtual std::valarray<double> getIndividualSynapticProfile(unsigned long synapseId) const; //defined in the SynapseExt structs
    virtual std::valarray<double> getOverallSynapticProfile() const;//defined
    virtual void advect()=0;
//This has to come from STDP, how they do it. I think that all the base things a dendrite can do, they have to be done by  this abstract class.
//Other methods like allocateSynapse, or allocateBranch, can be specified by each derived class.

    //Branched specific methods
    //There is also a need for a wrapper
    virtual void SetUpSynapseSlots(int branchId) = 0; //This function will set up the open synapse slots of a branch object with its id.This one I have to define in the parallel synaptic connectivity masks
    virtual void SetUpBranches(int remainingBranchingEvents, std::vector<int> anteriorBranches);// Here we set up the vector with the branches

    int GenerateBranchId(){return branchIdGenerator++;}

    virtual bool IsBranched() override {return true;}

    //
    double generateSynapticWeight();// Here we generate the synaptic weight to be allocated when a synapse is allocated
};



#endif