#include "./BranchedStructs.hpp"
#include "BranchedStructs.hpp"

DendriticSubRegion::DendriticSubRegion(char regionID, std::vector<int> branchesInRegion): regionID{regionID}, branchesInRegion{branchesInRegion}
{

}

Branch::Branch(double gap, double branchLength, std::vector<int> anteriorBranches, int branchId):branchSlots{static_cast<size_t>(std::round(branchLength/gap))}, synapticGap{gap}, branchLength{branchLength}, anteriorBranches{anteriorBranches}, branchId{branchId}//,morphoSynapseIDs(static_cast<size_t>(branchSlots), -1),branchSynapseIDs(static_cast<size_t>(branchSlots), -1), , spikedSyn(branchSlots, false)
{
    //std::iota(uniqueSynapsePositionIDs.begin(),uniqueSynapsePositionIDs.end() , branchId*(branchSlots));
}

void Branch::postConnectSetUp(std::vector<BranchedSpinePtr> SynapseData)
{
    //In case it is necessary in the future
}

ResourceBranch::ResourceBranch(double gap, double branchLength, std::vector<int> anteriorBranches, int branchId, int triggerMaxCount): 
Branch(gap, branchLength, anteriorBranches, branchId), triggerCount(branchSlots, triggerMaxCount)//, potentiationCountSTDP(branchSlots, 10)//, maxCountSTDPPotentiation{branchMaxCountSTDPPotentiation}//, maxCountTrigger{branchMaxCountTrigger}//, plasticityEventsPerTimestepWindow(betaEventsWindowSize)
{
}
void ResourceBranch::postConnectSetUp(std::vector<BranchedSpinePtr> synapseData)
{
    SetUpSynapseData(synapseData);
}
void ResourceBranch::SetUpSynapseData(std::vector<BranchedSpinePtr> synapseData)
{
    //This function sets up the spine pointers in the branch, something not possible until ConnectNeurons() runs, thus its execution from PostConnectSetUp()
    for (BranchedSpinePtr synapse :synapseData){
        if (synapse->GetBranchId()==branchId){
            branchSynapseData.push_back(std::dynamic_pointer_cast<ResourceSynapseSpine>(synapse));
        }
    }
    std::sort(branchSynapseData.begin(), branchSynapseData.end(), BranchIDCompare); //This allows to do indexing of the synapse data using the branch ID
}

void ResourceBranch::TickAllCounts()
{
    //This function is supposed to increase the counts of all synapses
    for (int& count:this->triggerCount){
    //if (count<maxCount){
        count++;
    //}
    }
}

// void ResourceBranch::CheckIncreaseInBetaResources()
// {
//     //Assuming the plasticityEventsInThisTimestep is tracked properly:
//     plasticityEventsPerTimestepWindow.pop_front();
//     plasticityEventsPerTimestepWindow.push_back(plasticityBranchEventsThisTimestep);
//     if (plasticityBranchEventsThisTimestep>0){//This requirement is a logically coherent efficiency, as potentiation should not happen if there are no plasticity events.
//         plasticityBranchEventsTotal+=plasticityBranchEventsThisTimestep;
//         sumPlasticityEvents =std::accumulate(this->plasticityEventsPerTimestepWindow.begin(), this->plasticityEventsPerTimestepWindow.end(), 0);
//         if (sumPlasticityEvents>=plasticityEventsPerTimestepThreshold){
//             betaResourcePool += betaUpTick;
//             //here, do we reset the deque, or do we set a refractory period?
//         }
//         plasticityBranchEventsThisTimestep=0;
//     }
// }