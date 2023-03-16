#ifndef _BRANCHED_MORPHOLOGY_HEADER_
#define _BRANCHED_MORPHOLOGY_HEADER_

#include "../../../Synapse/HeteroCurrentSynapse.hpp" //This include also causes an include loop. Careful
#include <string>
#include <numeric>
#include <algorithm>
#include <deque>
#include <iterator>
#include <random>
#include "../../../GlobalFunctions.hpp"
#include "./Morphology.hpp"
#include "./SynapseSpines/SynapseSpineBase.hpp"
#include "./SynapseSpines/SynapseSpineBranched.hpp"

class BranchedMorphology : public Morphology {

protected:
//MonoDendriteSTDP moved
    unsigned long synapseIdGenerator{0}; // variable used to allocate new synapses. type is legacy compatible
    int branchIdGenerator{0}; //Same but in branches. Should be 1 or 0?
    //std::vector<bool> integratePostSpike;// Not necessary
    //std::vector<bool> integratePreSpike;//Not necessary

    //Weight normalization vars
    int synapticGap{};
    int branchLength{};

//Branched specific
    int branchings{1};

    bool orderedSynAllocationB{false};// If not properly loaded from LP, exception
    bool randomSynAllocationB{false};

    /*bool setBranchAllocationB{false};
    bool orderedBranchAllocationB{false};// If not properly loaded from LP, exception
    bool randomBranchAllocationB{false};*/

    std::vector<std::shared_ptr<Branch>> branches{};//unique_ptr's constructor is explicit, so you either need to use emplace_back or stuff.push_back(std::unique_ptr<int>(new int(i)));. Between the two, emplace_back is much cleaner.
    //initialize with the pointer, push_back() the pointer
public:
    explicit BranchedMorphology(GlobalSimInfo * info);
    virtual ~BranchedMorphology() = default;

    //Methods derived from the MonoDendriteSTDP and Morphology classes
    virtual void SaveParameters(std::ofstream * stream, std::string neuronPreId) override;
    virtual void LoadParameters(std::vector<std::string> *input) override;
    virtual const std::string getType() = 0;

    virtual void recordPostSpike();//REDO
    virtual void recordExcitatoryPreSpike(unsigned long synSpikerId); //REDO
    virtual std::valarray<double> getIndividualSynapticProfile(unsigned long synapseId) const override;
    virtual void advect()=0;
    void reset() override;
    //This has to come from STDP, how they do it. I think that all the base things a dendrite can do, they have to be done by  this abstract class.
    //Other methods like allocateSynapse, or allocateBranch, can be specified by each derived class.

    //Branched specific methods
    //There is also a need for a wrapper called at the end of LP
    void setUpMorphology();
    virtual void setUpSynapseSlots(std::shared_ptr<Branch> branch); //This function will set up the open synapse slots of a branch object with its id.This one I have to define in the parallel synaptic connectivity masks or the derived classes
    //setUp SYnapse slots is called for every branch in a loop and depending on the bool (universal for all branches for now) it calls random or ordered.
    //The overriding function calls functions of BMorpho. 
    virtual void setUpBranchings(int remainingBranchingEvents, std::vector<int> anteriorBranches = std::vector<int>());// Here we set up the vector with the branches

    //Allocation shennanigans
    
    virtual std::shared_ptr<SynapseSpineBase> allocateNewSynapse(HeteroCurrentSynapse& synapse) override; //Use the reference to call getBranchTarget

    virtual int allocateBranch(const HeteroCurrentSynapse &synapse);//The selected branch allocation is simple. This function is called in allocateNewSynapse
    virtual int randomBranchAllocation();
    virtual int orderedBranchAllocation();
    //setBranchAllocation() is implicit in the function (or has to be) allocate NewSynapse
    //virtual int orderedGuidedBranchAllocation(const char subregionID);
    virtual void randomSynapseAllocation(std::shared_ptr<Branch> branch);
    virtual void orderedSynapseAllocation(std::shared_ptr<Branch> branch);//These two are coming from the setUpSynapseSlots already, called depending on a bool. 
    //virtual void AlternatedSynapseAllocation(std::shared_ptr<Branch> branch);
    //
    virtual bool const isBranchedBool() override {return true;}
    int generateBranchId(){return branchIdGenerator++;}

};


#endif