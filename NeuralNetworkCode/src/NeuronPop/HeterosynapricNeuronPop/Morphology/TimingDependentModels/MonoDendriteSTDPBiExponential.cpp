////
//// Created by Saif Ahmed on 19.07.21.
////
//
//#include "MonoDendriteSTDPBiExponential.hpp"
//
//MonoDendriteSTDPBiExponential::MonoDendriteSTDPBiExponential(GlobalSimInfo *info) : MonoDendriteSTDP(info) {
//}
//
//void MonoDendriteSTDPBiExponential::updateLTP(unsigned long synId) {
//    SynapseExt* syn = this->synapseData[synId].get();
//    this->weightsSum -= this->synapseData[synId]->weight;
//    this->synapseData[synId]->weight += this->preFactorLTP * this->aLTP(syn->theta) * this->gLTP(this->lastPostSpikeTime - syn->lastSpike);
//    this->synapseData[synId]->weight = std::min(2.0, this->synapseData[synId]->weight);
//    this->synapseData[synId]->theta = 0.0; // TODO: check with Andreas
//    this->weightsSum += this->synapseData[synId]->weight;
//}
//
//void MonoDendriteSTDPBiExponential::updateLTD(unsigned long synId) {
//    SynapseExt* syn = this->synapseData[synId].get();
//    this->weightsSum -= this->synapseData[synId]->weight;
//    this->synapseData[synId]->weight += this->preFactorLTD * this->aLTD(syn->theta) * this->gLTD(syn->lastSpike - this->lastPostSpikeTime);
//    this->synapseData[synId]->weight = std::max(0.0, this->synapseData[synId]->weight);
//    this->synapseData[synId]->theta = 0.0; // TODO: check with Andreas
//    this->weightsSum += this->synapseData[synId]->weight;
//}
//
//double MonoDendriteSTDPBiExponential::gLTP(double deltaT) const {
//    double denominator = 2 * this->sigmaLTP * this->sigmaLTP;
//    double numerator = this->muLTP - deltaT;
//    numerator *= numerator;
//    return exp(-numerator / denominator);
//}
//
//double MonoDendriteSTDPBiExponential::gLTD(double deltaT) const {
//    double denominator = 2 * this->sigmaLTD * this->sigmaLTD;
//    double numerator = this->muLTD - deltaT;
//    numerator *= numerator;
//    return exp(-numerator / denominator);
//}
//
//double MonoDendriteSTDPBiExponential::aLTP(double theta) const {
//    return 1.0;
//}
//
//double MonoDendriteSTDPBiExponential::aLTD(double theta) const {
//    return 1.0;
//}
//
//std::string MonoDendriteSTDPBiExponential::getType() {
//    return str_MonoDendriteSTDPBiExponential;
//}