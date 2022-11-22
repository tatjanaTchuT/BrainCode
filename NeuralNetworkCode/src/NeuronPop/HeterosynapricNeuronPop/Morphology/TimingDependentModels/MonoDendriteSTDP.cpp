//
// Created by Saif Ahmed on 27.06.21.
//

#include "MonoDendriteSTDP.hpp"
#include <string>

MonoDendriteSTDP::MonoDendriteSTDP(GlobalSimInfo* info): Morphology(info), integratePostSpike(false) {
}

void MonoDendriteSTDP::advect() {
    this->timeDecay();

    // update cooperativity between spiker and spiker pairs -> avoids double counting when combined with loop that follows
    for (unsigned long i = 0; i < this->spikedSynapsesId.size(); ++i) {
        for (unsigned long j = i+1; j < this->spikedSynapsesId.size(); ++j) {
            this->updateCooperativity(this->spikedSynapsesId.at(i), this->spikedSynapsesId.at(j));
        }
    }

    // update cooperativity between spiker and non-spiker pairs
    for (const auto& synSpikerId : this->spikedSynapsesId) {
        for (unsigned long j = 0; j < this->synapseData.size(); ++j) {
            if (!this->spikedSynapses[j] && synSpikerId != j) {
                this->updateCooperativity(synSpikerId, j);
            }
        }
    }

    // update for post-pre effects for spiker synapses
    for (const auto& synSpikerId: this->spikedSynapsesId) {
        if (this->lastPostSpikeTime > 0 && this->integratePostSpike.at(synSpikerId)) {
            this->updateLTD(synSpikerId);
            this->integratePostSpike.at(synSpikerId) = false;

        }
    }

    // update for pre-post effects for all synapses
    if (this->postSpiked){
        for (const auto& syn: this->synapseData) {
            if (syn->lastSpike > 0 && this->integratePreSpike.at(syn->globalId)) {
                this->updateLTP(syn->globalId);
                this->integratePreSpike.at(syn->globalId) = false;
            }
        }
        this->postSpiked=false;
    }

    this->reset();
}

void MonoDendriteSTDP::recordPostSpike() {
    Morphology::recordPostSpike();
    std::fill(this->integratePostSpike.begin(), this->integratePostSpike.end(), true);
    this->postSpiked = true;
}

void MonoDendriteSTDP::recordPreSpike(unsigned long synSpikerId) {
    Morphology::recordPreSpike(synSpikerId);
    this->integratePreSpike.at(synSpikerId) = true;
}

void MonoDendriteSTDP::timeDecay() {
    for (const std::shared_ptr<SynapseExt>& syn: this->synapseData) {
        syn->theta *= exp(-this->info->dt/this->tauTheta);
    }
}

void MonoDendriteSTDP::SaveParameters(std::ofstream *stream) {
    Morphology::SaveParameters(stream);
}

void MonoDendriteSTDP::LoadParameters(std::vector<std::string> *input) {
    Morphology::LoadParameters(input);

    std::string name;
    std::vector<std::string> values;

    bool dendriteInitialized = false,
         synapticGapInitialized = false,
         tauThetaInitialized = false,
         lambdaDistInitialized = false,
         tauDelayInitialized = false;

    this->preFactorLTP = 1.0;
    this->preFactorLTD = 1.0;

    for (auto & it : *input) {
        SplitString(&it,&name,&values);

        if(name.find("dendritic_length") != std::string::npos){
            this->dendriticLength = std::stod(values.at(0));
            dendriteInitialized = true;
        } else if (name.find("synaptic_gap") != std::string::npos) {
            this->synapticGap = std::stod(values.at(0));
            synapticGapInitialized = true;
        } else if (name.find("heterosynaptic_theta_decay") != std::string::npos) {
            this->tauTheta = std::stod(values.at(0));
            tauThetaInitialized = true;
        } else if (name.find("intersynapse_distance_decay") != std::string::npos){
            this->lambdaDist = std::stod(values.at(0));
            lambdaDistInitialized = true;
        } else if (name.find("intersynapse_spike_timing_decay") != std::string::npos) {
            this->tauDelay = std::stod(values.at(0));
            tauDelayInitialized = true;
        } else if (name.find("distribute_weights") != std::string::npos) {
            if (values.at(0) == "true") {
                distributeWeights = true;
            }
        } else if (name.find("pre_factor_ltp") != std::string::npos) {
            this->preFactorLTP = std::stod(values.at(0));
        } else if (name.find("pre_factor_ltd") != std::string::npos) {
            this->preFactorLTD = std::stod(values.at(0));
        }
    }

    assertm(dendriteInitialized == true, "Using heterosynaptic synapses without specifying dendritic_length is not allowed.");
    assertm(synapticGapInitialized == true, "Using heterosynaptic synapses without specifying synaptic_gap is not allowed.");

    // TODO: Update assert messages to appropriate
    assertm(tauThetaInitialized == true, "Using heterosynaptic synapses without specifying dendritic_length is not allowed.");
    assertm(lambdaDistInitialized == true, "Using heterosynaptic synapses without specifying synaptic_gap is not allowed.");
    assertm(tauDelayInitialized == true, "Using heterosynaptic synapses without specifying synaptic_gap is not allowed.");


//    this->posLo = this->synapticGap;
//    this->posHi = this->dendriticLength - this->synapticGap;
//    this->allocateDistal = false;
    this->nextPos =  this->synapticGap;
    this->synapseIdGenerator = 0;
}

std::shared_ptr<SynapseExt> MonoDendriteSTDP::allocateNewSynapse() {

    std::uniform_real_distribution<double> distribution(0.0,2.0);

    std::shared_ptr<SynapseExt> newSynapse;

    if (this->nextPos < this->dendriticLength) {
        newSynapse = std::make_shared<SynapseExt>();

//        if (this->allocateDistal) {
//            newSynapse->distToSoma = this->posHi;
//            this->posHi -= this->synapticGap;
//        } else {
//            newSynapse->distToSoma = this->posLo;
//            this->posLo += this->synapticGap;
//        }
//
//        this->allocateDistal = !this->allocateDistal;

        newSynapse->distToSoma = this->nextPos;
        newSynapse->lastSpike = -200.0; // large negative value indicates no spikes of synapse during simulation
        newSynapse->theta = 0;
        if (distributeWeights) {
            std::random_device rd; //Probably wise to have a general random engine that is not initialized per call
            std::default_random_engine generator(rd()); // rd() provides a random seed
            newSynapse->weight = distribution(generator);
        } else {
            newSynapse->weight = 1; // assuming a range of weight between 0 and 2, weight is initialized to midpoint: 1
        }
        this->weightsSum += newSynapse->weight;
        newSynapse->globalId = this->synapseIdGenerator++;
        // newSynapse->postNeuronId = ? // set in the Synapse object that calls for a new synapse
        // newSynapse->preNeuronId = ? // set in the Synapse object that calls for a new synapse

        this->synapseData.push_back(newSynapse);
        this->nextPos += this->synapticGap; //Not random, set

        this->spikedSynapses.push_back(false);
        this->integratePostSpike.push_back(false);
        this->integratePreSpike.push_back(false);
    }

    return newSynapse;
}

void MonoDendriteSTDP::updateCooperativity(unsigned long spikerId, unsigned long neighborId) {
    SynapseExt* spiker = this->synapseData.at(spikerId).get();
    SynapseExt* neighbor = this->synapseData.at(neighborId).get();

    double hEffects = getDistanceEffects(spiker, neighbor);
    hEffects *= getTimingEffects(spiker, neighbor);//Makes no sense
    spiker->theta += hEffects;
    neighbor->theta += hEffects;
}

std::valarray<double> MonoDendriteSTDP::getIndividualSynapticProfile(unsigned long synapseId) const {
    /*
     * returned array organised as follows:
     * item 1: distance of synapse from soma
     * item 2: value of heterosynaptic cooperativity
     * item 3: value of the synaptic weight
     * item 4: last spike time of the synapse
     * */
    SynapseExt syn = *synapseData.at(synapseId);
    std::valarray<double> ret(4);
    ret[0] = syn.distToSoma;
    ret[1] = syn.theta;
    ret[2] = syn.weight;
    ret[3] = syn.lastSpike;
    return ret;
}

std::valarray<double> MonoDendriteSTDP::getOverallSynapticProfile() const {
    /*
     * returned array organised as follows:
     * item 1: average synaptic weight
     * item 2: total post spikes
     * item 3: total pre spikes
     * */
    std::valarray<double> ret(3);
    ret[0] = this->weightsSum / this->synapseData.size();
    ret[1] = this->totalPostSpikes;
    ret[2] = this->totalPreSpikes;
    return ret;
}
