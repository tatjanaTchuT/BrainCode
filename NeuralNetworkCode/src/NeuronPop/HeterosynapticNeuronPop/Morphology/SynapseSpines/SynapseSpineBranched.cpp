#include "SynapseSpineBranched.hpp"

// SynapseSpineBranched::SynapseSpineBranched(int distanceFromNode, double lastSpike, double weight, int branchId, int branchPositionId): distanceFromNode{ distanceFromNode }, branchId{ branchId }, branchPositionId {branchPositionId}, SynapseSpineBase(weight, lastSpike)
// {
// }

std::valarray<double> SynapseSpineBranched::getIndividualSynapticProfile() const
{
    std::valarray<double> ret(4);
    ret[0] = this->distanceFromNode;
    ret[1] = this->branchId;
    ret[2] = this->weight;
    ret[3] = this->lastSpike;
    return ret;
}

std::string SynapseSpineBranched::getIndividualSynapticProfileHeaderInfo() const
{
    return std::string("Profile -> {<dist to branch node>, <branch ID>, <weight>, <last spike>} \n");
}
