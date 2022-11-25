//
// Created by Saif Ahmed on 27.06.21.
//

#include "MonoDendriteSTDP.hpp"
#include <string>
#include <numeric>

MonoDendriteSTDP::MonoDendriteSTDP(GlobalSimInfo* info): Morphology(info), integratePostSpike(false), initialWeights(1.0), stepWeights(false) {
}

void MonoDendriteSTDP::advect() {

    Morphology::advect();

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

void MonoDendriteSTDP::timeDecay() {
    for (const std::shared_ptr<SynapseExt>& syn: this->synapseData) {
        syn->theta *= exp(-this->info->dt/this->tauTheta);
    }
}

void MonoDendriteSTDP::recordPostSpike() {
    Morphology::recordPostSpike();
    std::fill(this->integratePostSpike.begin(), this->integratePostSpike.end(), true);
    this->postSpiked = true;
}

void MonoDendriteSTDP::recordExcitatoryPreSpike(unsigned long synSpikerId) {
    Morphology::recordExcitatoryPreSpike(synSpikerId);
    this->integratePreSpike.at(synSpikerId) = true;
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
         tauDelayInitialized = false,
         base_ltpInitialized = false,
         incr_ltpInitialized = false,
         base_ltdInitialized = false,
         decr_ltdInitialized = false;


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
                    assertm(x == true, "Issues with step weights..");
                }
            } else {
                try {
                    this->initialWeights = std::stod(values.at(1));
                } catch (...) {
                }
            }
        } else if (name.find("pre_factor_ltp") != std::string::npos) {
            this->preFactorLTP = std::stod(values.at(0));
        } else if (name.find("pre_factor_ltd") != std::string::npos) {
            this->preFactorLTD = std::stod(values.at(0));
        } else if (name.find("base_ltp") != std::string::npos) {
            this->base_ltp = std::stod(values.at(0));
            base_ltpInitialized = true;
        } else if (name.find("base_ltd") != std::string::npos) {
            this->base_ltd = std::stod(values.at(0));
            base_ltdInitialized = true;
        } else if (name.find("incr_ltp") != std::string::npos) {
            this->incr_ltp = std::stod(values.at(0));
            incr_ltpInitialized = true;
        } else if (name.find("decr_ltd") != std::string::npos) {
            this->decr_ltd = std::stod(values.at(0));
            decr_ltdInitialized = true;
        }

    }

    assertm(dendriteInitialized == true, "Using heterosynaptic synapses without specifying dendritic_length is not allowed.");
    assertm(synapticGapInitialized == true, "Using heterosynaptic synapses without specifying synaptic_gap is not allowed.");

    // TODO: Update assert messages to appropriate
    assertm(tauThetaInitialized == true, "Using heterosynaptic synapses without specifying dendritic_length is not allowed.");
    assertm(lambdaDistInitialized == true, "Using heterosynaptic synapses without specifying synaptic_gap is not allowed.");
    assertm(tauDelayInitialized == true, "Using heterosynaptic synapses without specifying synaptic_gap is not allowed.");
    assertm(base_ltdInitialized == true && base_ltpInitialized == true && incr_ltpInitialized == true && decr_ltdInitialized == true,
            "some of the incr/decr ltp/ltd params not set");


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
        if (stepWeights) {
            if (synapseData.size() > weightStepBoundary.at(currWightStepId)) {
                currWightStepId++;
            }
            newSynapse->weight = weightStepValue.at(currWightStepId);
        } else {
            if (distributeWeights) {
                std::random_device rd;
                std::default_random_engine generator(rd()); // rd() provides a random seed
                newSynapse->weight = distribution(generator);
            } else {
                newSynapse->weight = this->initialWeights; // assuming a range of weight between 0 and 2, weight is initialized to midpoint: 1
            }
        }
        this->weightsSum += newSynapse->weight;
        newSynapse->globalId = this->synapseIdGenerator++;
        // newSynapse->postNeuronId = ? // set in the Synapse object that calls for a new synapse
        // newSynapse->preNeuronId = ? // set in the Synapse object that calls for a new synapse

        this->synapseData.push_back(newSynapse);
        this->nextPos += this->synapticGap;

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
    hEffects *= getTimingEffects(spiker, neighbor);
    hEffects *= spiker->weight * neighbor->weight;
    spiker->theta += hEffects;
    neighbor->theta += hEffects;

    if (hEffects != 0.0) {
//        pseudoCoop( spikerId, neighborId);

        this->theta_changes.emplace_back(spikerId, hEffects);
        this->theta_changes.emplace_back(neighborId, hEffects);
    }


}

void MonoDendriteSTDP::pseudoCoop(unsigned long synId, unsigned long neighborId) {
    SynapseExt* spiker = this->synapseData.at(synId).get();
    SynapseExt* neighbor = this->synapseData.at(neighborId).get();
    std::cout << "id 1: " << synId << ", id 2: " << neighborId << std::endl;
    std::cout << "dist: " << abs(spiker->distToSoma - neighbor->distToSoma) << std::endl;
    std::cout << "time: " << abs(spiker->lastSpike - neighbor->lastSpike) << std::endl;
    std::cout << "dist effect: " << getDistanceEffects(spiker, neighbor);
    std::cout << "time effect: " << getTimingEffects(spiker, neighbor);

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

