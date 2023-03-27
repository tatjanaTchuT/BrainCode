#ifndef _RESOURCE_SYNAPSE_SPINE_CLASS_HEADER_
#define _RESOURCE_SYNAPSE_SPINE_CLASS_HEADER_

#include "BranchedSynapseSpine.hpp"
#include <vector>
#include <algorithm>
#include <iterator>
#include <numeric>

class ResourceSynapseSpine : public BranchedSynapseSpine {

    protected:
    //Resource model.
    // double kBasal{1.0};
    // double nBasal{1.0};
    double alphaBasal{1.0};

    // double kStimmulus{0.0};
    // double nStimmulus{0.0};
    double alphaStimmulus{1.0}; //This is delta alpha

    // double kStimmulusExpDecay{1.0};//This is of stimmulus variable
    // double nStimmulusExpDecay{1.0};//This is of stimmulus variable
    double alphaStimmulusExpDecay{1.0}; //This is delta alpha

    double alphaResources{};

    // std::vector<double> kStimmulusTempEffect{};//These need to be here for non-matrix management.
    // std::vector<double> nStimmulusTempEffect{};
    // std::vector<int> stimmulusEffectCount{};
    std::vector<std::pair<double, int>> alphaTempAndCount;

    int maxCount{100};

    public:
    //Getters
    //int GetMaxCount(){return maxCount;}//Not necessary for now
    //Setters

    // void SetNBasal(int nBasalInput){nBasal=nBasalInput;}
    // void SetKBasal(int kBasalInput){kBasal=kBasalInput;}
    // void SetKExponentialDecay(int kStimmulusExpDecayCalc){kStimmulusExpDecay=kStimmulusExpDecayCalc;}//Input is supposed to be exp(-dt/tau)
    // void SetNExponentialDecay(int nStimmulusExpDecayCalc){nStimmulusExpDecay=nStimmulusExpDecayCalc;}
    void SetMaxCount(int maxCountnew) {maxCount=maxCountnew;} //UNRESOLVED  Call in AllocateNewSynapse
    //Alpha methods
    void RecalculateAlphaResources();//Should be recalced at least once in the Spine Setup
    void DecayAlphaResources();
    //Temp effets methods
    void AddTempResourcesToSpine(double alphaStimmulusInput);
    void ApplyAllTempEffects(int STDPmultiplier);//input must be -1 if depression or STDPratio if potentiation, or the inverse swapping everything
    //Stimmulus vector methods
    void TickStimmulusCounts();//Called in Reset(), but both should be mutually exclusive with AATE above
    void CullStimmulusVectors();//Called in Reset()
    //Profile methods
    std::valarray<double> GetIndividualSynapticProfile() const override;
    std::string GetIndividualSynapticProfileHeaderInfo() const override;
};

#endif