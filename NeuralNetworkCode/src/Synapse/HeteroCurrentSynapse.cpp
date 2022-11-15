#include "HeteroCurrentSynapse.hpp"

HeteroCurrentSynapse::HeteroCurrentSynapse(NeuronPop* postNeurons, NeuronPop* preNeurons, GlobalSimInfo* info):
    Synapse(postNeurons, preNeurons, info) {

    // remove default legacy geometry initialization. It is done in HeteroCurrentSynapse::LoadParameters with appropriate assertions
    delete this->geometry;
    this->geometry = new HeteroRandomConnectivity(this, this->info);

    // assert that HeteroSynapses are allowed only when the post NeuronPop supports heterosynaptic behavior
    assertm(dynamic_cast<HeteroLIFNeuronPop*>(postNeurons) != nullptr,
            "Attempting to create a Heterosynapse but the PostNeurons and/or PreNeurons are not Heterosynaptic.");

}

void HeteroCurrentSynapse::advect(std::vector<double> * synaptic_dV) {

    resetcumulatedDV();

    std::vector<double> currents;

    //Get list of spikers
    std::vector<long> spikers = *neuronsPre->GetSpikers();

    auto* connectivity = dynamic_cast<HeteroRandomConnectivity*>(this->geometry);

    //Go through all the spikers and add current arising from spikers to waiting_matrix
    for(auto const& spiker: spikers){
        std::vector<std::pair<unsigned long, unsigned long>> targetList = connectivity->getSynapticTargets(spiker);
        currents.resize(targetList.size(),0);
        std::fill(currents.begin(), currents.end(), 0);//initializes vector containing the Current to each target

        advect_spikers(currents, spiker);

        FillWaitingMatrix(spiker, currents);
    }

    ReadWaitingMatrixEntry(*synaptic_dV);
    advect_finalize(&waiting_matrix);
}

void HeteroCurrentSynapse::advect_spikers(std::vector<double>& currents, long spiker) {
    auto* connectivity = dynamic_cast<HeteroRandomConnectivity*>(this->geometry);
    std::vector<std::pair<unsigned long, unsigned long>> targetList = connectivity->getSynapticTargets(spiker);

    auto* heteroNeuronsPost = dynamic_cast<HeteroLIFNeuronPop*>(this->neuronsPost);

    double couplingStrength;
    double current;
    unsigned long postNeuronId;
    unsigned long localSynapseId;
    unsigned long globalSynapseId;

    std::pair<unsigned long, unsigned long> neuronSynapsePair;
    for (int i = 0; i < targetList.size(); i++) {
        neuronSynapsePair = targetList.at(i);
        postNeuronId = neuronSynapsePair.first;
        localSynapseId = neuronSynapsePair.second;
        globalSynapseId = this->synapseData[localSynapseId]->globalId;

        couplingStrength = GetCouplingStrength(spiker, i); // i is used as "postId" because of how SetDistributionJ is implemented in Connectivity.cpp
//        std::cout << couplingStrength << std::endl;
        current = couplingStrength * this->synapseData[localSynapseId]->weight;
//        std::cout << current << std::endl;
        currents[i] += current;
        this->cumulatedDV += current;
        heteroNeuronsPost->recordSynapticSpike(postNeuronId, globalSynapseId);
    }
}

void HeteroCurrentSynapse::advect_finalize(std::vector<std::vector<double>> * waiting_matrix) {
}

void HeteroCurrentSynapse::LoadParameters(std::vector<std::string> *input){
    Synapse::LoadParameters(input);
}

void HeteroCurrentSynapse::SaveParameters(std::ofstream * stream, std::string id_str){
    // TODO: What extra parameters need to be saved
    Synapse::SaveParameters(stream,id_str);
}

unsigned long HeteroCurrentSynapse::allocateSynapse(unsigned long preId, unsigned long postId) {

    auto* heteroNeuronsPost = dynamic_cast<HeteroLIFNeuronPop*>(this->neuronsPost); //Why HeteroLIF? What is wrong?

    std::shared_ptr<SynapseExt> synapseExtPtr = heteroNeuronsPost->allocateNewSynapse(postId); //the function is in HeteroPop

    if (synapseExtPtr != nullptr) {
        synapseExtPtr->preNeuronId = preId;
        synapseExtPtr->postNeuronId = postId;
        synapseExtPtr->localId = this->synapseData.size();

        this->synapseData.push_back(synapseExtPtr);

        return synapseExtPtr->localId;
    }

    throw noAllocatableSynapseException();
}

int HeteroCurrentSynapse::GetNumberOfDataColumns() {
    return 0;
}

std::string HeteroCurrentSynapse::GetDataHeader(int data_column) {
    return "nothing";
}

std::string HeteroCurrentSynapse::GetUnhashedDataHeader() {
    return "nothing";
}

std::valarray<double> HeteroCurrentSynapse::GetSynapticState(int pre_neuron) {
    std::valarray<double> val;
    return val;
}

std::vector<std::pair<unsigned long, unsigned long>> getSynapticTargets(HeteroCurrentSynapse& syn, unsigned long preId) {
    auto* connectivity = dynamic_cast<HeteroRandomConnectivity*>(syn.geometry);
    return connectivity->getSynapticTargets(preId);
}

std::vector<SynapseExt> getSynapseData(HeteroCurrentSynapse& syn) {
    std::vector<SynapseExt> synData;
    for (const auto& item: syn.synapseData) {
        synData.push_back(*item);
    }
    return synData;
}
