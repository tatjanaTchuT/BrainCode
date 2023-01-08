//
// Created by Saif Ahmed on 03.08.21.
//

#include "HeteroNeuronPop.hpp"

HeteroNeuronPop::HeteroNeuronPop(GlobalSimInfo *info, int id): NeuronPop(info, id) {
}

void HeteroNeuronPop::SaveParameters(std::ofstream * stream) {
    NeuronPop::SaveParameters(stream);
    if (this->morphology.size()!=0){
        morphology.at(0)->SaveParameters(stream, "neurons_" + std::to_string(GetId()));
    }
}

void HeteroNeuronPop::LoadParameters(std::vector<std::string> *input) {

    NeuronPop::LoadParameters(input);

    std::string name;
    std::vector<std::string> values;

    // checks for correct initialization
    bool morphologyFound = false;

    for (auto & it : *input) {
        SplitString(&it,&name,&values);

        if(name.find("morphology") != std::string::npos) {
            if (values.at(0) == str_MonoDendriteSTDPTazerart) {
                for (unsigned long i = 0; i < this->noNeurons; i++) {
                    this->morphology.push_back(std::make_unique<MonoDendriteSTDPTazerart>(this->info));
                    this->morphology.back()->LoadParameters(input);
                }
                morphologyFound = true;
            } else if (values.at(0) == str_MonoDendriteSTDPBiWindow) {
                for (unsigned long i = 0; i < this->noNeurons; i++) {
                    this->morphology.push_back(std::make_unique<MonoDendriteSTDPBiWindow>(this->info));
                    this->morphology.back()->LoadParameters(input);
                }
                morphologyFound = true;
            } else if (values.at(0) == str_MonoDendriteSTDPTazerartRelative) {
                for (unsigned long i = 0; i < this->noNeurons; i++) {
                    this->morphology.push_back(std::make_unique<MonoDendriteSTDPTazerartRelative>(this->info));
                    this->morphology.back()->LoadParameters(input);
                }
                morphologyFound = true;
            }
        }
    }

    assertm(morphologyFound == true, "Please specify morphology of the neurons to be used.");
}

std::shared_ptr<SynapseExt> HeteroNeuronPop::allocateNewSynapse(unsigned long neuronId, HeteroCurrentSynapse& syn) {
    return this->morphology[neuronId]->allocateNewSynapse(syn);
}

void HeteroNeuronPop::recordExcitatorySynapticSpike(unsigned long neuronId, unsigned long synapseId) {
    this->morphology[neuronId]->recordExcitatoryPreSpike(synapseId);
}

std::vector<unsigned long> getSpikedSynapses(const HeteroNeuronPop& pop, unsigned long neuronId) {
    return getSpikedSynapsesFromMorphology(*pop.morphology[neuronId].get());
}

unsigned long HeteroNeuronPop::getSynapseCount(unsigned long neuronId) {
    return this->morphology.at(neuronId)->getSynapseCount();
}

double HeteroNeuronPop::getWeight(unsigned long neuronId, unsigned long synapseId) {
    return this->morphology.at(neuronId)->getWeight(synapseId);
}

std::valarray<double> HeteroNeuronPop::getIndividualSynapticProfile(unsigned long neuronId, unsigned long synapseId) {
    return this->morphology.at(neuronId)->getIndividualSynapticProfile(synapseId);
}

std::valarray<double> HeteroNeuronPop::getOverallSynapticProfile(unsigned long neuronId) {
    return this->morphology.at(neuronId)->getOverallSynapticProfile();
}
// STDP Analysis
void HeteroNeuronPop::triggerStatOut(std::string dirPath) {
//    this->morphology.at(0)->triggerStatOut(dirPath);
}

void HeteroNeuronPop::printThetasAndWeights() {
    for (auto& m: this->morphology) {
        m->printThetasAndWeights();
    }
}
