//
// Created by Saif Ahmed on 07.09.21.
//

#include "HeteroSynapse.hpp"

HeteroSynapse::HeteroSynapse(NeuronPop* postNeurons, NeuronPop* preNeurons, GlobalSimInfo* info): Synapse(postNeurons, preNeurons, info) {
    // resize to match number of postsynaptic neurons
    this->synapseExtensions.resize(neuronsPost->GetNoNeurons());

    // assert that HeteroSynapses are allowed only when the post NeuronPop supports heterosynaptic behavior
    assertm(dynamic_cast<HeteroNeuronPop*>(postNeurons) != nullptr,"Attempting to create a Heterosynapse but the PostNeurons are not Heterosynaptic.");
}

/**
 * "preAdvect" performs following operations: (non-abstract subclass example STDPHeteroCurrentSynapse)
 *  1. updates cooperativity through "updateCooperativity" which is overloaded in subclass
 *  2. updates plasticity through "advectSynapses" which is overloaded in subclass
 *  3. normalizes out of bounds synapses due to plasticity updates in 2
 * */
void HeteroSynapse::preAdvect() {
    this->prepareSynapses(); //overloaded in subclass // any state updates required for the synapses
    this->updateCooperativityAmongSynapses(); //overloaded in subclass // cooperativity changes here
    this->advectSynapses();  // overloaded in subclass // plasticity changes here
    this->finalizeSynapses(); //overloaded in subclass
}

/**
 * "advect" performs following operations: (non-abstract subclass example STDPHeteroCurrentSynapse)
 *  1. determines synaptic spikers and records them at the postsynaptic neuron through "advect_spikers" which is overloaded in subclass
 *  2. waiting matrix mechanism to implement delays (ask Pierre/Andreas)
 * */
void HeteroSynapse::advect(std::vector<double> * synaptic_dV) {
    //Get list of spikers
    std::vector<long> spikerNeurons = *neuronsPre->GetSpikers();

    resetcumulatedDV();
    std::vector<double> currents;
    auto* connectivity = dynamic_cast<HeteroConnectivity*>(this->geometry);

    //Go through all the spikers and add current arising from spikers to waiting_matrix
    for(auto const& spikerNeuron: spikerNeurons){
        std::vector<std::pair<unsigned long, unsigned long>> targetList = connectivity->getSynapticTargetList(spikerNeuron);
        currents.resize(targetList.size(),0);
        std::fill(currents.begin(), currents.end(), 0);//initializes vector containing the Current to each target

        advect_spikers(&currents, spikerNeuron); // overloaded in subclass
        FillWaitingMatrix(spikerNeuron, &currents, &this->waiting_matrix);
    }

    ReadWaitingMatrixEntry(&this->waiting_matrix, synaptic_dV);
    advect_finalize(&waiting_matrix);
}

void HeteroSynapse::advect_finalize(std::vector<std::vector<double>> * waiting_matrix) {
    //Placeholder for vitual function, not necessary.
}

void HeteroSynapse::ConnectNeurons() {
    auto* heteroGeometry = dynamic_cast<HeteroConnectivity*>(this->geometry);
    heteroGeometry->connectNeuronsWithHeteroSynapses(this->synapseExtensions);
}

void HeteroSynapse::SaveParameters(std::ofstream * stream,std::string id_str) {
    Synapse::SaveParameters(stream, id_str);

    *stream << id_str << "heterosynaptic_theta_decay\t" << this->tauTheta << "\ts\n";
}

void HeteroSynapse::LoadParameters(std::vector<std::string> *input) {
    Synapse::LoadParameters(input);

    std::string name;
    std::vector<std::string> values;

    bool tauThetaInitialized = false;

    for (auto & it : *input) {
        SplitString(&it,&name,&values);

        if (name.find("heterosynaptic_theta_decay") != std::string::npos) {
            this->tauTheta = std::stod(values.at(0));
            tauThetaInitialized = true;
        }
    }
    assertm(tauThetaInitialized == true, "Using heterosynaptic synapses without specifying decay for cooperativity is not allowed.");

}

std::string HeteroSynapse::getHeteroSynapticDataHeader() {
    std::string dataHeader;
    for (unsigned  long i = 0; i < neuronsPost->GetNoNeurons(); ++i) {
        dataHeader += "stateOf_postId_" + std::to_string(i) + "\t";
    }
    return dataHeader;
}

std::string HeteroSynapse::getHeterosynapticState(unsigned long postId) const {
    std::string stateString;
    for (const auto& syn : this->synapseExtensions.at(postId)) {
        stateString += std::to_string(syn->preNeuronId) + "," +
                       std::to_string(syn->postNeuronId) + "," +
                       std::to_string(syn->distToSoma) + "," +
                       std::to_string(syn->lastSpike) + "," +
                       std::to_string(syn->theta) + "," +
                       std::to_string(syn->weight) +
                       "\t";
    }
    stateString.back() = ' ';
    return stateString;
}

std::vector<std::vector<SynExtSharedPtr>> HeteroSynapse::getSynapseExtensions() const {
    return this->synapseExtensions;
}
