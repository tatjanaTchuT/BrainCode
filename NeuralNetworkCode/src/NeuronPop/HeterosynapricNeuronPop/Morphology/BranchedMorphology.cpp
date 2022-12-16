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

void BranchedMorphology::SaveParameters(std::ofstream *stream, std::string neuronPreId) {
    Morphology::SaveParameters(stream, neuronPreId);

    *stream << neuronPreId<<"_branch_length\t\t\t"<<std::to_string(this->branchLength)<<" #μm";
    *stream << "\t"<<"#Length of each branch.\n";

    *stream << neuronPreId<<"_synaptic_gap\t\t\t\t"<<std::to_string(this->synapticGap)<<" #μm";
    *stream << "\t"<<"#Unitary distance between synapse slots.\n";
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
            this->synapticGap = std::stod(values.at(0));
            synapticGapInitialized = true;
        } else if (name.find("distribute_weights") != std::string::npos) {
            //This whole part is experimental, it seems it was not completely tested
            //As such, this is deprecated from publication
            if (values.at(0) == "true") {
                distributeWeights = true;
            } else if (values.at(0) == "step") {
                stepWeights = true;
                int cIdx = 1;
                try {
                    int weightSteps = std::stoi(values.at(cIdx++));
                    for (int k = 0; k < weightSteps; k++) {
                        this->weightStepBoundary.push_back(std::stoul(values.at(cIdx++)));
                        this->weightStepValue.push_back(std::stod(values.at(cIdx++)));
                    }
                    this->currWightStepId = 0;
                } catch (...) {
                    bool x =false;
                    assertm(x, "Issues with step weights..");
                }
            } else {
                try {
                    this->initialWeights = std::stod(values.at(1));
                } catch (...) {
                }
            }
        }
    }

    assertm(dendriteInitialized, "Using heterosynaptic synapses without specifying dendritic_length is not allowed.");
    assertm(synapticGapInitialized, "Using heterosynaptic synapses without specifying synaptic_gap is not allowed.");

    this->synapseIdGenerator = 0;
}

double BranchedMorphology::generateSynapticWeight(){
    double weight{};
    std::uniform_real_distribution<double> distribution(0.0,2.0);
            if (stepWeights) {
            if (synapseData.size() > weightStepBoundary.at(currWightStepId)) {
                currWightStepId++;}

            weight = weightStepValue.at(currWightStepId);
            } 
            else {
            if (distributeWeights) {
                std::random_device rd;
                std::default_random_engine generator(rd()); // rd() provides a random seed
                weight = distribution(generator);
            } else {
                weight = this->initialWeights; // assuming a range of weight between 0 and 2, weight is initialized to midpoint: 1
            }
        }
        this->weightsSum += weight;
        return weight;
}

std::valarray<double> BranchedMorphology::getIndividualSynapticProfile(unsigned long synapseId) const {
    /*
     * returned array organised as follows:
     * item 1: distance of synapse from soma
     * item 2: value of heterosynaptic cooperativity
     * item 3: value of the synaptic weight
     * item 4: last spike time of the synapse
     * */
    SynapseExt syn = *synapseData.at(synapseId);
    std::valarray<double> ret(3);
    ret[0] = syn.distance_from_root;
    ret[1] = syn.weight;
    ret[2] = syn.lastSpike;
    return ret;
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
