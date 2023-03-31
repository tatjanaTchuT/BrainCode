#include "ResourceSynapseSpine.hpp"

std::valarray<double> ResourceSynapseSpine::GetIndividualSynapticProfile() const
{
    std::valarray<double> dataArray(3);
    dataArray[0] = this->branchId;
    dataArray[1] = this->weight;
    dataArray[2] = this->alphaResources;
    return dataArray;
}

std::string ResourceSynapseSpine::GetIndividualSynapticProfileHeaderInfo() const
{
    return std::string("{<branch ID>, <weight>, <alpha resources>}");
}

void ResourceSynapseSpine::RecalculateAlphaResources()
{
    //alphaResources=(kBasal+kStimmulus)/(nBasal+nStimmulus);
    alphaResources=alphaBasal+alphaStimmulus;
    DecayAlphaResources();
}

void ResourceSynapseSpine::DecayAlphaResources()
{
    // kStimmulus*=kStimmulusExpDecay;
    // nStimmulus*=nStimmulusExpDecay;
    alphaStimmulus *= alphaStimmulusExpDecay;
}

void ResourceSynapseSpine::RecalcWeight(double weightResourceFactor)
{
    this->weight=this->alphaResources*weightResourceFactor;
}

void ResourceSynapseSpine::AddTempResourcesToSpine(double alphaStimmulusInput)
{
    // kStimmulusTempEffect.push_back(kStimmulusInput);
    // nStimmulusTempEffect.push_back(nStimmulusInput);
    //stimmulusEffectCount.push_back(0);
    alphaTempAndCount.emplace_back(std::pair<double, int>(alphaStimmulusInput, 0));
}

void ResourceSynapseSpine::ApplyAllTempEffects(int STDPmultiplier, DHashMap& STDPdecayMap)
{
    // kStimmulus=std::accumulate(kStimmulusTempEffect.begin(), kStimmulusTempEffect.end(), kStimmulus, [STDPmultiplier](double accumulator, double kStemp){return accumulator + kStemp*STDPmultiplier;});
    // nStimmulus=std::accumulate(nStimmulusTempEffect.begin(), nStimmulusTempEffect.end(), nStimmulus, [STDPmultiplier](double accumulator, double nStemp){return accumulator + nStemp*STDPmultiplier;});
    // kStimmulusTempEffect.clear();
    // nStimmulusTempEffect.clear();
    //stimmulusEffectCount.clear();
    alphaStimmulus=std::accumulate(alphaTempAndCount.begin(), alphaTempAndCount.end(), alphaStimmulus, [STDPmultiplier, STDPdecayMap](double accumulator, std::pair<double, int>& alphaStemp){return accumulator + alphaStemp.first*STDPmultiplier*STDPdecayMap.at(alphaStemp.second);});
    if (alphaStimmulus+alphaBasal<0.0){
        alphaStimmulus= (-alphaBasal);
    }
}

void ResourceSynapseSpine::TickStimmulusCounts()
{
    std::for_each(alphaTempAndCount.begin(), alphaTempAndCount.end(), [](std::pair<double,int>& element) {element.second+=1;});
}

void ResourceSynapseSpine::CullStimmulusVectors()
{
    // for (int i=stimmulusEffectCount.size()-1; i>0; i--){//If exception, i>1//This may need to be changed later to use a vector for the indexes (or delete everything at the left of the last 0 count)
    //     if (stimmulusEffectCount.at(i)>=maxCount){
    //         stimmulusEffectCount.erase(std::next(stimmulusEffectCount.begin(), i));
    //         kStimmulusTempEffect.erase(std::next(kStimmulusTempEffect.begin(), i));
    //         nStimmulusTempEffect.erase(std::next(nStimmulusTempEffect.begin(), i));
    //     }
    // }

    for (std::list<std::pair<double,int>>::reverse_iterator reverseIterator = alphaTempAndCount.rbegin(); reverseIterator != alphaTempAndCount.rend(); reverseIterator++)
    {
        if (reverseIterator->second>=maxCount){
            alphaTempAndCount.erase(alphaTempAndCount.begin(), std::next(reverseIterator).base());
            break;
        }

    }
}
