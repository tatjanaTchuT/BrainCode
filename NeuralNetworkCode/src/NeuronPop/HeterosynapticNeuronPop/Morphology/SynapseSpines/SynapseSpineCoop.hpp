#ifndef _SYNAPSE_SPINE_COOP_CLASS_HEADER_
#define _SYNAPSE_SPINE_COOP_CLASS_HEADER_

#include "SynapseSpineBase.hpp"

class SynapseSpineCoop : public SynapseSpineBase {

    protected:
    double distToSoma{};
    double theta{}; // heterosynaptic cooperativity

    public:
    SynapseSpineCoop()=default;
    //SynapseSpineCoop(double distToSoma, double lastSpike, double weight);

    //getters
    double GetDistToSoma() const override {return distToSoma;};
    double GetTheta() const override {return theta;};

    //setters
    void SetDistToSoma(double dist) override {distToSoma=dist;};
    void SetTheta(double thetaIn) override {theta=thetaIn;};

    //Misc
    void AddToTheta(double hEffect) override {theta+=hEffect;}

    //Profile function
    std::valarray<double> GetIndividualSynapticProfile() const override;
    std::string GetIndividualSynapticProfileHeaderInfo() const override;
};


#endif