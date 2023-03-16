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
    double getDistToSoma() const override {return distToSoma;};
    double getTheta() const override {return theta;};

    //setters
    void setDistToSoma(double dist) override {distToSoma=dist;};
    void setTheta(double thetaIn) override {theta=thetaIn;};

    //Misc
    void addToTheta(double hEffect) override {theta+=hEffect;}

    //Profile function
    std::valarray<double> getIndividualSynapticProfile() const override;
    std::string getIndividualSynapticProfileHeaderInfo() const override;
};


#endif