#ifndef _SYNAPSE_SPINE_COOP_CLASS_HEADER_
#define _SYNAPSE_SPINE_COOP_CLASS_HEADER_

#include "BaseSynapseSpine.hpp"

class CoopSynapseSpine : public BaseSynapseSpine {

    protected:
    double distToSoma{};
    double theta{}; // heterosynaptic cooperativity

    public:
    CoopSynapseSpine()=default;
    //CoopSynapseSpine(double distToSoma, double lastSpike, double weight);

    //getters
    double GetDistToSoma() const {return distToSoma;};
    double GetTheta() const {return theta;};

    //setters
    void SetDistToSoma(double dist) {distToSoma=dist;};
    void SetTheta(double thetaIn) {theta=thetaIn;};

    //Misc
    void AddToTheta(double hEffect) {theta+=hEffect;}

    //Profile function
    std::valarray<double> GetIndividualSynapticProfile() const override;
    std::string GetIndividualSynapticProfileHeaderInfo() const override;
};


#endif