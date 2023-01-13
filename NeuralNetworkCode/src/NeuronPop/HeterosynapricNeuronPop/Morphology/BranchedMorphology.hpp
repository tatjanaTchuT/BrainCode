#ifndef _BRANCHED_MORPHOLOGY_HEADER_
#define _BRANCHED_MORPHOLOGY_HEADER_

#include "Morphology.hpp"
#include <string>
#include <numeric>
#include <algorithm>
#include <deque>
#include <iterator>

class HeteroCurrentSynapse;
class Morphology;
class BranchedMorphology : public Morphology {

protected:
//MonoDendriteSTDP moved
    unsigned long synapseIdGenerator{0}; // variable used to allocate new synapses. type is legacy compatible
    int branchIdGenerator{0}; //Same but in branches. Should be 1 or 0?

    bool distributeWeights{false};
    
    std::vector<bool> integratePostSpike;//
    std::vector<bool> integratePreSpike;//These two are created in allocateNewSynapse
    bool postSpiked{false};

    //Weight normalization vars

    int synapticGap{};
    int branchLength{};

//Branched specific
    int branchings{1};
    /* These are functionall present in derived classes
    bool orderedSynAllocation{false};// If not properly loaded from LP, exception
    bool randomSynAllocation{false};
    */

    std::vector<std::shared_ptr<Branch>> branches{};//unique_ptr's constructor is explicit, so you either need to use emplace_back or stuff.push_back(std::unique_ptr<int>(new int(i)));. Between the two, emplace_back is much cleaner.
    //initialize with the pointer, push_back() the pointer
public:
    explicit BranchedMorphology(GlobalSimInfo * info);
    virtual ~BranchedMorphology() = default;


    //Methods derived from the MonoDendriteSTDP and Morphology classes

    virtual void SaveParameters(std::ofstream * stream, std::string neuronPreId) override;//defined
    virtual void LoadParameters(std::vector<std::string> *input) override; //defined
    virtual const std::string const getType() = 0;

    

    virtual void recordPostSpike();// defined
    virtual void recordExcitatoryPreSpike(unsigned long synSpikerId); //defined
    virtual std::valarray<double> getIndividualSynapticProfile(unsigned long synapseId) const override; //defined in the SynapseExt structs
    virtual std::valarray<double> getOverallSynapticProfile() const;//defined
    virtual void advect()=0;
    //This has to come from STDP, how they do it. I think that all the base things a dendrite can do, they have to be done by  this abstract class.
    //Other methods like allocateSynapse, or allocateBranch, can be specified by each derived class.

    //Branched specific methods
    //There is also a need for a wrapper called at the end of LP
    virtual void setUpSynapseSlots(std::shared_ptr<Branch> branch) = 0; //This function will set up the open synapse slots of a branch object with its id.This one I have to define in the parallel synaptic connectivity masks or the derived classes
    //setUp SYnapse slots is called for every branch in a loop and depending on the bool (universal for all branches for now) it calls random or ordered.
    //The overriding function calls functions of BMorpho. 
    virtual void setUpBranchings(int remainingBranchingEvents, std::vector<int> anteriorBranches = std::vector<int>());// Here we set up the vector with the branches

    //Allocation shennanigans
    double generateSynapticWeight();// Here we generate the synaptic weight to be allocated when a synapse is allocated
    virtual std::shared_ptr<SynapseExt> allocateNewSynapse(HeteroCurrentSynapse& synapse) override =0; //Use the reference to call getBranchTarget
    //VERY IMPORTANT that the SynapseExt pointer poiints to a SynapseExtBranched (in derived classes) and here I need access to the subregion of the incoming synapse
    virtual int allocateBranch()=0;//The selected branch allocation is simple. Subregion will be implemented in the future
    virtual int randomBranchAllocation();
    virtual int orderedBranchAllocation();
    //setBranchAllocation is implicit in the function (or has to be) allocate NewSynapse
    //virtual int orderedGuidedBranchAllocation(const char subregionID);
    virtual void RandomSynapseAllocation(std::shared_ptr<Branch> branch);
    virtual void OrderedSynapseAllocation(std::shared_ptr<Branch> branch);//These two are coming from the setUpSynapseSlots already, called depending on a bool. 
    //virtual void AlternatedSynapseAllocation(std::shared_ptr<Branch> branch);
    //
    virtual bool const isBranchedBool() override {return true;}
    int generateBranchId(){return branchIdGenerator++;}

};


#endif