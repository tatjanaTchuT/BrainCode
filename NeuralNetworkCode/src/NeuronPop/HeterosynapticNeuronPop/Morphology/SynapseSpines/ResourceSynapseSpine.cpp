#include "ResourceSynapseSpine.hpp"

std::valarray<double> ResourceSynapseSpine::GetIndividualSynapticProfile() const
{
    std::valarray<double> dataArray(5);
    dataArray[0] = this->distanceFromNode;
    dataArray[1] = this->branchId;
    dataArray[2] = this->weight;
    dataArray[3] = this->lastSpike;
    dataArray[4] = this->alphaResources;
    return dataArray;
}

std::string ResourceSynapseSpine::GetIndividualSynapticProfileHeaderInfo() const
{
    return std::string("{<dist to branch node>, <branch ID>, <weight>, <last spike>, <alpha resources>}");
}

void ResourceSynapseSpine::RecalculateAlphaResources()
{
    alphaResources=(kBasal+kStimmulus)/(nBasal+nStimmulus);
    DecayAlphaResources();
}

void ResourceSynapseSpine::DecayAlphaResources()
{
    kStimmulus*=kStimmulusExpDecay;
    nStimmulus*=nStimmulusExpDecay;
}

void ResourceSynapseSpine::AddTempResourcesToSpine(double kStimmulusInput, double nStimmulusInput)
{
    kStimmulusTempEffect.push_back(kStimmulusInput);
    nStimmulusTempEffect.push_back(nStimmulusInput);
    stimmulusEffectCount.push_back(0);
}

void ResourceSynapseSpine::ApplyAllTempEffects(int STDPmultiplier)
{
    kStimmulus=std::accumulate(kStimmulusTempEffect.begin(), kStimmulusTempEffect.end(), kStimmulus, [STDPmultiplier](double accumulator, double kStemp){return accumulator + kStemp*STDPmultiplier;});
    nStimmulus=std::accumulate(nStimmulusTempEffect.begin(), nStimmulusTempEffect.end(), nStimmulus, [STDPmultiplier](double accumulator, double nStemp){return accumulator + nStemp*STDPmultiplier;});
    kStimmulusTempEffect.clear();
    nStimmulusTempEffect.clear();
    stimmulusEffectCount.clear();
}

void ResourceSynapseSpine::TickStimmulusCounts()
{
    std::for_each(stimmulusEffectCount.begin(), stimmulusEffectCount.end(), [](int& element) {element+=1;});
}

void ResourceSynapseSpine::CullStimmulusVectors()
{
    for (int i=stimmulusEffectCount.size()-1; i>0; i--){//If exception, i>1//This may need to be changed later to use a vector for the indexes (or delete everything at the left of the last 0 count)
        if (stimmulusEffectCount.at(i)>=maxCount){
            stimmulusEffectCount.erase(std::next(stimmulusEffectCount.begin(), i));
            kStimmulusTempEffect.erase(std::next(kStimmulusTempEffect.begin(), i));
            nStimmulusTempEffect.erase(std::next(nStimmulusTempEffect.begin(), i));
        }
    }
}
