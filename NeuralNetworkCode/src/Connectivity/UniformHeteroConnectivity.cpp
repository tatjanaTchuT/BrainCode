//
// Created by Saif Ahmed on 07.09.21.
//

#include "UniformHeteroConnectivity.hpp"
#include "../Synapse/Synapse.hpp"
#include "../NeuronPop/HeteroNeuronPop.hpp"

UniformHeteroConnectivity::UniformHeteroConnectivity(Synapse *syn,GlobalSimInfo  * info): HeteroConnectivity(syn, info) {
}

std::shared_ptr<SynapseExtension> UniformHeteroConnectivity::allocationScheme(unsigned long preId, unsigned long postId,
                                                                              std::shared_ptr<SynapseAllocationState> prevAllocationState) {

    std::uniform_real_distribution<double> distribution(0.0,2.0);

    std::shared_ptr<SynapseExtension> newSynapse = std::make_shared<SynapseExtension>();
    newSynapse->distToSoma = prevAllocationState->lastAllocatedPosition + this->synapticGap;
    newSynapse->lastSpike = LAST_SPIKE_INIT_VAL; // large negative value indicates no spikes of synapse during simulation
    newSynapse->theta = 0;
    if (this->distributeInitialSynapticWeights) {
        std::random_device rd;
        std::default_random_engine generator(rd()); // rd() provides a random seed
        newSynapse->weight = distribution(generator);
    } else {
        newSynapse->weight = this->homogenousInitialSynapticWeight; // assuming a range of weight between 0 and 2, weight is initialized to midpoint: 1
    }
    newSynapse->preNeuronId = preId;
    newSynapse->postNeuronId = postId;
    newSynapse->ltpEnabled = false;
    newSynapse->ltdEnabled = false;

    return newSynapse;
}

void UniformHeteroConnectivity::SaveParameters(std::ofstream *stream, const std::string id_str) {
    HeteroConnectivity::SaveParameters(stream, id_str);

    *stream << id_str << "connectivity_synaptic_gap\t" << this->synapticGap << "\n";
}

void UniformHeteroConnectivity::LoadParameters(std::vector <std::string> *input) {
    HeteroConnectivity::LoadParameters(input);

    std::string name;
    std::vector<std::string> values;

    bool synapticGapInitialized = false;

    for (auto & it : *input) {
        SplitString(&it,&name,&values);

        if (name.find("synaptic_gap") != std::string::npos) {
            this->synapticGap = std::stod(values.at(0));
            synapticGapInitialized = true;
        }
    }

    assertm(synapticGapInitialized == true, "Using heterosynaptic connectivity without specifying synaptic_gap is not allowed.");
}

std::string UniformHeteroConnectivity::GetTypeStr() {
    return str_uniformHeteroConnectivity;
}

unsigned long UniformHeteroConnectivity::GetNumberAverageSourceNeurons() {
    return synapse->GetNoNeuronsPre();
}
