#ifndef NEURALNETWORK_MORPHOLOGY_H
#define NEURALNETWORK_MORPHOLOGY_H


#include "./SynapseSpines/SynapseSpineBase.hpp"
#include "./../../../GlobalFunctions.hpp"
#include "../../../Synapse/HeteroCurrentSynapse.hpp"
#include <vector>
#include <cmath>
#include <limits>
#include <valarray>
#include <algorithm>
#include <iostream>
#include <string>


enum WeightNormalization {
    NOPNormalization, HardNormalization, SoftMaxNormalization
};

class Morphology {

protected:
    void hardNormalize();
    void softMaxNormalize();

    // STDP Analysis
    std::vector<std::pair<unsigned long, double>> preSpikes;
    std::vector<double> postSpikes;

    std::vector<std::pair<unsigned long, double>> theta_changes;
//    std::vector<double> theta_changes;
//    std::vector<double> theta_changes;

    std::vector<std::pair<unsigned long, double>> weight_changes;

    GlobalSimInfo * info;

    std::vector<std::shared_ptr<SynapseSpineBase>> synapseData;
    double weightsSum {};
    double totalPostSpikes {};
    double totalPreSpikes {};
    bool postSpiked{false};

    bool distributeWeights{false};
    int seed{0};
    std::default_random_engine generator{ std::default_random_engine(seed) };

    double lastPostSpikeTime;
    std::vector<bool> spikedSynapses;
    std::vector<unsigned long> spikedSynapsesId;

    WeightNormalization weightNormalization {NOPNormalization};
    double minWeight {0.0};
    double initialWeights {1.0};
    double maxWeight {2.0};

    bool decayWeights {false};
    double weightDecayConstant{1.0};
    double weightExpDecay {};

    virtual void reset();
    void normalizeWeights();

    virtual void weightDecay();

public:
    explicit Morphology(GlobalSimInfo * info);
    virtual ~Morphology() = default;

    virtual void SaveParameters(std::ofstream * stream, std::string neuronPreId);
    virtual void LoadParameters(std::vector<std::string> *input);

    virtual std::shared_ptr<SynapseSpineBase> allocateNewSynapse(HeteroCurrentSynapse& synapse)=0;

    virtual const std::string getType() = 0;

    virtual void advect() = 0;
    virtual void recordPostSpike();
    virtual void recordExcitatoryPreSpike(unsigned long synSpikerId);
    virtual std::valarray<double> getIndividualSynapticProfile(unsigned long synapseId) const = 0;
    virtual std::valarray<double> getOverallSynapticProfile() const;

    //friend std::vector<unsigned long> getSpikedSynapsesFromMorphology(const Morphology&); // This function is not necessary as the spikedSynapses is not used outside of the class
    unsigned long getSynapseCount() const;
    double generateSynapticWeight();// Here we generate the synaptic weight to be allocated when a synapse is allocated
    double getWeight(unsigned long synapseId) const;

    // STDP Analysis
    //void triggerStatOut(std::string dirPath);

    //void printThetasAndWeights();

    //Qualifying methods
    virtual bool const isBranchedBool() {return false;}

};

//std::vector<unsigned long> getSpikedSynapsesFromMorphology(const Morphology&);

#endif //NEURALNETWORK_MORPHOLOGY_H
