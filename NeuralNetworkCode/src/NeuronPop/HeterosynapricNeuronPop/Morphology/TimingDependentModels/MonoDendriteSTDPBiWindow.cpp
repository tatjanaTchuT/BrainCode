//
// Created by Saif Ahmed on 15.07.21.
//
#include <string>
#include "MonoDendriteSTDPBiWindow.hpp"

MonoDendriteSTDPBiWindow::MonoDendriteSTDPBiWindow(GlobalSimInfo *info) : MonoDendriteSTDP(info) {
}

void MonoDendriteSTDPBiWindow::SaveParameters(std::ofstream *stream, std::string neuronPreId) {
    //COMPLETE SP HERE
    MonoDendriteSTDP::SaveParameters(stream, neuronPreId);
}

void MonoDendriteSTDPBiWindow::LoadParameters(std::vector<std::string> *input) {
    MonoDendriteSTDP::LoadParameters(input);

    std::string name;
    std::vector<std::string> values;

    bool tauLTPInitialized = false, tauLTDInitialized = false, alphaInitialized = false, betaInitialized = false;

    for (auto & it : *input) {
        SplitString(&it, &name, &values);

        if (name.find("tau_LTP") != std::string::npos) {
            this->tauLTP = std::stod(values.at(0));
            tauLTPInitialized = true;
        } else if (name.find("tau_LTD") != std::string::npos) {
            this->tauLTD = std::stod(values.at(0));
            tauLTDInitialized = true;
        }  else if (name.find("alpha_LTP") != std::string::npos) {
            this->alpha = std::stod(values.at(0));
            alphaInitialized = true;
        } else if (name.find("beta_LTD") != std::string::npos) {
            this->beta = std::stod(values.at(0));
            betaInitialized = true;
        }
    }

    assertm(tauLTPInitialized == true, "Using heterosynaptic synapses without specifying dendritic_length is not allowed.");
    assertm(alphaInitialized == true, "Using heterosynaptic synapses without specifying synaptic_gap is not allowed.");

    assertm(tauLTDInitialized == true, "Using heterosynaptic synapses without specifying dendritic_length is not allowed.");
    assertm(betaInitialized == true, "Using heterosynaptic synapses without specifying synaptic_gap is not allowed.");
}

void MonoDendriteSTDPBiWindow::updateLTP(unsigned long synId) {
    SynapseSpine* syn = this->synapseData[synId].get();
//    this->weightsSum -= this->synapseData[synId]->weight;
//    double time = this->lastPostSpikeTime - syn->getLastSpike();
    double change = this->preFactorLTP * this->aLTP(syn->getTheta()) * this->gLTP(this->lastPostSpikeTime - syn->getLastSpike());
    this->synapseData[synId]->addToWeight(change);

//    this->weight_changes.emplace_back(synId, change);

//    if (synId == 0) {
//        std::cout << change << std::endl;
//    }

//    this->synapseData[synId]->weight = std::min(2.0, this->synapseData[synId]->weight);
//    this->weightsSum += this->synapseData[synId]->weight;

}

void MonoDendriteSTDPBiWindow::updateLTD(unsigned long synId) {
    SynapseSpine* syn = this->synapseData[synId].get();
//    this->weightsSum -= this->synapseData[synId]->weight;

//    double time = this->lastPostSpikeTime - syn->lastSpike;
    double change  = this->preFactorLTD * this->aLTD(syn->getTheta()) * this->gLTD(this->lastPostSpikeTime - syn->getLastSpike());
    this->synapseData[synId]->addToWeight(change);

//    this->weight_changes.emplace_back(synId, change);


//    std::cout << change << std::endl;

//    this->synapseData[synId]->weight = std::max(0.0, this->synapseData[synId]->weight);
//    this->weightsSum += this->synapseData[synId]->weight;
}

double MonoDendriteSTDPBiWindow::gLTP(double deltaT) const {
    if (deltaT <= 0.0) return 0.0;
    return exp(-deltaT / this->tauLTP);
}

double MonoDendriteSTDPBiWindow::gLTD(double deltaT) const {
    if (deltaT >= 0.0) return 0.0;
    return exp(deltaT / this->tauLTD);
}

double MonoDendriteSTDPBiWindow::aLTP(double theta) const {//coop
    return base_ltp + incr_ltp * (1 - exp(-this->alpha * theta));
}

double MonoDendriteSTDPBiWindow::aLTD(double theta) const {//coop
    return - (base_ltd - decr_ltd * (1 - exp(-this->beta * theta)));
}

const std::string MonoDendriteSTDPBiWindow::getType() {
    return str_MonoDendriteSTDPBiWindow;
}

double MonoDendriteSTDPBiWindow::getTimingEffects(const SynapseSpine* synA, const SynapseSpine* synB) const {
    if (synA == synB) {
        return 0.0;
    }
    if (synA->getLastSpike() < 0 || synB->getLastSpike() < 0) {
        return 0.0;
    }
    return exp(-std::abs(synA->getLastSpike() - synB->getLastSpike()) / this->tauDelay);
}

double MonoDendriteSTDPBiWindow::getDistanceEffects(const SynapseSpine* synA, const SynapseSpine* synB) const {
    if (synA == synB) {
        return 0;
    }
    return exp(-std::abs(synA->getDistToSoma() - synB->getDistToSoma()) / this->lambdaDist);
}
