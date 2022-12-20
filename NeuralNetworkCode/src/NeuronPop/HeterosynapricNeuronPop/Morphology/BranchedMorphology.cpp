#include "BranchedMorphology.hpp"

Branch::Branch(size_t synSlots, int gap, int branch_length):openSynapsesSlots(synSlots), spikedSyn(synSlots), synapticGap{gap}, branchLength{branch_length}{
    
}

BranchedMorphology::BranchedMorphology(GlobalSimInfo * info): Morphology(info){

}

void BranchedMorphology::recordPostSpike() {
    Morphology::recordPostSpike();
    std::fill(this->integratePostSpike.begin(), this->integratePostSpike.end(), true);
    this->postSpiked = true;
}

void BranchedMorphology::recordExcitatoryPreSpike(unsigned long synSpikerId) {
    Morphology::recordExcitatoryPreSpike(synSpikerId);
    this->integratePreSpike.at(synSpikerId) = true;
}

void BranchedMorphology::LoadParameters(std::vector<std::string> *input) {
    Morphology::LoadParameters(input);

    std::string name;
    std::vector<std::string> values;

    bool dendriteInitialized = false,
        synapticGapInitialized = false;

    for (auto & it : *input) {
        SplitString(&it,&name,&values);

        if(name.find("branch_length") != std::string::npos){
            this->branchLength = std::stoi(values.at(0));
            dendriteInitialized = true;
        } else if (name.find("synaptic_gap") != std::string::npos) {
            this->synapticGap = std::stoi(values.at(0));
            synapticGapInitialized = true;
        } else if (name.find("distribute_weights") != std::string::npos) {
            //This whole part is experimental, it seems it was not completely tested
            //As such, this is deprecated from publication
            if (values.at(0) == "true") {
                distributeWeights = true;
            } else {
                    this->initialWeights = std::stod(values.at(1));
            }
        }
    }

    assertm(dendriteInitialized, "Using heterosynaptic synapses without specifying dendritic_length is not allowed.");
    assertm(synapticGapInitialized, "Using heterosynaptic synapses without specifying synaptic_gap is not allowed.");

}

void BranchedMorphology::SaveParameters(std::ofstream *stream, std::string neuronPreId) {
    Morphology::SaveParameters(stream, neuronPreId);

    *stream << neuronPreId<<"_morphology_branch_length\t\t"<<std::to_string(this->branchLength)<<" #μm";
    *stream << "\t"<<"#Length of each branch.\n";

    *stream << neuronPreId<<"_morphology_synaptic_gap\t\t\t"<<std::to_string(this->synapticGap)<<" #μm";
    *stream << "\t"<<"#Unitary distance between synapse slots.\n";

    *stream << neuronPreId<<"_morphology_distribute_weights\t\t";
    if (this->distributeWeights){
        *stream << "true\t";
    }else{
        *stream<<"false\t"<<std::to_string(this->initialWeights);
    }
    *stream << "\t"<<"#The bool corresponds to distributing weight between min and max uniformally. The number will be the weight assigned to all synapses if bool is false (do not confuse with implementation in MonoDendriteSTDP).\n";
}


double BranchedMorphology::generateSynapticWeight(){
    double weight{};
    std::uniform_real_distribution<double> distribution(this->minWeight,this->maxWeight);
            if (distributeWeights) {
                std::random_device rd;
                std::default_random_engine generator(rd()); // rd() provides a random seed
                weight = distribution(generator);
            } else {
                weight = this->initialWeights; // assuming a range of weight between 0 and 2, weight is initialized to midpoint: 1
            }
        this->weightsSum += weight;
        return weight;
}

std::valarray<double> BranchedMorphology::getIndividualSynapticProfile(unsigned long synapseId) const {
    /*
     * returned array organised as follows:
     * item 1: distance of synapse from branch root
     * item 2: value of the synaptic weight
     * item 3: last spike time of the synapse
     * */
    return synapseData.at(synapseId)->getIndividualSynapticProfile();
}

std::valarray<double> BranchedMorphology::getOverallSynapticProfile() const {
    /*
     * returned array organised as follows:
     * item 1: average synaptic weight
     * item 2: total post spikes
     * item 3: total pre spikes
     * */
    std::valarray<double> ret(3);
//    double weightSum = 0;
//    for (unsigned long i = 0; i < 1000; i++) {
//        weightSum += synapseData.at(i).get()->weight;
//    }

    double weightSum = std::accumulate(this->synapseData.begin(), this->synapseData.end(), 0.0,
                                       [] (const double acc, const std::shared_ptr<SynapseExt>& syn) { return acc + syn->weight; });

    ret[0] = weightSum / this->synapseData.size();
    ret[1] = this->totalPostSpikes;
    ret[2] = this->totalPreSpikes;
    return ret;
}
void BranchedMorphology::SetUpBranch (int branch_id){

}