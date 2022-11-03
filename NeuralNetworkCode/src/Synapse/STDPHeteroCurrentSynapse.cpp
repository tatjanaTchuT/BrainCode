//
// Created by Saif Ahmed on 22.09.21.
//

#include "STDPHeteroCurrentSynapse.hpp"

#include <memory>

STDPHeteroCurrentSynapse::STDPHeteroCurrentSynapse(NeuronPop *postNeurons, NeuronPop *preNeurons, GlobalSimInfo *info)
        : HeteroSynapse(postNeurons, preNeurons, info), weightNormalization(NOPNormalization) {
}

void STDPHeteroCurrentSynapse::prepareSynapses() {
    auto* heteroPostNeuronPop = dynamic_cast<HeteroNeuronPop*>(this->neuronsPost);
    double currentTime = (double) this->info->time_step * this->info->dt;

    // Whenever a new post spike arrives, all synapses must be marked so they are available for depression due to post-pre interactions (only the 1st following pre)
    // TODO: Task - Implement SharedSynapticState mechanism for all Heterosynapse using name post population
    for (unsigned long postId = 0; postId < GetNoNeuronsPost(); ++postId) {
        if (std::abs(heteroPostNeuronPop->getLastNeuronalSpikeTime(postId) - currentTime) < 1.5 * this->info->dt) {
            for (auto& synExt: synapseExtensions.at(postId)) {
                synExt->markForLTD();
            }
        }
    }
}

void STDPHeteroCurrentSynapse::updateCooperativityAmongSynapses() {
    for (unsigned long postId = 0; postId < GetNoNeuronsPost(); ++postId) {

        // TODO: Task - Implement SharedSynapticState mechanism for all Heterosynapse using name post population
        auto* heteroPostNeuronPop = dynamic_cast<HeteroNeuronPop*>(this->neuronsPost);
        const std::unordered_set<std::shared_ptr<SynapseExtension>>& spikerSynExts = heteroPostNeuronPop->getSpikerSynapses(postId);
        std::vector<std::shared_ptr<SynapseExtension>>& synapseExts = this->synapseExtensions.at(postId);

        // decay of cooperativity (thetas)
        for (auto& synExt: synapseExts) {
            synExt->theta *= std::exp(-this->info->dt / this->tauTheta);
        }

        // update of cooperativity (thetas) due to spiker synapses
        for (const auto& spiker: spikerSynExts) {
            for (const auto& neighbor: synapseExts) {
                if (spiker != neighbor) {
                    double weightProd = spiker->weight * neighbor->weight;
                    double cooperation = weightProd * heteroDistanceEffect(spiker.get(), neighbor.get()) * heteroTimingEffect(spiker.get(), neighbor.get());

                    spiker->theta += cooperation;
                    // avoids double counting of cooperation
                    if (spikerSynExts.find(neighbor) != spikerSynExts.end()) {
                        neighbor->theta += cooperation;
                    }
                }
            }
        }
    }
}

void STDPHeteroCurrentSynapse::advectSynapses() {
    for (unsigned long postId = 0; postId < GetNoNeuronsPost(); ++postId) {

        // TODO: Task - Implement SharedSynapticState mechanism for all Heterosynapse using name post population
        auto* heteroPostNeuronPop = dynamic_cast<HeteroNeuronPop*>(this->neuronsPost);
        const std::unordered_set<std::shared_ptr<SynapseExtension>>& spikerSynExts = heteroPostNeuronPop->getSpikerSynapses(postId);
        std::vector<std::shared_ptr<SynapseExtension>>& synapseExts = this->synapseExtensions.at(postId);

        double weightChange;
        double currentTime = (double) this->info->time_step * this->info->dt;

        double lastNeuronalSpikeTime = heteroPostNeuronPop->getLastNeuronalSpikeTime(postId);

        // Timing-Dependent Potentiation
        if (std::abs(lastNeuronalSpikeTime - currentTime) < 1.5 * this->info->dt) {

            for (auto& synExt: synapseExts) {
                if (synExt->availableForLTP()) {
                    weightChange = this->preFactorPotentiation *
                                   this->heteroPreFactorLTP(synExt.get()) *
                                   this->kernelPotentiation(lastNeuronalSpikeTime - synExt->lastSpike);
                    synExt->weight += weightChange;
                }
                synExt->unmarkForLTP();
            }
        }

        // Timing-Dependent Depression
        for (auto& spikerSynExt: spikerSynExts) {
            if (spikerSynExt->availableForLTD()) {
                weightChange = this->preFactorDepression *
                               this->heteroPreFactorLTD(spikerSynExt.get()) *
                               this->kernelDepression(lastNeuronalSpikeTime - spikerSynExt->lastSpike);
                spikerSynExt->weight += weightChange;
            }
            spikerSynExt->unmarkForLTD();
        }
    }
}

void STDPHeteroCurrentSynapse::finalizeSynapses() {
    if (this->weightNormalization == HardNormalization) {
        for (unsigned long postId = 0; postId < GetNoNeuronsPost(); ++postId) {
            std::vector<std::shared_ptr<SynapseExtension>>& synapseExts = this->synapseExtensions.at(postId);
            for (auto& syn: synapseExts) {
                syn->weight = std::max(MIN_WEIGHT, std::min(MAX_WEIGHT, syn->weight));
            }
        }
    }
}

void STDPHeteroCurrentSynapse::advect_spikers(std::vector<double>* currents, long spiker) {
    auto* heteroConnectivity = dynamic_cast<HeteroConnectivity*>(this->geometry);
    std::vector<std::pair<unsigned long, unsigned long>> targetList = heteroConnectivity->getSynapticTargetList(spiker);
    auto* heteroNeuronsPost = dynamic_cast<HeteroNeuronPop*>(this->neuronsPost);

    double couplingStrength;
    double current;
    unsigned long postNeuronId;
    unsigned long synapseId;
    std::shared_ptr<SynapseExtension> currentSynapseExt;

    std::pair<unsigned long, unsigned long> neuronSynapsePair;
    for (int i = 0; i < targetList.size(); i++) {
        neuronSynapsePair = targetList.at(i);
        postNeuronId = neuronSynapsePair.first;
        synapseId = neuronSynapsePair.second;

        couplingStrength = GetCouplingStrength(spiker, i); // i is used as "postId" because of how SetDistributionJ is implemented in Connectivity.cpp
        if (couplingStrength < 0.0) {
            current = couplingStrength;
        } else {
            currentSynapseExt = this->synapseExtensions.at(postNeuronId).at(synapseId);
            // mark synapse as a spiker synapse and notify HeteroNeuronPop
            // TODO: Task - Implement SharedSynapticState mechanism for all Heterosynapse using name post population
            currentSynapseExt->updateSpikeTimeAndMarkForLTP((double) this->info->time_step * this->info->dt);
            heteroNeuronsPost->recordSpikerSynapse(postNeuronId, currentSynapseExt);

            current = couplingStrength * currentSynapseExt->weight;
        }
        currents->at(i) += current;
        this->cumulatedDV += current;
    }
}

double STDPHeteroCurrentSynapse::heteroDistanceEffect(SynapseExtension* synExtA, SynapseExtension* synExtB) const {
    return std::exp(-std::abs(synExtA->distToSoma - synExtB->distToSoma) / this->lambdaDist);
}

double STDPHeteroCurrentSynapse::heteroTimingEffect(SynapseExtension* synExtA, SynapseExtension* synExtB) const {
    double lastSpikeSynA = synExtA->lastSpike, lastSpikeSynB = synExtB->lastSpike;
    return (lastSpikeSynA < 0.0 || lastSpikeSynB < 0.0) ? 0.0 : std::exp(-std::abs(lastSpikeSynA - lastSpikeSynB) / this->tauDelay);
}

double STDPHeteroCurrentSynapse::kernelPotentiation(double deltaT) {
    return this->kernel->responseLTP(deltaT);
}

double STDPHeteroCurrentSynapse::kernelDepression(double deltaT) {
    return this->kernel->responseLTD(deltaT);
}

double STDPHeteroCurrentSynapse::heteroPreFactorLTP(SynapseExtension* synExt) const {
    return this->baseLineLTP + this->incrLTP * (1 - std::exp(-this->alphaPotentiation * synExt->theta));
}

double STDPHeteroCurrentSynapse::heteroPreFactorLTD(SynapseExtension* synExt) const {
    return this->baseLineLTD - this->decrLTD * (1 - std::exp(-this->betaDepression * synExt->theta));
}

void STDPHeteroCurrentSynapse::SaveParameters(std::ofstream *stream, std::string id_str) {
    HeteroSynapse::SaveParameters(stream, id_str);

    *stream << id_str << "intersynapse_distance_decay\t" << this->lambdaDist << "\tmicrometer\n";
    *stream << id_str << "intersynapse_spike_timing_decay\t" << this->tauDelay << "\ts\n";
    *stream << id_str << "pre_factor_ltp\t" << this->preFactorPotentiation << "\n";
    *stream << id_str << "pre_factor_ltd\t" << this->preFactorDepression << "\n";
    *stream << id_str << "alpha_LTP\t" << this->alphaPotentiation << "\n";
    *stream << id_str << "beta_LTD\t" << this->betaDepression << "\n";
    *stream << id_str << "base_LTP\t" << this->baseLineLTP << "\n";
    *stream << id_str << "base_LTD\t" << this->baseLineLTD << "\n";
    *stream << id_str << "incr_LTP\t" << this->incrLTP << "\n";
    *stream << id_str << "decr_LTD\t" << this->decrLTD << "\n";

    this->kernel->save(stream, id_str);

    if (this->weightNormalization == HardNormalization) {
        *stream << id_str << "weight_normalization\t" << str_HardNormalization << "\n";
    } else {
        *stream << id_str << "weight_normalization\t" << str_NOPNormalization << "\n";
    }

}

void STDPHeteroCurrentSynapse::LoadParameters(std::vector<std::string> *input) {
    HeteroSynapse::LoadParameters(input);

    std::string name;
    std::vector<std::string> values;

    double tauLTP, tauLTD, muLTP, sigmaLTP, muLTD, sigmaLTD;
    std::string kernelChoice;

    bool lambdaDistInitialized = false,
         tauDelayInitialized = false,
         preFactorPotentiationInitialized = false,
         preFactorDepressionInitialized = false,
         alphaInitialized = false,
         betaInitialized = false,
         baselineLTPInitialized = false,
         incrLTPInitialized = false,
         baselineLTDInitialized = false,
         decrLTDInitialized = false,
         kernelSelected = false,
         tauLTPInitialized = false,
         tauLTDInitialized = false,
         muLTPInitialized = false,
         sigmaLTPInitialized = false,
         muLTDInitialized = false,
         sigmaLTDInitialized = false;

    for (auto & it : *input) {
        SplitString(&it,&name,&values);

        if (name.find("intersynapse_distance_decay") != std::string::npos){
            this->lambdaDist = std::stod(values.at(0));
            lambdaDistInitialized = true;
        } else if (name.find("intersynapse_spike_timing_decay") != std::string::npos) {
            this->tauDelay = std::stod(values.at(0));
            tauDelayInitialized = true;
        } else if (name.find("pre_factor_ltp") != std::string::npos) {
            this->preFactorPotentiation = std::stod(values.at(0));
            preFactorPotentiationInitialized = true;
        } else if (name.find("pre_factor_ltd") != std::string::npos) {
            this->preFactorDepression = std::stod(values.at(0));
            preFactorDepressionInitialized = true;
        } else if (name.find("alpha_LTP") != std::string::npos) {
            this->alphaPotentiation = std::stod(values.at(0));
            alphaInitialized = true;
        } else if (name.find("beta_LTD") != std::string::npos) {
            this->betaDepression = std::stod(values.at(0));
            betaInitialized = true;
        } else if (name.find("base_LTP") != std::string::npos) {
            this->baseLineLTP = std::stod(values.at(0));
            baselineLTPInitialized = true;
        } else if (name.find("base_LTD") != std::string::npos) {
            this->baseLineLTD = std::stod(values.at(0));
            baselineLTDInitialized = true;
        } else if (name.find("incr_LTP") != std::string::npos) {
            this->incrLTP = std::stod(values.at(0));
            incrLTPInitialized = true;
        } else if (name.find("decr_LTD") != std::string::npos) {
            this->decrLTD = std::stod(values.at(0));
            decrLTDInitialized = true;
        } else if (name.find("kernel") != std::string::npos) {
            if (values.at(0) == str_ExponentialSTDPKernel) {
                kernelChoice = str_ExponentialSTDPKernel;
                kernelSelected = true;
            } else if (values.at(0) == str_GaussianSTDPKernel) {
                kernelChoice = str_GaussianSTDPKernel;
                kernelSelected = true;
            }
        } else if (name.find("sigma_LTP") != std::string::npos) {
            sigmaLTP = std::stod(values.at(0));
            sigmaLTPInitialized = true;
        } else if (name.find("mu_LTP") != std::string::npos) {
            muLTP = std::stod(values.at(0));
            muLTPInitialized = true;
        } else if (name.find("sigma_LTD") != std::string::npos) {
            sigmaLTD = std::stod(values.at(0));
            sigmaLTDInitialized = true;
        } else if (name.find("mu_LTD") != std::string::npos) {
            muLTD = std::stod(values.at(0));
            muLTDInitialized = true;
        } else if (name.find("tau_LTP") != std::string::npos) {
            tauLTP = std::stod(values.at(0));
            tauLTPInitialized = true;
        } else if (name.find("tau_LTD") != std::string::npos) {
            tauLTD = std::stod(values.at(0));
            tauLTDInitialized = true;
        } else if (name.find("weight_normalization") != std::string::npos) {
            if (values.at(0) == str_HardNormalization) {
                this->weightNormalization = HardNormalization;
            }
        }
    }

    assertm(lambdaDistInitialized == true, "Using STDPHetero synapses without specifying intersynapse_distance_decay is not allowed.");
    assertm(tauDelayInitialized == true, "Using STDPHetero synapses without specifying intersynapse_spike_timing_decay is not allowed.");
    assertm(preFactorPotentiationInitialized == true, "Using STDPHetero synapses without specifying pre_factor_ltp is not allowed.");
    assertm(preFactorDepressionInitialized == true, "Using STDPHetero synapses without specifying pre_factor_ltd is not allowed.");
    assertm(alphaInitialized == true, "Using STDPHetero synapses without specifying alpha_LTP is not allowed.");
    assertm(betaInitialized == true, "Using STDPHetero synapses without specifying beta_LTD is not allowed.");
    assertm(baselineLTPInitialized == true, "Using STDPHetero synapses without specifying baseline_LTP is not allowed.");
    assertm(baselineLTDInitialized == true, "Using STDPHetero synapses without specifying baseline_LTD is not allowed.");
    assertm(incrLTPInitialized == true, "Using STDPHetero synapses without specifying incr_LTP is not allowed.");
    assertm(decrLTDInitialized == true, "Using STDPHetero synapses without specifying decr_LTD is not allowed.");
    assertm(kernelSelected == true, "Using STDPHetero synapses without specifying STDP kernel is not allowed.");
    if (kernelChoice == str_ExponentialSTDPKernel) {
        assertm((tauLTPInitialized & tauLTDInitialized) == true,
                "Using Exponential STDP Kernel without specifying tau_LTP/tau_LTD is not allowed.");
        this->kernel = std::make_unique<ExponentialKernel>(tauLTP, tauLTD);
    }
    if (kernelChoice == str_GaussianSTDPKernel) {
        assertm((sigmaLTPInitialized & muLTPInitialized & sigmaLTDInitialized & muLTDInitialized) == true ,
                "Using Gaussian STDP Kernel without specifying sigma_LTP/mu_LTP/sigma_LTD/mu_LTD is not allowed.");
        this->kernel = std::make_unique<GaussianKernel>(muLTP, sigmaLTP, muLTD, sigmaLTD);
    }

    if (this->weightNormalization == NOPNormalization) {
        std::wcout << "Heterosynapses have been initiaized with no NOP Normalization.";
    }

}

int STDPHeteroCurrentSynapse::GetNumberOfDataColumns() {
    return 0;
}

std::string STDPHeteroCurrentSynapse::GetDataHeader(int data_column)
{
    return "#" + std::to_string(data_column) + " J_"+ GetIdStr() + " (mV)\n";
}

std::string STDPHeteroCurrentSynapse::GetUnhashedDataHeader()
{
    return "J_" + GetIdStr() + "\t";
}

std::valarray<double> STDPHeteroCurrentSynapse::GetSynapticState(int pre_neuron)
{
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

std::string STDPHeteroCurrentSynapse::GetTypeStr() {
    return str_STDPHeteroCurrentSynapse;
}