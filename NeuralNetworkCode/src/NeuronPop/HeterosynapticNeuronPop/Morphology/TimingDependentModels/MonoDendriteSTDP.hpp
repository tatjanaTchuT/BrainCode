//
// Created by Saif Ahmed on 27.06.21.
//

#ifndef NEURALNETWORK_MONODENDRITE_H
#define NEURALNETWORK_MONODENDRITE_H

#include <iostream>
#include <unordered_set>
#include <algorithm>

#include "../Morphology.hpp"

class HeteroCurrentSynapse;

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
    bool stepWeights{};
    std::vector<unsigned long> weightStepBoundary{};
    std::vector<double> weightStepValue{};
    unsigned long currWightStepId{};

    std::vector<bool> integratePostSpike{};
    std::vector<bool> integratePreSpike{};
    bool postSpiked{};

    double initialWeights{1.0};

    double base_ltp{};
    double base_ltd{};

    double incr_ltp{};
    double decr_ltd{};

    void thetaDecay();
    void updateCooperativity(unsigned long synId, unsigned long neighborId);
    void pseudoCoop(unsigned long synId, unsigned long neighborId);

    virtual void updateLTP(unsigned long synId) = 0;
    virtual void updateLTD(unsigned long synId) = 0;

    virtual double gLTP(double deltaT) const = 0;
    virtual double gLTD(double deltaT) const = 0;

    virtual double aLTP(double theta) const = 0;
    virtual double aLTD(double theta) const = 0;

    virtual double getDistanceEffects(const SynapseSpine* synA, const SynapseSpine* synB) const = 0;
    virtual double getTimingEffects(const SynapseSpine* synA, const SynapseSpine* synB) const = 0;

public:
    explicit MonoDendriteSTDP(GlobalSimInfo* info);
    ~MonoDendriteSTDP() override = default;

    void advect() override;
    void recordPostSpike() override;
    void recordExcitatoryPreSpike(unsigned long synSpikerId) override;

    void SaveParameters(std::ofstream * stream, std::string neuronPreId) override;
    void LoadParameters(std::vector<std::string> *input) override;

    virtual std::shared_ptr<SynapseSpine> allocateNewSynapse(HeteroCurrentSynapse& synapse) override;
    
    std::valarray<double> getIndividualSynapticProfile(unsigned long synapseId) const override;
    std::valarray<double> getOverallSynapticProfile() const override;

};


#endif //NEURALNETWORK_MONODENDRITE_H
