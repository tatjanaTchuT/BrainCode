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

#include "Morphology/TimingDependentModels/MonoDendriteSTDPTazerart.hpp"
#include "Morphology/TimingDependentModels/MonoDendriteSTDPTazerartRelative.hpp"
#include "Morphology/TimingDependentModels/MonoDendriteSTDPBiWindow.hpp"
#include "Morphology/BranchedSimpleModels/SimplePOnlyB.hpp"
#include "../../GlobalFunctions.hpp"
#include "../NeuronPop.hpp"
#include "./Morphology/SynapseSpine.hpp"

class HeteroCurrentSynapse;
class SynapseSpine;
class HeteroNeuronPop: public NeuronPop {

protected:
    std::vector<std::unique_ptr<Morphology>> morphology{};
    bool isBranched{false}; //PARAMETERLOADING with Set isBranchedTrue

public:
    explicit HeteroNeuronPop(GlobalSimInfo * info, int id);
    ~HeteroNeuronPop() override = default;

    void SaveParameters(std::ofstream * stream) override;
    void LoadParameters(std::vector<std::string> *input) override;

    // Heterosynaptic functionality
    virtual std::shared_ptr<SynapseSpine> allocateNewSynapse(unsigned long neuronId, HeteroCurrentSynapse& syn) override;
    virtual void recordExcitatorySynapticSpike(unsigned long neuronId, unsigned long synapseId) override;
    virtual std::valarray<double> getIndividualSynapticProfile(unsigned long neuronId, unsigned long synapseId)  override;
    virtual std::valarray<double> getOverallSynapticProfile(unsigned long neuronId) override;

    //Accessing dendritic data
    unsigned long getSynapseCount(unsigned long neuronId);
    double getWeight(unsigned long neuronId, unsigned long synapseId);

    //Inheritance optimizations

    virtual bool HasHeterosynapticPlasticity() override {return true;}
    virtual bool isBranchedBool() override {return isBranched;}
    void SetBranchedTrue(){isBranched=true;}

    // Testing, check the purpose of this
    friend std::vector<unsigned long> getSpikedSynapses(const HeteroNeuronPop&, unsigned long neuronId); //currently not in use

    //Morphology& getMorphologyRef(int neuronId); //Return a reference to Morpho object

    // STDP Analysis
    void triggerStatOut(std::string dirPath); //currently not in use
    void printThetasAndWeights();  //currently not in use



};


#endif //NEURALNETWORK_HETEROPOP_H
