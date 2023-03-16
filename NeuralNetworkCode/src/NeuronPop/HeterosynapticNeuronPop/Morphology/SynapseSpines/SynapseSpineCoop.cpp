#include "SynapseSpineCoop.hpp"

// SynapseSpineCoop::SynapseSpineCoop(double distToSoma, double lastSpike, double weight):distToSoma{distToSoma}, SynapseSpineBase(weight,lastSpike){
// }

std::valarray<double> SynapseSpineCoop::getIndividualSynapticProfile() const
{
    std::valarray<double> ret(4);
    ret[0] = this->distToSoma;
    ret[1] = this->theta;
    ret[2] = this->weight;
    ret[3] = this->lastSpike;
    return ret;
}

std::string SynapseSpineCoop::getIndividualSynapticProfileHeaderInfo() const
{
    return std::string("Profile -> {<dist to soma>, <hetero cooperativity>, <weight>, <last spike>} \n");
}
