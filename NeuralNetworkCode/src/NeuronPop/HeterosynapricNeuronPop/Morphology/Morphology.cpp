#include <iostream>
#include <string>
#include "Morphology.hpp"

Morphology::Morphology(GlobalSimInfo *info): info(info), lastPostSpikeTime(-200), weightsSum(0), totalPostSpikes(0), totalPreSpikes(0), weightNormalization(NOPNormalization) {
}

void Morphology::LoadParameters(std::vector<std::string>* input) {

    std::string name;
    std::vector<std::string> values;

    this->decayWeights = false;

    // checks for correct initialization
    bool normalizationFound = false;

    for (auto& it : *input) {
        SplitString(&it, &name, &values);

        if (name.find("weight_normalization") != std::string::npos) {
            if (values.at(0) == str_NOPNormalization) {
                weightNormalization = NOPNormalization;
                normalizationFound = true;
            }
            else if (values.at(0) == str_HardNormalization) {
                weightNormalization = HardNormalization;
                normalizationFound = true;
            }
            else if (values.at(0) == str_SoftMaxNormalization) {
                weightNormalization = SoftMaxNormalization;
                normalizationFound = true;
            }
        }
        else if (name.find("weight_decay") != std::string::npos) {
            this->decayWeights = true;
            this->weightDecayConstant = std::stod(values.at(0));
        }

    }
}
void Morphology::SaveParameters(std::ofstream *stream) {
}


void Morphology::advect() {
    this->timeDecay();
}

void Morphology::recordPostSpike() {
    this->lastPostSpikeTime = this->info->dt * (double) this->info->time_step;
    this->totalPostSpikes++;

    // STDP Analysis
    //this->postSpikes.push_back(this->lastPostSpikeTime);
}


void Morphology::recordExcitatoryPreSpike(unsigned long synSpikerId) {
    this->synapseData.at(synSpikerId)->lastSpike = static_cast<double> (this->info->time_step) * this->info->dt;
    this->spikedSynapsesId.push_back(synSpikerId);
    this->spikedSynapses.at(synSpikerId) = true;
    this->totalPreSpikes++;

    // STDP Analysis
    //this->preSpikes.emplace_back(synSpikerId, this->synapseData.at(synSpikerId)->lastSpike);
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
        syn->weight = std::max(0.0, std::min(2.0, syn->weight));//Is this even compatible with negative weights? OPTIMIZATION
    }
}

void Morphology::softMaxNormalize() {
    double maxWeights, sumWeights;

    maxWeights = std::numeric_limits<double>::min();
    for (auto& syn : this->synapseData) {
        maxWeights = std::max(maxWeights, syn->weight);
    }

    sumWeights = 0;
    for (auto& syn : this->synapseData) {
        sumWeights += std::exp(syn->weight - maxWeights);
    }
}

void Morphology::timeDecay() {
    double dt           = info->dt;
    double expdt        = exp(-dt/weightDecayConstant);
    if (this->decayWeights) {
        for (const std::shared_ptr<SynapseExt>& syn: this->synapseData) {
            syn->weight *= expdt;
        }
    }
}
/*
void Morphology::triggerStatOut(std::string dirPath) {
    std::ofstream preFile;
    std::ofstream postFile;

    std::ofstream thetasFile;
    std::ofstream weightsFile;

    preFile.open (dirPath + "_pres.dat", std::ofstream::out | std::ofstream::trunc);
    postFile.open (dirPath + "_posts.dat", std::ofstream::out | std::ofstream::trunc);
    thetasFile.open (dirPath + "_thetas.dat", std::ofstream::out | std::ofstream::trunc);
    weightsFile.open (dirPath + "_weights.dat", std::ofstream::out | std::ofstream::trunc);

    for (const auto& line: preSpikes) {
        preFile << line.first << ":" << line.second << std::endl;
    }

    for (const auto& line: postSpikes) {
        postFile << line << std::endl;
    }

    unsigned long count = 0;
    for (const auto& line: theta_changes) {
        if (count % 2 == 0) {
            thetasFile << "s -- ";
        }
        thetasFile << line.first << ":" << line.second << std::endl;
        count++;
    }

    for (const auto& line: weight_changes) {
        weightsFile << line.first << ":" << line.second << std::endl;
    }

    preFile.close();
    postFile.close();
    thetasFile.close();
    weightsFile.close();

}*/

void Morphology::printThetasAndWeights() {
    std::cout << "weights: " << std::endl;
    for (auto& syn: this->synapseData) {
        std::cout << syn->weight << ", ";
    }
    std::cout << std::endl;

    std::cout << "thetas: " << std::endl;
    for (auto& syn: this->synapseData) {
        std::cout << syn->theta << ", ";
    }
    std::cout << std::endl;
}


