#ifndef _BRANCHED_MORPHOLOGY_HEADER_
#define _BRANCHED_MORPHOLOGY_HEADER_

#include "Morphology.hpp"

struct Branch{
    //ID
    int branch_id;
    //For setup
    std::vector<int> anterior_branches;

    int synaptic_gap;
    int branch_length;

    std::vector<int> openSynapsesSlots;//We will pop_back() from here the position of the synapse being setup
    //For the actual checks
    std::vector<bool> spikedSyn;//Here, with the size of the branch open positions, we will store the bool indicating if the preneuron fired in the timestep
    std::vector<int> branchSynapsesID; // Here we store the actual positions of our synapses. Non-occupied will be -1? Or 0 if the openSynapsesPositions starts from 1.

    //Formed in constructor. The idea is to .pop_back() ids that have been ordered inside here
    Branch(size_t synSlots, int gap, int branch_length);
};

class BranchedMorphology : public Morphology {

protected:
    int ramifications;
    int totalNoBranches;

    int branch_id_generator;

    std::vector<std::shared_ptr<Branch>> branches;

public:
    explicit BranchedMorphology(GlobalSimInfo * info);
    virtual ~BranchedMorphology() = default; //what does default mean here?

    virtual void SaveParameters(std::ofstream * stream, std::string neuronPreId) override;//define
    virtual void LoadParameters(std::vector<std::string> *input) override; //define
    virtual std::string getType() = 0;

    virtual std::shared_ptr<SynapseExt> allocateNewSynapse() = 0;

    virtual void recordPostSpike();// define
    virtual void recordExcitatoryPreSpike(unsigned long synSpikerId); //define
    virtual std::valarray<double> getIndividualSynapticProfile(unsigned long synapseId) const = 0; //define?
    virtual std::valarray<double> getOverallSynapticProfile() const = 0;//define?
//This has to come from STDP, how they do it. I think that all the base things a dendrite can do, they have to be done by  this abstract class.
//Other methods like allocateSynapse, or allocateBranch, can be specified by each derived class.

    //Branched specific methods
    virtual void SetUpSynapseSlots(int branch_id) = 0; //This function will set up the open synapse slots of a branch object with its id.
    virtual void SetUpBranch (int branch_id);//This one I have to define

    virtual bool IsBranched() override {return true;}
};



#endif