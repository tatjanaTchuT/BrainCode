#include "BranchedSynapseSpine.hpp"

// BranchedSynapseSpine::BranchedSynapseSpine(int distanceFromNode, double lastSpike, double weight, int branchId, int branchPositionId): distanceFromNode{ distanceFromNode }, branchId{ branchId }, branchPositionId {branchPositionId}, BaseSynapseSpine(weight, lastSpike)
// {
// }

std::valarray<double> BranchedSynapseSpine::GetIndividualSynapticProfile() const
{
    std::valarray<double> ret(4);
    ret[0] = this->distanceFromNode;
    ret[1] = this->branchId;
    ret[2] = this->weight;
    ret[3] = this->lastSpike;
    return ret;
}

std::string BranchedSynapseSpine::GetIndividualSynapticProfileHeaderInfo() const
{
    return std::string("{<dist to branch node>, <branch ID>, <weight>, <last spike>}");
}
