//
// Created by Saif Ahmed on 07.09.21.
//

#include "HeteroNeuronPop.hpp"

HeteroNeuronPop::HeteroNeuronPop(GlobalSimInfo * info, int id): NeuronPop(info, id) {
}

// Update this method in order to add more details to allocation state along with potential extension of struct SynapseAllocationState
void HeteroNeuronPop::updateAllocationState(unsigned long neuronId, const SynExtSharedPtr& newSynapse) {
    SynAllocStateSharedPtr currentState = this->allocationState.at(neuronId);
    currentState->lastAllocatedPosition = newSynapse->distToSoma;
}

void HeteroNeuronPop::recordSpikerSynapse(unsigned long neuronId, const SynExtSharedPtr& synExt) {
    this->spikerSynapses.at(neuronId).insert(synExt);
}

const std::unordered_set<SynExtSharedPtr>& HeteroNeuronPop::getSpikerSynapses(unsigned long neuronId) {
    return this->spikerSynapses.at(neuronId);
}

void HeteroNeuronPop::clearSpikerSynapses() {
    for (auto& it: this->spikerSynapses) {
        it.clear();
    }
}

void HeteroNeuronPop::recordSpiker(unsigned long neuronId) {
    this->spiker.push_back(neuronId);
    this->lastNeuronalSpikeTime.at(neuronId) = (double) this->info->time_step * this->info->dt;
}

double HeteroNeuronPop::getLastNeuronalSpikeTime(unsigned long neuronId) const {
    return this->lastNeuronalSpikeTime.at(neuronId);
}

SynExtSharedPtr HeteroNeuronPop::getNewSynapseAllocation(unsigned long preId, unsigned long postId,
                                                         const std::function<SynExtSharedPtr(unsigned long, unsigned long, SynAllocStateSharedPtr)>& allocationScheme) {
    SynAllocStateSharedPtr currentState = this->allocationState.at(postId);

    // pass to allocation strategy and obtain a new Synapse
    std::shared_ptr<SynapseExtension> newSynapse = allocationScheme(preId, postId, currentState);
    // update allocation state
    this->updateAllocationState(postId, newSynapse);

    return newSynapse;
}

SynAllocStateSharedPtr HeteroNeuronPop::getAllocationState(unsigned long neuronId) {
    return this->allocationState.at(neuronId);
}


void HeteroNeuronPop::SaveParameters(std::ofstream * stream) {
    NeuronPop::SaveParameters(stream);

    // TODO: additional save operations
}


void HeteroNeuronPop::LoadParameters(std::vector<std::string> *input) {
    NeuronPop::LoadParameters(input);

    // initialize LastNeuronalSpike for each neuron
    this->lastNeuronalSpikeTime.resize(this->noNeurons);
    std::fill(this->lastNeuronalSpikeTime.begin(), this->lastNeuronalSpikeTime.end(), LAST_SPIKE_INIT_VAL);

    // initialize SpikerSynapses for each neuron
    this->spikerSynapses.resize(this->noNeurons);

    // initialize a SynapseAllocationState for each neuron
    this->allocationState.resize(this->noNeurons);
    for (auto& state: this->allocationState) {
        state = std::make_shared<SynapseAllocationState>();
    }
}

