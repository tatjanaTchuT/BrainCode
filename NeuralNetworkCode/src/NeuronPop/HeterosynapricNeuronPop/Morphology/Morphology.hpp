#ifndef NEURALNETWORK_MORPHOLOGY_H
#define NEURALNETWORK_MORPHOLOGY_H

#include <vector>
#include <cmath>
#include <limits>
#include <valarray>
#include <algorithm>

#include "../../../GlobalFunctions.hpp"

class HeteroCurrentSynapse;

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

    std::vector<std::shared_ptr<SynapseExt>> synapseData;
    double weightsSum {};
    double totalPostSpikes {};
    double totalPreSpikes {};

    double lastPostSpikeTime;
    std::vector<bool> spikedSynapses;
    std::vector<unsigned long> spikedSynapsesId;

    WeightNormalization weightNormalization {NOPNormalization};
    double minWeight {0.0};
    double initialWeights {1.0};
    double maxWeight {2.0};

    bool decayWeights {false};
    double weightDecayConstant {};
    double expdt {};

    void reset();
    void normalizeWeights();

    virtual void timeDecay();

public:
    explicit Morphology(GlobalSimInfo * info);
    virtual ~Morphology() = default;

    virtual void SaveParameters(std::ofstream * stream, std::string neuronPreId);
    virtual void LoadParameters(std::vector<std::string> *input);

    virtual std::shared_ptr<SynapseExt> allocateNewSynapse(HeteroCurrentSynapse& syn)=0;

    virtual std::string getType() = 0;

    virtual void advect();
    virtual void recordPostSpike();
    virtual void recordExcitatoryPreSpike(unsigned long synSpikerId);
    virtual std::valarray<double> getIndividualSynapticProfile(unsigned long synapseId) const = 0;
    virtual std::valarray<double> getOverallSynapticProfile() const = 0;

    friend std::vector<unsigned long> getSpikedSynapsesFromMorphology(const Morphology&);
    unsigned long getSynapseCount() const;

    double getWeight(unsigned long synapseId) const;

    // STDP Analysis
    //void triggerStatOut(std::string dirPath);

    void printThetasAndWeights();

    //Qualifying methods
    virtual bool isBranchedBool() {return false;}

};

std::vector<unsigned long> getSpikedSynapsesFromMorphology(const Morphology&);

#endif //NEURALNETWORK_MORPHOLOGY_H
