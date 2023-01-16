#include "SynapseSpine.hpp"

SynapseSpine::SynapseSpine(double distToSoma, double lastSpike, double weight): distToSoma{distToSoma}, lastSpike{lastSpike}, weight{weight}
{
}

SynapseSpine::SynapseSpine(int distanceFromNode, double lastSpike, double weight, int branchId, int branchPositionId): lastSpike{lastSpike}, weight{weight} , distanceFromNode{ distanceFromNode }, branchId{ branchId }, branchPositionId {branchPositionId}
{
}

std::valarray<double> SynapseSpine::getIndividualSynapticProfileLegacy() const
{
    std::valarray<double> ret(4);
    ret[0] = this->distToSoma;
    ret[1] = this->theta;
    ret[2] = this->weight;
    ret[3] = this->lastSpike;
    return ret;
}

std::valarray<double> SynapseSpine::getIndividualSynapticProfileBranched() const
{
    std::valarray<double> ret(3);
    ret[0] = this->distanceFromNode;
    ret[1] = this->weight;
    ret[2] = this->lastSpike;
    return ret;
}

std::valarray<double> SynapseSpine::getIndividualSynapticProfile() const
{
    if (IsBranched){
        return getIndividualSynapticProfileBranched();
    }
    else{
        return getIndividualSynapticProfileLegacy();
    }
}