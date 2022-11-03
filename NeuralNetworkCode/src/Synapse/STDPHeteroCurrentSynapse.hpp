//
// Created by Saif Ahmed on 22.09.21.
//

#ifndef NEURALNETWORK_TAZERARTHETEROSYNAPSE_H
#define NEURALNETWORK_TAZERARTHETEROSYNAPSE_H

#include "HeteroSynapse.hpp"
#include "STDP/Kernel.hpp"

enum WeightNormalization {
    NOPNormalization, HardNormalization
};

class STDPHeteroCurrentSynapse: public HeteroSynapse {

protected:
    // Extension members to enable Heterosynaptic effects
    double preFactorPotentiation{};
    double preFactorDepression{};
    double alphaPotentiation{};
    double betaDepression{};
    double lambdaDist{}; // decay constant of heterosynaptic effects over distance between synapses
    double tauDelay{}; // decay constant of heterosynaptic effects over inter-synapse spike timing difference
    double baseLineLTP{};
    double baseLineLTD{};
    double incrLTP{};
    double decrLTD{};
    std::unique_ptr<Kernel> kernel;
    WeightNormalization weightNormalization;


    // Overrides from HeteroSynapse
    void prepareSynapses() override;
    void updateCooperativityAmongSynapses() override;
    void advectSynapses() override;
    void finalizeSynapses() override;

    // Overrides from Synapse
    void advect_spikers(std::vector<double> * currents, long spiker) override;

    // Extension methods to enable STDP + Heterosynaptic effects
    // General STDP Kernels. Overload in subclass to determine type of STDP
    virtual double kernelPotentiation(double);
    virtual double kernelDepression(double);

    // LTP Enhancement, LTD Depression derived from Tazerart. Overload in subclass to change effects of hetero cooperation.
    virtual double heteroPreFactorLTP(SynapseExtension*) const;
    virtual double heteroPreFactorLTD(SynapseExtension*) const;

    // Distance and Timing effects derived from Tazerart. Overload in subclass to change how distance and timing between
    // synaptic spikes affects their cooperation
    virtual double heteroDistanceEffect(SynapseExtension*, SynapseExtension*) const;
    virtual double heteroTimingEffect(SynapseExtension*, SynapseExtension*) const;

public:
    STDPHeteroCurrentSynapse(NeuronPop* postNeurons, NeuronPop* preNeurons, GlobalSimInfo * info);
    ~STDPHeteroCurrentSynapse() override = default;

    // overrides from HeteroSynapse
    void SaveParameters(std::ofstream * stream,std::string id_str) override;
    void LoadParameters(std::vector<std::string> *input) override;

    int GetNumberOfDataColumns() override;
    std::string GetDataHeader(int data_column) override;
    std::string GetUnhashedDataHeader() override;
    std::valarray<double> GetSynapticState(int pre_neuron) override;
    std::string GetTypeStr() override;

};


#endif //NEURALNETWORK_TAZERARTHETEROSYNAPSE_H
