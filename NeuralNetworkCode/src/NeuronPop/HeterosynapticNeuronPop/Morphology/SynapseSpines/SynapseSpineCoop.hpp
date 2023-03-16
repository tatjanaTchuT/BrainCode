#ifndef _SYNAPSE_SPINE_COOP_CLASS_HEADER_
#define _SYNAPSE_SPINE_COOP_CLASS_HEADER_

#include "SynapseSpineBase.hpp"

class SynapseSpineCoop : public SynapseSpineBase {
    double distToSoma{};
    double theta{}; // heterosynaptic cooperativity

    public:
    SynapseSpineCoop(double distToSoma, double lastSpike, double weight);

    //getters
    double getDistToSoma() const {return distToSoma;};
    double getTheta() const {return theta;};

    //setters
    void setDistToSoma(double dist){distToSoma=dist;};
    void setTheta(double thetaIn){theta=thetaIn;};

    //Misc
    void addToTheta(double hEffect){theta+=hEffect;}

    //Profile function
    std::valarray<double> getIndividualSynapticProfile() const override;
    std::string getIndividualSynapticProfileHeaderInfo() const override;
};


#endif