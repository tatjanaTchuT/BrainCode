#include "CoopSynapseSpine.hpp"

// CoopSynapseSpine::CoopSynapseSpine(double distToSoma, double lastSpike, double weight):distToSoma{distToSoma}, BaseSynapseSpine(weight,lastSpike){
// }

std::valarray<double> CoopSynapseSpine::GetIndividualSynapticProfile() const
{
    std::valarray<double> ret(4);
    ret[0] = this->distToSoma;
    ret[1] = this->theta;
    ret[2] = this->weight;
    ret[3] = this->lastSpike;
    return ret;
}

std::string CoopSynapseSpine::GetIndividualSynapticProfileHeaderInfo() const
{
    return std::string("{<dist to soma>, <hetero cooperativity>, <weight>, <last spike>}");
}
