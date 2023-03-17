#include "SynapseSpineCoop.hpp"

// SynapseSpineCoop::SynapseSpineCoop(double distToSoma, double lastSpike, double weight):distToSoma{distToSoma}, SynapseSpineBase(weight,lastSpike){
// }

std::valarray<double> SynapseSpineCoop::GetIndividualSynapticProfile() const
{
    std::valarray<double> ret(4);
    ret[0] = this->distToSoma;
    ret[1] = this->theta;
    ret[2] = this->weight;
    ret[3] = this->lastSpike;
    return ret;
}

std::string SynapseSpineCoop::GetIndividualSynapticProfileHeaderInfo() const
{
    return std::string("{<dist to soma>, <hetero cooperativity>, <weight>, <last spike>}");
}
