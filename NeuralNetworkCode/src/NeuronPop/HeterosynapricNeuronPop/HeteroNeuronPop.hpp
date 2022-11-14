//
// Created by Saif Ahmed on 03.08.21.
//

#ifndef NEURALNETWORK_HETEROPOP_H
#define NEURALNETWORK_HETEROPOP_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <algorithm>

#include "Morphology/Morphology.hpp"
#include "Morphology/TimingDependentModels/MonoDendriteSTDPTazerart.hpp"
#include "Morphology/TimingDependentModels/MonoDendriteSTDPTazerartRelative.hpp"
#include "Morphology/TimingDependentModels/MonoDendriteSTDPBiWindow.hpp"
#include "../../GlobalFunctions.hpp"
#include "../NeuronPop.hpp"

class HeteroNeuronPop: public NeuronPop {

protected:
    std::vector<std::unique_ptr<Morphology>> morphology{};

public:
    explicit HeteroNeuronPop(GlobalSimInfo * info, int id);
    ~HeteroNeuronPop() override = default;

    void SaveParameters(std::ofstream * stream) override;
    void LoadParameters(std::vector<std::string> *input) override;

    // heterosynaptic functionality
    std::shared_ptr<SynapseExt> allocateNewSynapse(unsigned long neuronId);
    void recordSynapticSpike(unsigned long neuronId, unsigned long synapseId);
    std::valarray<double> getIndividualSynapticProfile(unsigned long neuronId, unsigned long synapseId);
    std::valarray<double> getOverallSynapticProfile(unsigned long neuronId);

    // Testing
    friend std::vector<unsigned long> getSpikedSynapses(const HeteroNeuronPop&, unsigned long);
    unsigned long getSynapseCount(unsigned long neuronId);
    double getWeight(unsigned long neuronId, unsigned long synapseId);

};


#endif //NEURALNETWORK_HETEROPOP_H
