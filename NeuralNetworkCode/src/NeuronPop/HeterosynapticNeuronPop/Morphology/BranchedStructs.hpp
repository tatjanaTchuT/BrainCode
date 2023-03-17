#ifndef _BRANCHED_STRUCTS_HEADER_
#define _BRANCHED_STRUCTS_HEADER_

#include <vector>
#include <deque>

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
    //For setup
    const std::vector<int> anteriorBranches{}; 

    //For now these are identical to the morphology ones, but we will see in the future
    const int synapticGap{};
    const int branchLength{};

    std::deque<int> openSynapsesSlots{};//We will pop_back() from here the position of the synapse being setup. Will push_back() in setup, so no default size
    //The idea is to .pop_front() ids that have been ordered inside here
    //For the actual checks
    std::vector<bool> spikedSyn{};//Here, with the size of the branch discrete positions, we will store the bool indicating if the preneuron fired in the timestep
    //Because of how the kernelized version of plasticity works, I will  use the branch ID and relative position ID from the SynapseExtBranched, so the next two vectors are unnecessary
    std::vector<int> synapseSlotClosedIndex{}; // Here we store the used indexes in the vector, to check for stuff faster and only go to indexes
    std::vector<int> morphoSynapseIDs{};// REMEMBER TO PUT IN CONSTRUCTOR Unique ID for the poisition in the dendritic tree. Probably going to be for allocation only
    //Maybe more vectors will be needed. Not in use commented out. 
    //Branch()=default;
    Branch(int gap, int branchLength, std::vector<int>anteriorBranches, int branchId);
};

struct ResourceBranch : public Branch {
    
};
#endif