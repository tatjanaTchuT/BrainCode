#ifndef _BRANCHED_STRUCTS_HEADER_
#define _BRANCHED_STRUCTS_HEADER_

#include "./SynapseSpines/ResourceSynapseSpine.hpp"
#include <vector>
#include <deque>
#include <set>
#include <numeric>
#include <memory>

typedef std::unordered_map<int, int> IDHashMap;

struct SubRegion{
    const char regionID;
    const std::vector<int> branchesInRegion; //I will have to read this from the morphology LP, every subregion is a line, first input is ID, rest is branchIDs. Then in Synapse you put the subregion where the synapse goes. 
    SubRegion(char regionID, std::vector<int> branchesInRegion);
};

struct BranchTargeting{
    int targetBranch{};
    bool setTargetBranch{false};
    bool randomTargetBranch{false};
    bool orderedTargetBranch{false};
    char subRegion{'0'};
};

struct Branch{
    //ID
    const int branchId{};
    //Branched vars
    const std::vector<int> anteriorBranches{}; 

    const int synapticGap{};    //For now these are identical to the morphology ones, but we will see in the future
    const int branchLength{};

    size_t branchSlots{};

    std::deque<int> openSynapsesSlots{};
    //For the actual checks
    std::vector<bool> spikedSyn{};
    std::vector<int> synapseSlotClosedIndex{};
    //std::vector<int> morphoSynapseIDs{};//This data variable is no longer relevant
    IDHashMap synapseSlotToMorphoIDMap;
    //Resource paradigm variables
    int plasticityBranchEventsTotal{};
    //Methods
    //Branch()=default;
    Branch(int gap, int branchLength, std::vector<int>anteriorBranches, int branchId);

    virtual void IncreasePlasticityCounter(){plasticityBranchEventsTotal++;}
};

struct ResourceBranch : public Branch {

    //Synapse access
    std::vector<std::shared_ptr<ResourceSynapseSpine>> branchSynapseData;//CAREFUL! THIS VECTOR IS NOT SORTED WITH BRANCH IDS, BUT MORPHO IDS

    std::set<int> updatedSynapseSpines{};//IDs are branch slots, to index the vector above to index proper. Cleared every timestep, used for depression only
    std::set<int> updatedAlphaEffects{};//IDs are branch slots, Used in depression, cleared every timestep
    // int plasticityEventsPerTimestepThreshold{};
    int sumPlasticityEvents{};//Theoretically temporary

    //double betaResourcePool{1.0}; //We use the one in the Morpho object
    // double betaUpTick{0.05};
    // /double omegaPassiveResourceOffset{1.0}; //We use the one in the Morpho object
    double alphaTotalSum{};
    double weightResourceFactor{};
    //Counts (all of these are not general, which is kind of an issue for generality)
    int maxCountSTDPPotentiation{};
    int maxCountTrigger{};
    std::vector<int> triggerCount{};//Here is where we look for counts under 10
    std::vector<int> STDPPotentiationCount{};//Size of these has to be the amount of synapses in the branch (equal to triggerCount, length divided by gaps)
    //Misc
    // int plasticityBranchEventsThisTimestep{};//Has to be set to zero
    // std::deque<int> plasticityEventsPerTimestepWindow{};//Every timestep I push_front the events in the timestep and delete the last element with pop or erase. Then sum and check if over the threshold.
        //Do we clear() after a trigger or not? Most of the function would be the same, or put some refractory period UNRESOLVED
        //Here we could create a false history of plasticity events
    //Methods
    ResourceBranch(int gap, int branchLength, std::vector<int>anteriorBranches, int branchId, int branchMaxCountSTDPPotentiation, int branchMaxmaxCountTrigger, std::vector<std::shared_ptr<ResourceSynapseSpine>> branchSynapseData);
    void SetUpSynapseData(std::vector<std::shared_ptr<ResourceSynapseSpine>> branchSynapseData);
        //Count related functions
    void TickAllCounts();//Use ternary operator. Called in Reset()
    void TickCounts(std::vector<int>& countVector, int maxCount);
    //void CheckIncreaseInBetaResources(); //Here I have to add current, delete last, sum and check against threshold. Called in Reset()
    //void UpdateAlphaSum(); //This dunction cannot be implemented in this struct as the struct has no access to the spine pointers
    //void IncreasePlasticityCounter() override {plasticityBranchEventsThisTimestep++;plasticityBranchEventsTotal++;}
};
#endif