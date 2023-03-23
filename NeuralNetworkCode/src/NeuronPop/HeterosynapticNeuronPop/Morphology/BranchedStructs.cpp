#include "./BranchedStructs.hpp"

Branch::Branch(int gap, int branchLength, std::vector<int> anteriorBranches, int branchId):spikedSyn(static_cast<size_t>(branchLength/gap), false),morphoSynapseIDs(static_cast<size_t>(branchLength/gap), -1), synapticGap{gap}, branchLength{branchLength}, anteriorBranches{anteriorBranches}, branchId{branchId}//,branchSynapseIDs(static_cast<size_t>(branchLength/gap), -1)
{
    //std::iota(uniqueSynapsePositionIDs.begin(),uniqueSynapsePositionIDs.end() , branchId*(branchLength/gap));
}

SubRegion::SubRegion(char regionID, std::vector<int> branchesInRegion): regionID{regionID}, branchesInRegion{branchesInRegion}
{

}

ResourceBranch::ResourceBranch(int gap, int branchLength, std::vector<int> anteriorBranches, int branchId, size_t timestepWindowSize, int maxCount): Branch(gap, branchLength, anteriorBranches, branchId), triggerCount(static_cast<size_t>(branchLength/gap), 10), STDPPotentiationCount(static_cast<size_t>(branchLength/gap), 10), plasticityEventsPerTimestepWindow(timestepWindowSize), maxCount{maxCount}
{
}

void ResourceBranch::TickAllCounts()
{
    TickCounts(this->STDPPotentiationCount);
    TickCounts(this->triggerCount);
}

void ResourceBranch::TickCounts(std::vector<int> &countVector)
{
    for (int& count:countVector){
        if (count<maxCount){
            count++;
        }
    }
}

void ResourceBranch::CheckIncreaseInBetaResources()
{
    //Assuming the plasticityEventsInThisTimestep is tracked properly:
    plasticityEventsPerTimestepWindow.pop_front();
    plasticityEventsPerTimestepWindow.push_back(plasticityBranchEventsThisTimestep);
    if (plasticityBranchEventsThisTimestep>0){//This requirement is a logically coherent efficiency, as potentiation should not happen if there are no plasticity events.
        plasticityBranchEventsTotal+=plasticityBranchEventsThisTimestep;
        sumPlasticityEvents =std::accumulate(this->plasticityEventsPerTimestepWindow.begin(), this->plasticityEventsPerTimestepWindow.end(), 0);
        if (sumPlasticityEvents>=plasticityEventsPerTimestepThreshold){
            betaResourcePool += betaUpTick;
            //here, do we reset the deque, or do we set a refractory period?
        }
        plasticityBranchEventsThisTimestep=0;
    }
}