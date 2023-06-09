#include "HeteroCurrentSynapse.hpp"

HeteroCurrentSynapse::HeteroCurrentSynapse(NeuronPop* postNeurons, NeuronPop* preNeurons, GlobalSimInfo* info):
    Synapse(postNeurons, preNeurons, info) {

    // remove default legacy geometry initialization. It is done in HeteroCurrentSynapse::LoadParameters with appropriate assertions. OPTIMIZATION.
    delete this->geometry;
    this->geometry = new HeteroRandomConnectivity(this, this->info);

    // assert that HeteroSynapses are allowed only when the post NeuronPop supports heterosynaptic behavior
    if (!postNeurons->HasHeterosynapticPlasticity()){
        throw;
    }
}

void HeteroCurrentSynapse::advect(std::vector<double> * synaptic_dV) {

    resetcumulatedDV();

    std::vector<double> currents{};

    //Get list of spikers
    const std::vector<long>& spikers = *neuronsPre->GetSpikers();

    //Go through all the spikers and add current arising from spikers to waiting_matrix
    for(const auto& spiker: spikers){
 
        currents.resize(this->geometry->getSynapticTargets(spiker).size(), 0.0);
        //OPTIMIZATION: To further improve speed, at the cost of memory, each neuron's currents matrix should be kept in the connectivity object, indexed per neuron.
        // This would avoid having to resize and clear the currents matrix.


        advect_spikers(currents, spiker);
        //std::fill(currents.begin(), currents.end(), 0);//initializes vector containing the Current to each target
        
        FillWaitingMatrix(spiker, currents);
        currents.clear();//Remove this line if the std::fill() is uncommented
    }

    ReadWaitingMatrixEntry(*synaptic_dV);
    //advect_finalize(&waiting_matrix);
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
        morphoSynapseId = this->synapseData[HCSSynapseId]->getIdInMorpho();

        couplingStrength = GetCouplingStrength(spiker, i); // i is used as "postId" because of how SetDistributionJ is implemented in Connectivity.cpp
        if (couplingStrength < 0.0) {//To avoid interaction with inhibitory synapses
            current =  couplingStrength;
        } else {
            current = couplingStrength * this->synapseData[HCSSynapseId]->getWeight();
            this->neuronsPost->recordExcitatorySynapticSpike(postNeuronId, morphoSynapseId);
        }
        currents[i] += current;
        this->cumulatedDV += current;
    }
}

//void HeteroCurrentSynapse::advect_finalize(std::vector<std::vector<double>> * waiting_matrix) {}

void HeteroCurrentSynapse::LoadParameters(std::vector<std::string> *input){
    std::string              name;
    std::vector<std::string> values;

    for(std::vector<std::string>::iterator it = (*input).begin(); it != (*input).end(); ++it) {
        SplitString(&(*it),&name,&values);
        if(name.find("target_branch") != std::string::npos){
            if (values.at(0).find("random")){
                this->synapseTargeting.randomTargetBranch=true;
            } else if (values.at(0).find("none") != std::string::npos){
                //Here nothing is done to handle the case where we do not used branched
                //Checking if the input is an integer would take more time than checking if None is in a single string with 5 characters max (I think).
            }else if (values.at(0).find("ordered") != std::string::npos){
                this->synapseTargeting.orderedTargetBranch=true;
            } else {
            this->synapseTargeting.targetBranch = std::stoi(values.at(1));
            this->synapseTargeting.setTargetBranch=true;
            //Missing exception management for when the input is not an integer.
            }
        } else if(name.find("subregion") != std::string::npos){
            this->synapseTargeting.subRegion = values[0][0];
        }
    }

    Synapse::LoadParameters(input);
}

void HeteroCurrentSynapse::SaveParameters(std::ofstream * stream, std::string id_str){

    *stream << id_str << "target_branch\t\t\t\t\t";
    if (this->synapseTargeting.randomTargetBranch){
        *stream<<"random\n"; //Missing comments on what this is supposed to do
    } else if(this->synapseTargeting.setTargetBranch){
        *stream<<std::to_string(this->synapseTargeting.targetBranch)<<"\n";//Missing comments on what this is supposed to do
    }else if(this->synapseTargeting.orderedTargetBranch){
       *stream<<"ordered\n";
    } else {
        *stream<<"none\n";//Missing comments on what this is supposed to do
    }
    *stream << "#\t\tYou can target branches in an 'ordered' manner (0,1,2...), 'random', or set (if you input a number). Put none if the HS does not used branched morphology\n";
    
    *stream << id_str << "subregion\t\t\t\t\t\t" << (this->synapseTargeting.subRegion) << "\n";
    *stream << "#\t\tThis is currently under development, but will allow you to determine subregions and target subregions of the dendritic tree.\n";
    //Missing comments on what this is supposed to do and check if char goes out properly
    Synapse::SaveParameters(stream,id_str);
}

unsigned long HeteroCurrentSynapse::allocateSynapse(unsigned long preId, unsigned long postId) {
    std::shared_ptr<SynapseSpine> SynapseSpinePtr = this->neuronsPost->allocateNewSynapse(postId, *this);//everything except first var can be moved to syn ref

    if (SynapseSpinePtr != nullptr) {
        SynapseSpinePtr->setPreNeuronId(preId);
        SynapseSpinePtr->setPostNeuronId(postId);
        SynapseSpinePtr->setIdInHCS(static_cast<unsigned long>(this->synapseData.size()));

        this->synapseData.push_back(SynapseSpinePtr);

        return SynapseSpinePtr->getIdInHCS();
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

std::vector<SynapseSpine> getSynapseData(HeteroCurrentSynapse& syn) {
    std::vector<SynapseSpine> synData;
    for (const auto& item: syn.synapseData) {
        synData.push_back(*item);
    }
    return synData;
}
*/
