#include "Morphology.hpp"

Morphology::Morphology(GlobalSimInfo *info): info(info), lastPostSpikeTime(-200), weightsSum(0), totalPostSpikes(0), totalPreSpikes(0), weightNormalization(NOPNormalization) {
}

void Morphology::recordPostSpike() {
    this->lastPostSpikeTime = this->info->dt * (double) this->info->time_step;
    this->totalPostSpikes++;
}

void Morphology::recordPreSpike(unsigned long synSpikerId) {
    this->synapseData.at(synSpikerId)->lastSpike = (double) this->info->time_step * this->info->dt;
    this->spikedSynapsesId.push_back(synSpikerId);
    this->spikedSynapses.at(synSpikerId) = true;
    this->totalPreSpikes++;
}

std::vector<unsigned long> getSpikedSynapsesFromMorphology(const Morphology& morph) {
    return morph.spikedSynapsesId;
}

unsigned long Morphology::getSynapseCount() const {
    return static_cast<unsigned long>(this->synapseData.size());
}

double Morphology::getWeight(unsigned long synapseId) const {
    return this->synapseData.at(synapseId)->weight;
}

void Morphology::reset() {
    this->normalizeWeights();
    for (unsigned long id: this->spikedSynapsesId) {
        this->spikedSynapses.at(id) = false;
    }
    this->spikedSynapsesId.clear();
}

void Morphology::normalizeWeights() {
    if (this->weightNormalization == HardNormalization) {
        this->hardNormalize();
    } else if (this->weightNormalization == SoftMaxNormalization) {
        this->softMaxNormalize();
    }
}

void Morphology::hardNormalize() {
    for (auto& syn: this->synapseData) {
        syn->weight = std::max(0.0, std::min(2.0, syn->weight));
    }
}

void Morphology::softMaxNormalize() {
    double maxWeights, sumWeights, constant;

    maxWeights = std::numeric_limits<double>::min();
    for (auto& syn: this->synapseData) {
        maxWeights = std::max(maxWeights, syn->weight);
    }

    sumWeights = 0;
    for (auto& syn: this->synapseData) {
        sumWeights += std::exp(syn->weight - maxWeights);
    }

    constant = maxWeights + std::log(sumWeights);
    for (auto& syn: this->synapseData) {
        syn->weight = exp(syn->weight - constant);
    }
}

void Morphology::SaveParameters(std::ofstream *stream) {
}

void Morphology::LoadParameters(std::vector<std::string> *input) {
    std::string name;
    std::vector<std::string> values;

    // checks for correct initialization
    bool normalizationFound = false;

    for (auto & it : *input) {
        SplitString(&it,&name,&values);

        if (name.find("weight_normalization") != std::string::npos) {
            if (values.at(0) == str_HardNormalization) {
                weightNormalization = HardNormalization;
                normalizationFound = true;
            } else if (values.at(0) == str_SoftMaxNormalization) {
                weightNormalization = SoftMaxNormalization;
                normalizationFound = true;
            }
        }
    }
    assertm(normalizationFound == true, "Please specify weight normalization mechanism for the synapses of the neurons to be used.");
}
