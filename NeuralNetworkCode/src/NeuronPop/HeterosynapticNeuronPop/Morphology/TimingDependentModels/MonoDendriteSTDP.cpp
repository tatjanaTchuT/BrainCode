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

    this->thetaDecay();

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
            if (syn->getLastSpike() > 0 && this->integratePreSpike.at(syn->getIdInMorpho())) {
                this->updateLTP(syn->getIdInMorpho());
                this->integratePreSpike.at(syn->getIdInMorpho()) = false;
            }
        }
        this->postSpiked=false;
    }

    this->reset();
}

void MonoDendriteSTDP::thetaDecay() {
    for (const std::shared_ptr<SynapseSpine>& syn: this->synapseData) {
        syn->setTheta(syn->getTheta() * exp(-this->info->dt/this->tauTheta));
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

void MonoDendriteSTDP::SaveParameters(std::ofstream *stream, std::string neuronPreId) {
    Morphology::SaveParameters(stream, neuronPreId);

    *stream << neuronPreId<<"_dendritic_length\t\t\t"<<std::to_string(this->dendriticLength);
    *stream << "\t"<<"#Length of the dendritic arm (μm).\n";

    *stream << neuronPreId<<"_synaptic_gap\t\t\t\t"<<std::to_string(this->synapticGap);
    *stream << "\t"<<"#Forced distance between synapses (μm).\n";

    *stream << neuronPreId<<"_heterosynaptic_theta_decay\t"<<std::to_string(this->tauTheta);
    *stream << "\t"<<"#Or tauTheta, decay constant of heterosynaptic effects in spines.\n";

    *stream << neuronPreId<<"_intersynapse_distance_decay\t"<<std::to_string(this->lambdaDist);
    *stream << "\t"<<"#Or lambdaDist, spatial decay constant of heterosynaptic boost between synapses.\n";

    *stream << neuronPreId<<"_intersynapse_spike_timing_decay\t"<<std::to_string(this->tauDelay);
    *stream << "\t"<<"#Or tauDelay, decay constant of heterosynaptic effects over inter-synapse spike timing difference.\n";

    *stream << neuronPreId<<"_pre_factor_ltp\t\t\t"<<std::to_string(this->preFactorLTP);
    *stream << "\t"<<"#Base factor that is multiplied by the spatio-temporal effects in LTP. If set to zero, LTP will be zero. \"A\" equivalent\n";

    *stream << neuronPreId<<"_pre_factor_ltd\t\t\t"<<std::to_string(this->preFactorLTD);
    *stream << "\t"<<"#Base factor that is multiplied by the spatio-temporal effects in LTD. If set to zero, LTD will be zero. \"A\" equivalent\n";

    *stream << neuronPreId<<"_incr_ltp\t\t\t\t"<<std::to_string(this->incr_ltp);
    *stream << "\t"<<"#Max possible increase in LTP due to cooperativity . \"I\" equivalent\n";

    *stream << neuronPreId<<"_decr_ltd\t\t\t\t"<<std::to_string(this->decr_ltd);
    *stream << "\t"<<"#Max possible LTD due to cooperativity.  \"D\" equivalent\n";

    *stream << neuronPreId<<"_base_ltp\t\t\t\t"<<std::to_string(this->base_ltp);
    *stream << "\t"<<"#Default increase in weight per LTP check.  \"B\" equivalent\n";

    *stream << neuronPreId<<"_base_ltd\t\t\t\t"<<std::to_string(this->base_ltd);
    *stream << "\t"<<"#Default decrease of weight per LTD check. \"B\" equivalent\n";
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

std::shared_ptr<SynapseSpine> MonoDendriteSTDP::allocateNewSynapse(HeteroCurrentSynapse& synapse) {

    std::uniform_real_distribution<double> distribution(0.0,2.0);

    std::shared_ptr<SynapseSpine> newSynapse;

    if (this->nextPos < this->dendriticLength) {
        newSynapse = std::make_shared<SynapseSpine>();

//        if (this->allocateDistal) {
//            newSynapse->distToSoma = this->posHi;
//            this->posHi -= this->synapticGap;
//        } else {
//            newSynapse->distToSoma = this->posLo;
//            this->posLo += this->synapticGap;
//        }
//
//        this->allocateDistal = !this->allocateDistal;

        newSynapse->setDistToSoma( this->nextPos);
        newSynapse->setLastSpike(-200.0); // large negative value indicates no spikes of synapse during simulation
        newSynapse->setTheta(0);
        if (stepWeights) {
            if (synapseData.size() > weightStepBoundary.at(currWightStepId)) {
                currWightStepId++;
            }
            newSynapse->setWeight(weightStepValue.at(currWightStepId));
        } else {
            if (distributeWeights) {
                std::random_device rd;
                std::default_random_engine generator(rd()); // rd() provides a random seed
                newSynapse->setWeight(distribution(generator));
            } else {
                newSynapse->setWeight(this->initialWeights); // assuming a range of weight between 0 and 2, weight is initialized to midpoint: 1
            }
        }
        this->weightsSum += newSynapse->getWeight();
        newSynapse->setIdInMorpho(this->synapseIdGenerator++);
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
    SynapseSpine* spiker = this->synapseData.at(spikerId).get();
    SynapseSpine* neighbor = this->synapseData.at(neighborId).get();

    double hEffects = getDistanceEffects(spiker, neighbor);
    hEffects *= getTimingEffects(spiker, neighbor);
    hEffects *= spiker->getWeight() * neighbor->getWeight();
    spiker->addToTheta(hEffects);
    neighbor->addToTheta(hEffects);

//    if (hEffects != 0.0) {//OPTIMIZATION. Problems with heap allocation of the data (causes an OOM error). Needs to be moved to a file if necessary
////        pseudoCoop( spikerId, neighborId);
//
//        this->theta_changes.emplace_back(spikerId, hEffects);
//        this->theta_changes.emplace_back(neighborId, hEffects);
//    }


}

void MonoDendriteSTDP::pseudoCoop(unsigned long synId, unsigned long neighborId) {
    SynapseSpine* spiker = this->synapseData.at(synId).get();
    SynapseSpine* neighbor = this->synapseData.at(neighborId).get();
    std::cout << "id 1: " << synId << ", id 2: " << neighborId << std::endl;
    std::cout << "dist: " << abs(spiker->getDistToSoma() - neighbor->getDistToSoma()) << std::endl;
    std::cout << "time: " << abs(spiker->getLastSpike() - neighbor->getLastSpike()) << std::endl;
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
    return synapseData.at(synapseId)->getIndividualSynapticProfile();
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
                                       [] (const double acc, const std::shared_ptr<SynapseSpine>& syn) { return acc + syn->getWeight(); });

    ret[0] = weightSum / this->synapseData.size();
    ret[1] = this->totalPostSpikes;
    ret[2] = this->totalPreSpikes;
    return ret;
}

