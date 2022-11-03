//
// Created by Saif Ahmed on 07.09.21.
//

#ifndef NEURONAL_NETWORK_SIMULATIONS_HETEROSYNAPTICCONNECTIVITY_H
#define NEURONAL_NETWORK_SIMULATIONS_HETEROSYNAPTICCONNECTIVITY_H

#include <string>
#include <memory>
#include <cfloat>

#include "Connectivity.hpp"

#define LAST_SPIKE_INIT_VAL (-100000)

class Synapse;
class HeteroNeuronPop;

class HeteroConnectivity: public Connectivity {

protected:
    // Extension members to enable Heterosynaptic effects
    std::vector<std::vector<std::pair<unsigned long, unsigned long>>> synapticTargets; //the list with postsynaptic (or target) neurons and syanpseId (Pair<>) for each neuron of the presynaptic population
    bool distributeInitialSynapticWeights{};
    double homogenousInitialSynapticWeight{};
    unsigned long noSourceNeurons{};
    double connectionProb{};

    void setNoSourceNeurons(unsigned long noSN);

public:
    HeteroConnectivity(Synapse *syn,GlobalSimInfo *info);
    ~HeteroConnectivity() override = default;

    // Extension methods to enable Heterosynaptic effects
    std::vector<std::pair<unsigned long, unsigned long>> getSynapticTargetList(unsigned long);
    void connectNeuronsWithHeteroSynapses(std::vector<std::vector<std::shared_ptr<SynapseExtension>>>&);
    virtual std::shared_ptr<SynapseExtension> allocationScheme(unsigned long, unsigned long, std::shared_ptr<SynapseAllocationState>) = 0;


    // overrides of Synapse methods
    void ConnectNeurons() override;
    void SaveParameters(std::ofstream * stream,std::string id_str) override;
    void LoadParameters(std::vector<std::string> *input) override;
};


#endif //NEURONAL_NETWORK_SIMULATIONS_HETEROSYNAPTICCONNECTIVITY_H
