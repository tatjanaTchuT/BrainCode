//
// Created by Saif Ahmed on 27.06.21.
//

#ifndef NEURALNETWORK_MONODENDRITE_H
#define NEURALNETWORK_MONODENDRITE_H

#include <iostream>
#include <unordered_set>
#include <algorithm>

#include "../Morphology.hpp"

class MonoDendriteSTDP: public Morphology {
protected:

    double tauTheta{}; // decay constant of heterosynaptic effects in spines
    double lambdaDist{}; // decay constant of heterosynaptic effects over distance between synapses
    double tauDelay{}; // decay constant of heterosynaptic effects over inter-synapse spike timing difference

    double dendriticLength{}; // this would change in case of more complex dendritic geometry (atm it is a single 1D dendrite)
    double synapticGap{}; //  minimum gap between syanpses along dendrite

    double preFactorLTP{};
    double preFactorLTD{};

    long synapseIdGenerator{}; // variable used to allocate new synapses
    double nextPos{};
    bool distributeWeights{};

    std::vector<bool> integratePostSpike{};
    std::vector<bool> integratePreSpike{};
    bool postSpiked{};

    void timeDecay();
    void updateCooperativity(unsigned long synId, unsigned long neighborId);

    virtual void updateLTP(unsigned long synId) = 0;
    virtual void updateLTD(unsigned long synId) = 0;

    virtual double gLTP(double deltaT) const = 0;
    virtual double gLTD(double deltaT) const = 0;

    virtual double aLTP(double theta) const = 0;
    virtual double aLTD(double theta) const = 0;

    virtual double getDistanceEffects(const SynapseExt* synA, const SynapseExt* synB) const = 0;
    virtual double getTimingEffects(const SynapseExt* synA, const SynapseExt* synB) const = 0;

public:

    explicit MonoDendriteSTDP(GlobalSimInfo* info);
    ~MonoDendriteSTDP() override = default;

    void advect() override;
    void recordPostSpike() override;
    void recordPreSpike(unsigned long synSpikerId) override;

    void SaveParameters(std::ofstream * stream) override;
    void LoadParameters(std::vector<std::string> *input) override;

    std::shared_ptr<SynapseExt> allocateNewSynapse()override;

    std::valarray<double> getIndividualSynapticProfile(unsigned long synapseId) const override;
    std::valarray<double> getOverallSynapticProfile() const override;

};


#endif //NEURALNETWORK_MONODENDRITE_H
