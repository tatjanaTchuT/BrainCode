#include "BranchedStructs.hpp"

Branch::Branch(int gap, int branchLength, std::vector<int> anteriorBranches, int branchId):spikedSyn(static_cast<size_t>(branchLength/gap), false),morphoSynapseIDs(static_cast<size_t>(branchLength/gap), -1), synapticGap{gap}, branchLength{branchLength}, anteriorBranches{anteriorBranches}, branchId{branchId}//,branchSynapseIDs(static_cast<size_t>(branchLength/gap), -1)
{
    //std::iota(uniqueSynapsePositionIDs.begin(),uniqueSynapsePositionIDs.end() , branchId*(branchLength/gap));
}

SubRegion::SubRegion(char regionID, std::vector<int> branchesInRegion): regionID{regionID}, branchesInRegion{branchesInRegion}
{

}