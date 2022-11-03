//
// Created by Saif Ahmed on 07.09.21.
//

#include "HeteroConnectivity.hpp"
#include "../Synapse/Synapse.hpp"
#include "../NeuronPop/HeteroNeuronPop.hpp"

HeteroConnectivity::HeteroConnectivity(Synapse *syn,GlobalSimInfo  * info): Connectivity(syn, info), synapticTargets(syn->GetNoNeuronsPre()) {
}

void HeteroConnectivity::setNoSourceNeurons(unsigned long noSN){
    if(noSN < 0)
        this->noSourceNeurons = 0;
    else if(noSN > this->synapse->GetNoNeuronsPre())
        this->noSourceNeurons = this->synapse->GetNoNeuronsPre();
    else if( (noSN == this->synapse->GetNoNeuronsPre()) && (this->synapse->IsRecurrent()))
        //noSourceNeurons = 0;
        this->noSourceNeurons = this->synapse->GetNoNeuronsPre()-1;
    else
        this->noSourceNeurons = noSN;
}

std::vector<std::pair<unsigned long, unsigned long>> HeteroConnectivity::getSynapticTargetList(unsigned long preId) {
    return this->synapticTargets.at(preId);
}

void HeteroConnectivity::connectNeuronsWithHeteroSynapses(std::vector<std::vector<std::shared_ptr<SynapseExtension>>>& synapseExtensions) {
    unsigned long numPostNeurons = synapse->GetNoNeuronsPost();
    long    output_Interval = numPostNeurons/10;
    if(output_Interval == 0)
        output_Interval = 1;


    auto* postNeuronPop = dynamic_cast<HeteroNeuronPop*>(this->synapse->GetNeuronsPost());

    unsigned long synId = 0;
    for(unsigned long postId = 0; postId < numPostNeurons; ++postId) {

        for(unsigned long preId = 0; preId < noSourceNeurons; ++preId) {

            if(synapse->IsRecurrent() && preId == postId) {
                continue;
            }

//            if(!target_id[preId].empty() && target_id[preId].back() == postId) {
//                continue;
//            }

            auto synAllocationSchemeFunc = [this](unsigned long preId, unsigned long postId, const SynAllocStateSharedPtr& currentAllocState) -> SynExtSharedPtr {
                return this->allocationScheme(preId, postId, currentAllocState);
            };
            synapseExtensions.at(postId).emplace_back(postNeuronPop->getNewSynapseAllocation(preId, postId, synAllocationSchemeFunc));

            synapticTargets[preId].push_back(std::make_pair(postId, synId++));
            target_id[preId].push_back(postId);
        }

        if((postId)%output_Interval == 0) {
            std::cout << 100 * postId / numPostNeurons << "%-";
        }
    }
}

void HeteroConnectivity::ConnectNeurons() {
    throw std::string("HeterosynapticConnectivity does not allow non-synaptic neuron connections.");
}

void HeteroConnectivity::SaveParameters(std::ofstream *stream, std::string id_str) {
    Connectivity::SaveParameters(stream, id_str);

    *stream << id_str << "connectionProbability\t" << this->connectionProb << "\n";
    if (this->distributeInitialSynapticWeights) {
        *stream << id_str << "distribute_initial_weights\t" << "true" << "\n";
    } else {
        *stream << id_str << "distribute_initial_weights\t" << "false\t" << this->homogenousInitialSynapticWeight << "\n";
    }

}

void HeteroConnectivity::LoadParameters(std::vector <std::string> *input) {
    Connectivity::LoadParameters(input);

    // additional load operations
    std::string name;
    std::vector<std::string> values;

    this->distributeInitialSynapticWeights = false;

    for (auto & it : *input) {
        SplitString(&it,&name,&values);

        if(name.find("seed") != std::string::npos){
            SetSeed(std::stoi(values.at(0)));
        } else if (name.find("ConnectionProba") != std::string::npos || name.find("connectionProbability") != std::string::npos) {
            this->connectionProb = std::stod(values.at(0));
            setNoSourceNeurons((unsigned long)((double) synapse->GetNoNeuronsPre() * std::stod(values.at(0))));
        } else if(name.find("noSourceNeurons") != std::string::npos){
            setNoSourceNeurons(std::stoul(values.at(0)));
        } else if (name.find("distribute_initial_weights") != std::string::npos) {
            if (values.at(0) == "true") {
                this->distributeInitialSynapticWeights = true;
            } else {
                this->homogenousInitialSynapticWeight = std::stod(values.at(1));
            }
        }
    }
}

