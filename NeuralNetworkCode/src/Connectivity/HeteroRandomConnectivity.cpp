#include "HeteroRandomConnectivity.hpp"

HeteroRandomConnectivity::HeteroRandomConnectivity(Synapse* syn, GlobalSimInfo* info):
    RandomConnectivity(syn, info), synapticTargets(syn->GetNoNeuronsPre()) {
}

void HeteroRandomConnectivity::ConnectNeurons() {

    unsigned long numPostNeurons = synapse->GetNoNeuronsPost();

    unsigned long output_Interval = numPostNeurons / 10;
    output_Interval = output_Interval == 0 ? 1 : output_Interval;

    std::uniform_int_distribution<int> distribution(0,(int)synapse->GetNoNeuronsPre()-1);

    auto* heteroSynapse = dynamic_cast<HeteroCurrentSynapse*>(this->synapse);
    unsigned long source, countedSourceNeurons, synapseId;

    for(unsigned long target = 0; target < numPostNeurons; target++) {
        countedSourceNeurons = 0;

        while(countedSourceNeurons < this->noSourceNeurons) {
            source = distribution(generator);

            if(synapse->IsRecurrent() && source == target) {
                continue;
            }

            if(!target_id[source].empty() && target_id[source].back() == target) {
                continue;
            }

            try {
                synapseId = heteroSynapse->allocateSynapse(source, target);
                synapticTargets[source].push_back(std::make_pair(target, synapseId));
                target_id[source].push_back(target);
                countedSourceNeurons++;
            } catch (const noAllocatableSynapseException& e) {
                break;
            }
        }
        if((target)%output_Interval == 0) {
            std::cout << 100 * target / numPostNeurons << "%-";
        }
    }

}

void HeteroRandomConnectivity::SaveParameters(std::ofstream *stream, const std::string &id_str) {
    RandomConnectivity::SaveParameters(stream, id_str);
}

void HeteroRandomConnectivity::LoadParameters(std::vector<std::string> *input) {
    RandomConnectivity::LoadParameters(input);
}

std::vector<std::pair<unsigned long, unsigned long>> HeteroRandomConnectivity::getSynapticTargets(unsigned long preId) {
    return this->synapticTargets.at(preId);
}

std::string HeteroRandomConnectivity::GetTypeStr() {
    return str_heteroRandomConnectivity;
}


//unsigned long HeteroRandomConnectivity::GetNumberAverageSourceNeurons() {
//    return 0;
//}
//
//int* HeteroRandomConnectivity::GetDistributionD(long preNeuronId, long postNeuronId) {
//    return &D_distribution[preNeuronId][postNeuronId];
//}
//
//void HeteroRandomConnectivity::SetDistributionD() {
//    this->D_distribution->resize(this->synapse->GetNoNeuronsPre());
//    for (unsigned long i = 0; i < this->synapse->GetNoNeuronsPre(); ++i) {
//        this->D_distribution[i].resize(this->synapse->GetNoNeuronsPost());
//        std::fill(this->D_distribution[i].begin(), this->D_distribution[i].end(), 0);
//    }
//}
//
//double *HeteroRandomConnectivity::GetDistributionJ(long preNeuronId, long postNeuronId) {
//    return &J_distribution[preNeuronId][postNeuronId];
//}
//
//void HeteroRandomConnectivity::SetDistributionJ() {
//    this->J_distribution->resize(this->synapse->GetNoNeuronsPre());
//    for (unsigned long i = 0; i < this->synapse->GetNoNeuronsPre(); ++i) {
//        this->J_distribution[i].resize(this->synapse->GetNoNeuronsPost());
//        std::fill(this->J_distribution[i].begin(), this->J_distribution[i].end(), 1.0);
//    }
//}