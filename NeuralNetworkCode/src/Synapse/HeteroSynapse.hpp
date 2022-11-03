//
// Created by Saif Ahmed on 07.09.21.
//

#ifndef NEURONAL_NETWORK_SIMULATIONS_HETEROCURRENTSYNAPSE_H
#define NEURONAL_NETWORK_SIMULATIONS_HETEROCURRENTSYNAPSE_H

#include "../GlobalFunctions.hpp"
#include "../NeuronPop/HeteroNeuronPop.hpp"
#include "Synapse.hpp"
#define MAX_WEIGHT 2.0
#define MIN_WEIGHT 0.0

/**
 * HeteroSynapse class
 *
 * Private/Protected members:
 *  - synapseExtensions: a vector for each post neuron containing the synapses instances (SynExt) associated with that neuron
 *  - tauTheta: determines the decay in cooperativity of a SynExt
 */
class HeteroSynapse: public Synapse {

protected:
    // Extension members to enable Heterosynaptic effects
    std::vector<std::vector<std::shared_ptr<SynapseExtension>>> synapseExtensions{}; // extensions representing point synapses. One vector for each post neuron
    double tauTheta{}; //

    // Extension methods to enable Heterosynaptic effects. These methods are sequentially called in HeteroSynapse::preAdvect
    virtual void prepareSynapses() = 0; 
    virtual void updateCooperativityAmongSynapses() = 0;
    virtual void advectSynapses() = 0;
    virtual void finalizeSynapses() = 0;

public:
    HeteroSynapse(NeuronPop* postNeurons, NeuronPop* preNeurons, GlobalSimInfo * info);
    ~HeteroSynapse() override = default;

    // overrides from Synapse
    void preAdvect() override; // updates synaptic states: cooperativity, plasticity ...
    void advect(std::vector<double> *  synaptic_dV) override; // updates synaptic_dV, identifies spiker synapses for next iteration
    void advect_finalize(std::vector<std::vector<double>> * waiting_matrix) override; // does nothing in the default form
    void ConnectNeurons() override;
    void SaveParameters(std::ofstream * stream,std::string id_str) override;
    void LoadParameters(std::vector<std::string> *input) override;

    // testing
    std::vector<std::vector<SynExtSharedPtr>> getSynapseExtensions() const;

    // recording
    virtual std::string getHeteroSynapticDataHeader();
    virtual std::string getHeterosynapticState(unsigned long) const;

};


#endif //NEURONAL_NETWORK_SIMULATIONS_HETEROCURRENTSYNAPSE_H
