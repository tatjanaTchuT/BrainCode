//
// Created by Saif Ahmed on 15.07.21.
//
#include <string>
#include "MonoDendriteSTDPTazerart.hpp"

MonoDendriteSTDPTazerart::MonoDendriteSTDPTazerart(GlobalSimInfo *info) : MonoDendriteSTDP(info) {
}

void MonoDendriteSTDPTazerart::SaveParameters(std::ofstream *stream) {
    MonoDendriteSTDP::SaveParameters(stream);
}

void MonoDendriteSTDPTazerart::LoadParameters(std::vector<std::string> *input) {
    MonoDendriteSTDP::LoadParameters(input);

    std::string name;
    std::vector<std::string> values;

    bool muLTPInitialized = false,
         sigmaLTPInitialized = false,
         alphaInitialized = false,
         muLTDInitialized = false,
         sigmaLTDInitialized = false,
         betaInitialized = false;

    for (auto & it : *input) {
        SplitString(&it, &name, &values);

        if (name.find("mu_LTP") != std::string::npos) {
            this->muLTP = std::stod(values.at(0));
            muLTPInitialized = true;
        } else if (name.find("sigma_LTP") != std::string::npos) {
            this->sigmaLTP = std::stod(values.at(0));
            sigmaLTPInitialized = true;
        } else if (name.find("alpha_LTP") != std::string::npos) {
            this->alpha = std::stod(values.at(0));
            alphaInitialized = true;
        } else if (name.find("mu_LTD") != std::string::npos) {
            this->muLTD = std::stod(values.at(0));
            muLTDInitialized = true;
        } else if (name.find("sigma_LTD") != std::string::npos) {
            this->sigmaLTD = std::stod(values.at(0));
            sigmaLTDInitialized = true;
        } else if (name.find("beta_LTD") != std::string::npos) {
            this->beta = std::stod(values.at(0));
            betaInitialized = true;
        }
    }

    assertm(muLTPInitialized == true, "Using heterosynaptic synapses without specifying dendritic_length is not allowed.");
    assertm(sigmaLTPInitialized == true, "Using heterosynaptic synapses without specifying synaptic_gap is not allowed.");
    assertm(alphaInitialized == true, "Using heterosynaptic synapses without specifying synaptic_gap is not allowed.");

    assertm(muLTDInitialized == true, "Using heterosynaptic synapses without specifying dendritic_length is not allowed.");
    assertm(sigmaLTDInitialized == true, "Using heterosynaptic synapses without specifying synaptic_gap is not allowed.");
    assertm(betaInitialized == true, "Using heterosynaptic synapses without specifying synaptic_gap is not allowed.");
}

void MonoDendriteSTDPTazerart::updateLTP(unsigned long synId) {
    SynapseExt* syn = this->synapseData[synId].get();
    this->weightsSum -= this->synapseData[synId]->weight;
    this->synapseData[synId]->weight +=  this->preFactorLTP * this->aLTP(syn->theta) * this->gLTP(this->lastPostSpikeTime - syn->lastSpike);
    this->synapseData[synId]->weight = std::min(2.0, this->synapseData[synId]->weight);
    this->weightsSum += this->synapseData[synId]->weight;
}

void MonoDendriteSTDPTazerart::updateLTD(unsigned long synId) {
    SynapseExt* syn = this->synapseData[synId].get();
    this->weightsSum -= this->synapseData[synId]->weight;
    this->synapseData[synId]->weight += this->preFactorLTD * this->aLTD(syn->theta) * this->gLTD(syn->lastSpike - this->lastPostSpikeTime);
    this->synapseData[synId]->weight = std::max(0.0, this->synapseData[synId]->weight);
    this->weightsSum += this->synapseData[synId]->weight;
}

double MonoDendriteSTDPTazerart::gLTP(double deltaT) const {//deltaT means this is a temporal rule?
    if (deltaT < 0.0) {//Are you sure this is how it has to behave when the input is negative?
        return 0.0;
    }
    double denominator = 2 * this->sigmaLTP * this->sigmaLTP;
    double numerator = this->muLTP - deltaT;
    numerator *= numerator;
    return exp(-numerator / denominator);
}

double MonoDendriteSTDPTazerart::gLTD(double deltaT) const {
    if (deltaT < 0.0) {
        return 0.0;
    }
    double denominator = 2 * this->sigmaLTD * this->sigmaLTD;
    double numerator = this->muLTD - deltaT;
    numerator *= numerator;
    return exp(-numerator / denominator);
}

double MonoDendriteSTDPTazerart::aLTP(double theta) const {
    return 2.0 - exp(-this->alpha * theta);
}

double MonoDendriteSTDPTazerart::aLTD(double theta) const {
    return -exp(-this->beta * theta);
}

std::string MonoDendriteSTDPTazerart::getType() {
    return str_MonoDendriteSTDPTazerart;
}

double MonoDendriteSTDPTazerart::getTimingEffects(const SynapseExt* synA, const SynapseExt* synB) const {
    if (synA == synB) {
        return 0.0;
    }
    if (synA->lastSpike < 0 || synB->lastSpike < 0) {
        return 0.0;
    }
    return exp(-abs(synA->lastSpike - synB->lastSpike) / this->tauDelay);
}

double MonoDendriteSTDPTazerart::getDistanceEffects(const SynapseExt* synA, const SynapseExt* synB) const {
    if (synA == synB) {
        return 0;
    }
    return exp(-abs(synA->distToSoma - synB->distToSoma) / this->lambdaDist);
}