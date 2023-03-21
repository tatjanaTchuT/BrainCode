#ifndef _BRANCHED_STRUCTS_HEADER_
#define _BRANCHED_STRUCTS_HEADER_

#include <vector>
#include <deque>
#include <set>

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

    std::deque<int> openSynapsesSlots{};
    //For the actual checks
    std::vector<bool> spikedSyn{};
    std::vector<int> synapseSlotClosedIndex{};
    std::vector<int> morphoSynapseIDs{};// REMEMBER TO PUT IN CONSTRUCTOR Unique ID for the poisition in the dendritic tree. Probably going to be for allocation only
    //Resource paradigm variables
    unsigned int plasticityBranchEvents{};

    //Methods
    //Branch()=default;
    Branch(int gap, int branchLength, std::vector<int>anteriorBranches, int branchId);


};

struct ResourceBranch : public Branch {

    std::set<int> updatedSynapseSpines{};//IDs are branch slots, to index the vector above to index proper. Cleared every timestep, used for depression only

    unsigned int timestepWindowSize{};
    unsigned int plasticityEventsPerTimestepThreshold{};

    double betaResourcePool{1.0};
    double betaUpTick{0.05};
    double omegaPassiveResourceOffset{1.0};
    double alphaTotalSum{};
    //Counts (all of these are not general, which is kind of an issue for generality)
    std::vector<int> triggerCount{};//Here is where we look for counts under 10
    std::vector<int> STDPPotentiationCount{};//Size of these has to be the amount of synapses in the branch (equal to )
    // std::vector<int> triggerCount{};
    // std::vector<int> triggerCount{};
    //Misc
    std::deque<int> plasticityEventsPerTimestepWindow{};//Every timestep I push_front the events in the timestep and delete the last element with pop or erase. Then sum and check if over the threshold.
        //Do we clear() after a trigger or not? Most of the function would be the same
        //Here we could create a false history of plasticity events
    //Methods
    ResourceBranch(int gap, int branchLength, std::vector<int>anteriorBranches, int branchId);

        //Count related functions
    void TickAllCounts();//Use ternary operator
    void CheckIncreaseInBetaResources(); //Here I have to add current, delete last, sum and check against threshold
    void UpdateAlphaSum();
};
#endif