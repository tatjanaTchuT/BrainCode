#include "HeteroCurrentSynapse.hpp"

HeteroCurrentSynapse::HeteroCurrentSynapse(NeuronPop* postNeurons, NeuronPop* preNeurons, GlobalSimInfo* info):
    Synapse(postNeurons, preNeurons, info) {

    // remove default legacy geometry initialization. It is done in HeteroCurrentSynapse::LoadParameters with appropriate assertions.
    //Ok, but this is secondary for now
    delete this->geometry;
    this->geometry = new HeteroRandomConnectivity(this, this->info);

    // assert that HeteroSynapses are allowed only when the post NeuronPop supports heterosynaptic behavior
    assertm(dynamic_cast<HeteroNeuronPop*>(postNeurons) != nullptr,
            "Attempting to create a Heterosynapse but the PostNeurons and/or PreNeurons are not Heterosynaptic.");
    //GOOD
}

void HeteroCurrentSynapse::advect(std::vector<double> * synaptic_dV) {

    resetcumulatedDV();

    std::vector<double> currents{};

    //Get list of spikers
    std::vector<long> spikers {*neuronsPre->GetSpikers()};
    std::vector<std::pair<unsigned long, unsigned long>> targetList{};
    auto* connectivity { dynamic_cast<HeteroRandomConnectivity*>(this->geometry)};

    //Go through all the spikers and add current arising from spikers to waiting_matrix
    for(auto const& spiker: spikers){
        targetList = connectivity->getSynapticTargets(spiker);

        currents.resize(targetList.size(), 0.0);
        //OPTIMIZATION: To further improve speed, at the cost of memory, each neuron's currents matrix should be kept in the connectivity object, indexed per neuron.
        // This would avoid having to resize and clear the currents matrix.


        advect_spikers(currents, spiker);
        //std::fill(currents.begin(), currents.end(), 0);//initializes vector containing the Current to each target
        
        FillWaitingMatrix(spiker, currents);
        currents.clear();//Remove this line if the std::fill() is uncommented
    }

    ReadWaitingMatrixEntry(*synaptic_dV);
    advect_finalize(&waiting_matrix);
}

void HeteroCurrentSynapse::advect_spikers(std::vector<double>& currents, long spiker) {
    const std::vector<std::pair<unsigned long, unsigned long>> targetList{ dynamic_cast<HeteroRandomConnectivity*>(this->geometry)->getSynapticTargets(spiker)}; 
    //OPTIMIZATION, targetList could be passed as a const reference to the previous copy (not doable currently, as this function overrides a virtual function with set arguments)

    auto* heteroNeuronsPost { dynamic_cast<HeteroNeuronPop*>(this->neuronsPost)}; 

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
        if (couplingStrength < 0.0) {
            current =  couplingStrength;
        } else {
            current = couplingStrength * this->synapseData[localSynapseId]->weight;
            heteroNeuronsPost->recordExcitatorySynapticSpike(postNeuronId, globalSynapseId);
        }
        currents[i] += current;
        this->cumulatedDV += current;
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

    auto* heteroNeuronsPost = dynamic_cast<HeteroNeuronPop*>(this->neuronsPost);

    std::shared_ptr<SynapseExt> synapseExtPtr = heteroNeuronsPost->allocateNewSynapse(postId);

    if (synapseExtPtr != nullptr) {
        synapseExtPtr->preNeuronId = preId;
        synapseExtPtr->postNeuronId = postId;
        synapseExtPtr->localId = static_cast<unsigned long>(this->synapseData.size());

        this->synapseData.push_back(synapseExtPtr);

        return synapseExtPtr->localId;
    }

    throw noAllocatableSynapseException();
}

std::string HeteroCurrentSynapse::GetDataHeader(int data_column) {
    return "#" + std::to_string(data_column) + " J_"+ GetIdStr() + " (mV)\n";
}

std::string HeteroCurrentSynapse::GetUnhashedDataHeader() {
    return "J_" + GetIdStr() + "\t";;
}

std::valarray<double> HeteroCurrentSynapse::GetSynapticState(int pre_neuron) {
    std::valarray<double> val(1);
    double Jsum = 0;
    // get average coupling strength
    for(unsigned int target=0; target < this->GetNumberOfPostsynapticTargets(pre_neuron); target++){
        Jsum += *(geometry->GetDistributionJ(pre_neuron,target));
    }
    val[0] = Jsum/double(this->GetNumberOfPostsynapticTargets(pre_neuron));
    //val[0] = GetCouplingStrength()*double(this->GetNumberOfPostsynapticTargets(pre_neuron));
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
