#include "Morphology.hpp"

Morphology::Morphology(GlobalSimInfo *info): info(info), lastPostSpikeTime(-200), weightsSum(0), totalPostSpikes(0), totalPreSpikes(0), weightNormalization(NOPNormalization) {
}

void Morphology::LoadParameters(std::vector<std::string>* input) {

    std::string name;
    std::vector<std::string> values;

    // checks for correct initialization
    bool normalizationFound {false};

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
        } else if (name.find("weight_decay") != std::string::npos) {
            this->decayWeights = {values.at(0)=="true"};
            this->weightDecayConstant = std::stod(values.at(1));
            this->expdt=exp(-this->info->dt/this->weightDecayConstant);
        } else if (name.find("min-max_weights") != std::string::npos) {
            this->minWeight = std::stod(values.at(0));
            this->maxWeight = std::stod(values.at(1));
        }
        //include here max and min weights

    }
    assert(normalizationFound);
}
void Morphology::SaveParameters(std::ofstream *stream, std::string neuronPreId) {
    *stream<< "#From here on is all Heterostuff\n";
    *stream << neuronPreId<<"_morphology_type\t\t\t\t"<<this->getType()<<"\n";
    *stream << neuronPreId<<"_morphology_weight_normalization\t";
    if (this->weightNormalization == HardNormalization){
        *stream<<"HardNormalization\n";
    }
    else if (this->weightNormalization == SoftMaxNormalization){
        *stream<<"SoftMaxNormalization\n";
    }
    else if (this->weightNormalization == NOPNormalization){
        *stream<<"NOPNormalization\n";
    }

    *stream << neuronPreId<<"_morphology_weight_decay\t"<<std::boolalpha<<this->decayWeights<<"\t"<<std::to_string(this->weightDecayConstant);
    *stream<<"\t"<<"#The first bool activates the weight decay per timestep. The second number is the time constant on an exponential in seconds [exp(-dt/ctt)].\n";

    *stream << neuronPreId<<"_morphology_min-max_weights\t"<<std::to_string(this->minWeight)<<"\t"<<std::to_string(this->maxWeight);
    *stream<<"\t"<<"#Only relevant for HardNormalization and distribute_weights, the first number is the minimum weight in normalization, the second one the hard cap for weight.\n";
    //include here max and min weights
}


void Morphology::advect() {
    this->timeDecay();
}

void Morphology::recordPostSpike() {
    this->lastPostSpikeTime = this->info->dt * static_cast<double> (this->info->time_step);
    this->totalPostSpikes++;
    // STDP Analysis
    //this->postSpikes.push_back(this->lastPostSpikeTime);
}


void Morphology::recordExcitatoryPreSpike(unsigned long synSpikerId) {
    //Is there supposed to be a different Inhibitory function?
    this->synapseData.at(synSpikerId)->setLastSpike(static_cast<double> (this->info->time_step) * this->info->dt);
    this->spikedSynapsesId.push_back(synSpikerId);
    this->spikedSynapses.at(synSpikerId) = true;//This does not seem to be correctly implemented
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
    return this->synapseData.at(synapseId)->getWeight();
}

void Morphology::reset() {
    this->normalizeWeights();
    std::fill(this->spikedSynapses.begin(),this->spikedSynapses.end(), false);
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
        syn->setWeight(std::max(minWeight, std::min(maxWeight, syn->getWeight())));
    }
}

void Morphology::softMaxNormalize() {
    double maxWeights, sumWeights;

    maxWeights = std::numeric_limits<double>::min();
    for (auto& syn : this->synapseData) {
        maxWeights = std::max(maxWeights, syn->getWeight());
    }

    sumWeights = 0;
    for (auto& syn : this->synapseData) {
        sumWeights += std::exp(syn->getWeight() - maxWeights);
    }
}

void Morphology::timeDecay() {
    const double& expdt {this->expdt};
    if (this->decayWeights) {
        for (const std::shared_ptr<SynapseSpine>& syn: this->synapseData) {
            syn->setWeight(syn->getWeight() * expdt);
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
        std::cout << syn->getWeight() << ", ";
    }
    std::cout << std::endl;

    std::cout << "thetas: " << std::endl;
    for (auto& syn: this->synapseData) {
        std::cout << syn->getTheta() << ", ";
    }
    std::cout << std::endl;
}


