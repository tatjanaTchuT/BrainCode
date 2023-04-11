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

void ResourceSynapseSpine::ApplyAllTempEffectsOnPostspike(double PotentiationDepressionRatio, DHashMap& STDPdecayMap)
{
    // kStimmulus=std::accumulate(kStimmulusTempEffect.begin(), kStimmulusTempEffect.end(), kStimmulus, [PotentiationDepressionRatio](double accumulator, double kStemp){return accumulator + kStemp*PotentiationDepressionRatio;});
    // nStimmulus=std::accumulate(nStimmulusTempEffect.begin(), nStimmulusTempEffect.end(), nStimmulus, [PotentiationDepressionRatio](double accumulator, double nStemp){return accumulator + nStemp*PotentiationDepressionRatio;});
    // kStimmulusTempEffect.clear();
    // nStimmulusTempEffect.clear();
    //stimmulusEffectCount.clear();
    //PotentiationDepressionRatio is to boost potentiation compared to depression, but also to input the current decay
    alphaStimmulus=std::accumulate(alphaTempAndCount.begin(), alphaTempAndCount.end(), alphaStimmulus, [PotentiationDepressionRatio, STDPdecayMap](double accumulator, std::pair<double, int>& alphaStemp){return accumulator + alphaStemp.first*PotentiationDepressionRatio*STDPdecayMap.at(alphaStemp.second);});
    // if (alphaStimmulus+alphaBasal<0.0){
    //     alphaStimmulus= (-alphaBasal);
    // }
}

void ResourceSynapseSpine::ApplyAllTempEffectsOnConflictPotentiation(double PotentiationDepressionRatio)
{
    //CUrrently I have no way of properly calculating the decay of potentiation proper by calculating the area under the graph.
    alphaStimmulus=std::accumulate(alphaTempAndCount.begin(), alphaTempAndCount.end(), alphaStimmulus, [PotentiationDepressionRatio](double accumulator, std::pair<double, int>& alphaStemp){return accumulator + alphaStemp.first*PotentiationDepressionRatio;});
    // if (alphaStimmulus+alphaBasal<0.0){
    //     alphaStimmulus= (-alphaBasal);
    // }
}

void ResourceSynapseSpine::ApplyAllTempEffectsOnDepression(DHashMap &STDPdecayMap, int STDPcount)
{
    //This is because depression updates instantly
    alphaStimmulus-= (alphaTempAndCount.back().first*STDPdecayMap.at(STDPcount));
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
        //Here we iterate over the alpha temporary effects on reverse. That way the first time we find a time-obsolete effect, we delete everything before it.
        //This is because everything before the last (first in reverse) count that reached max is older, so they must have reached the max count too
        if (reverseIterator->second>=maxCount){
            alphaTempAndCount.erase(alphaTempAndCount.begin(), std::next(reverseIterator).base());
            break;
        }

    }
}
