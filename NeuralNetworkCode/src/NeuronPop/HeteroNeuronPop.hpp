//
// Created by Saif Ahmed on 03.08.21.
//

#ifndef NEURALNETWORK_HETEROPOP_H
#define NEURALNETWORK_HETEROPOP_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <unordered_set>
#include <algorithm>
#include <memory>
#include <functional>
#include <cfloat>

#include "../Connectivity/HeteroConnectivity.hpp"
#include "../GlobalFunctions.hpp"
#include "NeuronPop.hpp"

#define LAST_SPIKE_INIT_VAL (-100000)

typedef std::shared_ptr<SynapseAllocationState> SynAllocStateSharedPtr;
typedef std::shared_ptr<SynapseExtension> SynExtSharedPtr;

class HeteroNeuronPop: public NeuronPop {

protected:
    /** Data shared amongst Synapse objects that share NeuronPop object as postsynaptic population*/
    // TODO: Task - Implement SharedSynapticState mechanism for all Heterosynapse using name post population
    std::vector<double> lastNeuronalSpikeTime{};
    std::vector<std::unordered_set<SynExtSharedPtr>> spikerSynapses{};
    std::vector<SynAllocStateSharedPtr> allocationState{}; // used in the allocation of new synapses
    /** Data shared amongst Synapse objects that share NeuronPop object as postsynaptic population*/

    // Extension methods to enable Heterosynaptic effects
    void updateAllocationState(unsigned long, const SynExtSharedPtr&);

public:
    HeteroNeuronPop(GlobalSimInfo * info, int id);
    ~HeteroNeuronPop() override = default;

    // Extension methods to enable Heterosynaptic effects: Synapses Spikers read/write
    void recordSpikerSynapse(unsigned long, const SynExtSharedPtr&);
    const std::unordered_set<SynExtSharedPtr>& getSpikerSynapses(unsigned long);
    void clearSpikerSynapses();

    // Extension methods to enable Heterosynaptic effects: Post Spike read/write
    void recordSpiker(unsigned long);
    double getLastNeuronalSpikeTime(unsigned long) const;

    // Extension methods to enable Heterosynaptic effects: Synapse allocation according on dendrite based on HeteroConnectivity
    SynExtSharedPtr getNewSynapseAllocation(unsigned long preId, unsigned long postId, const std::function<SynExtSharedPtr(unsigned long, unsigned long, SynAllocStateSharedPtr)>&);
    SynAllocStateSharedPtr getAllocationState(unsigned long);

    // overrides of NeuronPop methods
    void SaveParameters(std::ofstream * stream) override;
    void LoadParameters(std::vector<std::string> *input) override;

};


#endif //NEURALNETWORK_HETEROPOP_H
