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
    virtual std::shared_ptr<SynapseExt> allocateNewSynapse(unsigned long neuronId) override;
    virtual void recordExcitatorySynapticSpike(unsigned long neuronId, unsigned long synapseId) override;
    virtual std::valarray<double> getIndividualSynapticProfile(unsigned long neuronId, unsigned long synapseId)  override;
    virtual std::valarray<double> getOverallSynapticProfile(unsigned long neuronId) override;

    // Testing, check the purpose of this
    friend std::vector<unsigned long> getSpikedSynapses(const HeteroNeuronPop&, unsigned long);
    unsigned long getSynapseCount(unsigned long neuronId);
    double getWeight(unsigned long neuronId, unsigned long synapseId);

    // STDP Analysis
    void triggerStatOut(std::string dirPath);
    void printThetasAndWeights();

    //To optimize dynamic_casting in if statements

    virtual bool HasHeterosynapticPlasticity() override {return true;}

};


#endif //NEURALNETWORK_HETEROPOP_H
