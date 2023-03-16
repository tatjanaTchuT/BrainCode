#include "SynapseSpineBase.hpp"


SynapseSpineBase::SynapseSpineBase(double distToSoma, double lastSpike, double weight): distToSoma{distToSoma}, lastSpike{lastSpike}, weight{weight}
{
}

SynapseSpineBase::SynapseSpineBase(int distanceFromNode, double lastSpike, double weight, int branchId, int branchPositionId): lastSpike{lastSpike}, weight{weight} , distanceFromNode{ distanceFromNode }, branchId{ branchId }, branchPositionId {branchPositionId}
{
}

std::valarray<double> SynapseSpineBase::getIndividualSynapticProfileLegacy() const
{
    std::valarray<double> ret(4);
    ret[0] = this->distToSoma;
    ret[1] = this->theta;
    ret[2] = this->weight;
    ret[3] = this->lastSpike;
    return ret;
}

std::valarray<double> SynapseSpineBase::getIndividualSynapticProfileBranched() const
{
    std::valarray<double> ret(4);
    ret[0] = this->distanceFromNode;
    ret[1] = 1;//Here go resources
    ret[2] = this->weight;
    ret[3] = this->lastSpike;
    return ret;
}

std::valarray<double> SynapseSpineBase::getIndividualSynapticProfile() const
{
    if (this->getBranchedBool()){
        return getIndividualSynapticProfileBranched();
    }
    else{
        return getIndividualSynapticProfileLegacy();
    }
}