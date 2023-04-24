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
    depressionAlphaTempAndCount.push_back(alphaStimmulusInput);
    potentiationAlphaTempAndCount.emplace_back(PairDI(std::abs(alphaStimmulusInput), 0));
}

bool ResourceSynapseSpine::ApplyAllTempEffectsOnPostspike(const DHashMap& STDPdecayMap)
{
    // kStimmulus=std::accumulate(kStimmulusTempEffect.begin(), kStimmulusTempEffect.end(), kStimmulus, [PotentiationDepressionRatio](double accumulator, double kStemp){return accumulator + kStemp*PotentiationDepressionRatio;});
    // nStimmulus=std::accumulate(nStimmulusTempEffect.begin(), nStimmulusTempEffect.end(), nStimmulus, [PotentiationDepressionRatio](double accumulator, double nStemp){return accumulator + nStemp*PotentiationDepressionRatio;});
    // kStimmulusTempEffect.clear();
    // nStimmulusTempEffect.clear();
    //stimmulusEffectCount.clear();
    //PotentiationDepressionRatio is to boost potentiation compared to depression, but also to input the current decay
    alphaStimmulus=std::accumulate(potentiationAlphaTempAndCount.begin(), potentiationAlphaTempAndCount.end(), alphaStimmulus, [this, STDPdecayMap](double accumulator, const PairDI& alphaStemp){return accumulator + alphaStemp.first*PotentiationDepressionRatio*STDPdecayMap.at(alphaStemp.second);});
    // if (alphaStimmulus+alphaBasal<0.0){
    //     alphaStimmulus= (-alphaBasal);
    // }
    return (!potentiationAlphaTempAndCount.size()==0);
}

// void ResourceSynapseSpine::ApplyAllTempEffectsOnConflictPotentiation(double PotentiationDepressionRatio)
// {
//     //CUrrently I have no way of properly calculating the decay of potentiation proper by calculating the area under the graph.
//     alphaStimmulus=std::accumulate(potentiationAlphaTempAndCount.begin(), potentiationAlphaTempAndCount.end(), alphaStimmulus, [PotentiationDepressionRatio](double accumulator, PairDI& alphaStemp){return accumulator + alphaStemp.first*PotentiationDepressionRatio;});
//     // if (alphaStimmulus+alphaBasal<0.0){
//     //     alphaStimmulus= (-alphaBasal);
//     // }
// }

bool ResourceSynapseSpine::ApplyAllTempEffectsOnDepression(double expDecayFactor)
{
    //This is because depression updates instantly
    alphaStimmulus=std::accumulate(depressionAlphaTempAndCount.begin(), depressionAlphaTempAndCount.end(), alphaStimmulus, [expDecayFactor](double accumulator, double alphaStemp){return accumulator - alphaStemp*expDecayFactor;});
    
    if (alphaStimmulus+alphaBasal<0.0){
        alphaStimmulus= (-alphaBasal);
    }
    return (depressionAlphaTempAndCount.size()!=0);
}

void ResourceSynapseSpine::TickStimmulusCounts()
{
    std::for_each(potentiationAlphaTempAndCount.begin(), potentiationAlphaTempAndCount.end(), [](PairDI& element) {element.second++;});
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

    for (std::list<PairDI>::reverse_iterator reverseIterator = potentiationAlphaTempAndCount.rbegin(); reverseIterator != potentiationAlphaTempAndCount.rend(); reverseIterator++)
    {
        //Here we iterate over the alpha temporary effects on reverse. That way the first time we find a time-obsolete effect, we delete everything before it.
        //This is because everything before the last (first in reverse) count that reached max is older, so they must have reached the max count too
        if (reverseIterator->second>=maxCount){
            potentiationAlphaTempAndCount.erase(potentiationAlphaTempAndCount.begin(), reverseIterator.base());
            break;
        }
    }
    depressionAlphaTempAndCount.clear();
}
