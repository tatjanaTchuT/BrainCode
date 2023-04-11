#include "./BranchedStructs.hpp"
#include "BranchedStructs.hpp"

Branch::Branch(double gap, double branchLength, std::vector<int> anteriorBranches, int branchId):branchSlots{static_cast<size_t>(std::round(branchLength/gap))}, synapticGap{gap}, branchLength{branchLength}, anteriorBranches{anteriorBranches}, branchId{branchId}//,morphoSynapseIDs(static_cast<size_t>(branchSlots), -1),branchSynapseIDs(static_cast<size_t>(branchSlots), -1), , spikedSyn(branchSlots, false)
{
    //std::iota(uniqueSynapsePositionIDs.begin(),uniqueSynapsePositionIDs.end() , branchId*(branchSlots));
}

DendriticSubRegion::DendriticSubRegion(char regionID, std::vector<int> branchesInRegion): regionID{regionID}, branchesInRegion{branchesInRegion}
{

}

ResourceBranch::ResourceBranch(double gap, double branchLength, std::vector<int> anteriorBranches, int branchId, std::vector<std::shared_ptr<ResourceSynapseSpine>> branchSynapseData): 
Branch(gap, branchLength, anteriorBranches, branchId), triggerCount(branchSlots, 10)//, potentiationCountSTDP(branchSlots, 10)//, maxCountSTDPPotentiation{branchMaxCountSTDPPotentiation}//, maxCountTrigger{branchMaxCountTrigger}//, plasticityEventsPerTimestepWindow(betaEventsWindowSize)
{
    SetUpSynapseData(branchSynapseData);
}
void ResourceBranch::SetUpSynapseData(std::vector<std::shared_ptr<ResourceSynapseSpine>> branchSynapseData)
{
    for (std::shared_ptr<ResourceSynapseSpine> synapse :branchSynapseData){
        if (synapse->GetBranchId()==branchId){
            branchSynapseData.push_back(synapse);
        }
    }
}

void ResourceBranch::TickAllCounts()
{
    //TickCounts(this->potentiationCountSTDP);
        for (int& count:this->triggerCount){
        //if (count<maxCount){
            count++;
        //}
    }
}

// void ResourceBranch::TickCounts(std::vector<int> &countVector)
// {
//     for (int& count:countVector){
//         //if (count<maxCount){
//             count++;
//         //}
//     }
// }

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