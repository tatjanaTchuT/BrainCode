#include "HeteroCurrentSynapse.hpp"

HeteroCurrentSynapse::HeteroCurrentSynapse(NeuronPop* postNeurons, NeuronPop* preNeurons, GlobalSimInfo* info):
    Synapse(postNeurons, preNeurons, info) {

    // remove default legacy geometry initialization. It is done in HeteroCurrentSynapse::LoadParameters with appropriate assertions. OPTIMIZATION.
    delete this->geometry;
    this->geometry = new HeteroRandomConnectivity(this, this->info);

    // assert that HeteroSynapses are allowed only when the post NeuronPop supports heterosynaptic behavior
    assertm(postNeurons->HasHeterosynapticPlasticity(),
            "Attempting to create a Heterosynapse but the PostNeurons are not Heterosynaptic.");
}

void HeteroCurrentSynapse::advect(std::vector<double> * synaptic_dV) {

    resetcumulatedDV();

    std::vector<double> currents{};

    //Get list of spikers
    std::vector<long> spikers {*neuronsPre->GetSpikers()};

    //Go through all the spikers and add current arising from spikers to waiting_matrix
    for(auto const& spiker: spikers){

        size_t targetList_size {this->geometry->getSynapticTargets(spiker).size()};

        currents.resize(targetList_size, 0.0);
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
    const std::vector<std::pair<unsigned long, unsigned long>>& targetList{this->geometry->getSynapticTargets(spiker)}; 
    //OPTIMIZATION, targetList could be passed as a const reference to the previous copy (not doable currently, as this function overrides a virtual function with set arguments)
    //Overloaded function? Not necessary in others, as others use pointer. We cannot because the targetList is built differently (for now)

    double couplingStrength;
    double current;
    unsigned long postNeuronId;
    unsigned long HCSSynapseId;
    unsigned long morphoSynapseId;

    std::pair<unsigned long, unsigned long> neuronSynapsePair;

    for (int i = 0; i < targetList.size(); i++) {//i here is a postNeuronId
        neuronSynapsePair = targetList.at(i);
        postNeuronId = neuronSynapsePair.first;
        HCSSynapseId = neuronSynapsePair.second;
        morphoSynapseId = this->synapseData[HCSSynapseId]->idInMorpho;

        couplingStrength = GetCouplingStrength(spiker, i); // i is used as "postId" because of how SetDistributionJ is implemented in Connectivity.cpp
        if (couplingStrength < 0.0) {//To avoid interaction with inhibitory synapses
            current =  couplingStrength;
        } else {
            current = couplingStrength * this->synapseData[HCSSynapseId]->weight;
            this->neuronsPost->recordExcitatorySynapticSpike(postNeuronId, morphoSynapseId);
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
    int branchId;
    if (randomTargetBranch){
    HeteroNeuronPop* heteroPop {dynamic_cast<HeteroNeuronPop*>(this->neuronsPost)};
    int branchId = heteroPop->morphology.at(postId)->getrandomTargetBranch();// Fix this with a reference to morphology
    }
    else
    { int branchId{targetBranch};}
    std::shared_ptr<SynapseExt> synapseExtPtr = this->neuronsPost->allocateNewSynapse(postId, branchId);

    if (synapseExtPtr != nullptr) {
        synapseExtPtr->preNeuronId = preId;
        synapseExtPtr->postNeuronId = postId;
        synapseExtPtr->idInHCS = static_cast<unsigned long>(this->synapseData.size());

        this->synapseData.push_back(synapseExtPtr);

        return synapseExtPtr->idInHCS;
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
    val[0] = Jsum/static_cast<double>(this->GetNumberOfPostsynapticTargets(pre_neuron));
    //val[0] = GetCouplingStrength()*double(this->GetNumberOfPostsynapticTargets(pre_neuron));
    return val;
}
/*
const std::vector<std::pair<unsigned long, unsigned long>>& getSynapticTargets(HeteroCurrentSynapse& syn, const unsigned long& preId) {
    return syn.geometry->getSynapticTargets(preId);
}

std::vector<SynapseExt> getSynapseData(HeteroCurrentSynapse& syn) {
    std::vector<SynapseExt> synData;
    for (const auto& item: syn.synapseData) {
        synData.push_back(*item);
    }
    return synData;
}
*/
