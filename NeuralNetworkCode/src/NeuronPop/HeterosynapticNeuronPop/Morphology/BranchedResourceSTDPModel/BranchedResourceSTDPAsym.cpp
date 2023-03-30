#include "./BranchedResourceSTDPAsym.hpp"
#include "BranchedResourceSTDPAsym.hpp"

BranchedResourceSTDPAsymmetric::BranchedResourceSTDPAsymmetric(GlobalSimInfo *info):BranchedMorphology(info)
{
}

void BranchedResourceSTDPAsymmetric::LoadParameters(std::vector<std::string> *input)
{
    std::string name;
    std::vector<std::string> values;
    
    BranchedMorphology::LoadParameters(input);

    for (auto & it : *input) {
        SplitString(&it,&name,&values);
        if(name.find("available_weight") != std::string::npos){
            this->branchings = std::stod(values.at(0));//CHANGEs
        }
    }
    decayWeights=false;
}
void BranchedResourceSTDPAsymmetric::SaveParameters(std::ofstream *stream, std::string neuronPreId)
{
    BranchedMorphology::SaveParameters(stream, neuronPreId);
    *stream << neuronPreId<<"_morphology_available_weight\t\t"<<std::to_string(this->branchings);//CHANGE
    *stream << "\t"<<"#Total weight available to be distributed among synapses per time-step.\n";
}

void BranchedResourceSTDPAsymmetric::SetUpBranchings(int remainingBranchingEvents, std::vector<int> anteriorBranches)
{
    //This is a recursive function that sets up the branched dendritic tree and is generalized for 0 branchings (1 branch). This function has been unit tested by Antoni.
    remainingBranchingEvents-=1;
    //First call is done with an empty int vector
    for (int i = 0; i < 2;i++) {
        int branchId{this->GenerateBranchId()};
        this->resourceBranches.emplace_back(std::make_shared<ResourceBranch>(ResourceBranch(this->synapticGap, this->branchLength, anteriorBranches, branchId, MaxCountSTDP, branchMaxCountTrigger, this->synapseDataResources)));//This vector should be sorted by ID by default (tested).
        this->branches.push_back(static_cast<std::shared_ptr<Branch>>(this->resourceBranches.back()));
        //Constructor here
        if(remainingBranchingEvents>0){
            std::vector<int> anteriorBranchesCopy(anteriorBranches);
            anteriorBranchesCopy.push_back(branchId);
            this->SetUpBranchings(remainingBranchingEvents, anteriorBranchesCopy);
        }
    }
}

void BranchedResourceSTDPAsymmetric::SetUpHashTables()
{
//Statement 2 of second for loop is done thinking about time steps being in the order of magnitude of 0.1 ms, while the gap is 1 um. 
//If we make the kernel triangular, every "step" is one gap in space and in time it can be any amount of timesteps. To calculate the step relationship, we do:
    if ((static_cast<double>(timeKernelLength)/kernelGapNumber)<1.0){
        throw; //The timeKernel lenght is always thought to be longer than the spatial one. 1 ms timing for 10 um is very one-sided, and that is equilibrium
    }
    spaceTimeStepRelation = timeKernelLength/kernelGapNumber;
    kernelRadius=timeKernelLength+spaceTimeStepRelation;
    for (int spaceIndex=1; spaceIndex<=kernelGapNumber; spaceIndex++){//at least there is one gap
        for (int timeIndex=0; timeIndex<=timeKernelLength-(spaceTimeStepRelation*(spaceIndex-1)); timeIndex++){ 
            kernelHashTable[timeIndex][spaceIndex]=std::exp(-(synapticGap*spaceIndex)/spaceKernelDecayConstant)*std::exp(-(info->dt*timeIndex)/timeKernelDecayConstant);
            //Always access time, space later (always will be more times than gaps)
        }
    }

    for (int STDPindex=1; STDPindex<=MaxCountSTDP; STDPindex++){
        STDPDecayHashTable[STDPindex]=std::exp(-(STDPindex*info->dt)/STDPDecayConstant);
    }
}

void BranchedResourceSTDPAsymmetric::advect()
{
    //if this postSpike bool

    //Here I have to take into account the circular behaviour: from t=10 s=1 to s=10 t=1, sort of symmetric. (Each syn gap you move, you lose 10 dt of time)
        //If STDPDEpression count is not 10, immediately trigger STDP on spine trigger (while also flagging)
        //If first precount is bigger than twice the STDP depression count, depression. Otherwise potentiation.
    //When iterating over the synapse vector (whichever it might be inside a branch), always compare to .begin() or .end(). 
    //If begin(), call kernel and then break the for loop. If end() break the loop
    //USE BIDIRECTIONAL ACCESS ITERATOR!!!! To do from center to left, from center to right

    //Remember to call the branch plasticity counter for every event!!!
}

void BranchedResourceSTDPAsymmetric::STDPPotentiation()
{
    //If post spike, apply all stimms on positive mode (remember the coded function in spines)
}

void BranchedResourceSTDPAsymmetric::STDPDepression()
{
     //If count <10, apply stimms in negative mode. Basically input -1/STDPratio to the spine function. Has to be limited to zero alpha stimm or zero alpha, never negative
     //And also pass the STDP map by reference to the function call
}

void BranchedResourceSTDPAsymmetric::DetectPairing(std::vector<int> synapseIndexesToUpdate)//These are the spiked neurons on synapseDataIndexes
{

    //Here we call SetEffects if __it__ happens
}

void BranchedResourceSTDPAsymmetric::SetEffects(int synapseSpineIDinMorpho)
{
    //if STDPDepression counter, append synId to updatedSynapseSpines in branch
    //Here we call SpaceTimeKernel
}

void BranchedResourceSTDPAsymmetric::SpaceTimeKernel(int branchSynapseID, int branchID, int synapseSpineIDinMorpho)
{
    int synapsePositionIndex,absDistance,timeStepDifference;
    double alphaStimmulusEffect;
    std::set<int>& updatedSynapses = resourceBranches.at(branchID)->updatedSynapseSpines;
    std::set<int>& updatedAlphaEffects = resourceBranches.at(branchID)->updatedAlphaEffects;
    size_t& branchSlots{branches.at(branchID)->branchSlots};
    //Only call this function if synapse itself has spiked this timestep
    for (int gapIndex = -kernelGapNumber; gapIndex<=kernelGapNumber;gapIndex++){ //And then use branchSynapseID+gapindex
        synapsePositionIndex = std::min(0,branchSynapseID+gapIndex);
        if (synapsePositionIndex>branchSlots){
            break;
        }
        absDistance = std::abs(gapIndex);
        timeStepDifference=resourceBranches.at(branchID)->triggerCount.at(synapsePositionIndex);
        if(timeStepDifference+synapticGap*absDistance<=kernelRadius && (updatedSynapses.find(synapsePositionIndex) != updatedSynapses.end())){        //triggercount+distance gap*spaceTimeStepRelation<maxCountTime + 1*spaceTimeStepRelation (what should constrain the triangular matrix)
            alphaStimmulusEffect = baseAlphaStimmulusBump*CallKernelHashTable(absDistance, timeStepDifference);
            synapseDataResources.at(synapseSpineIDinMorpho)->AddTempResourcesToSpine(alphaStimmulusEffect);
            synapseDataResources.at(branches.at(branchID)->synapseSlotToMorphoIDMap.at(synapsePositionIndex))->AddTempResourcesToSpine(alphaStimmulusEffect);
            updatedAlphaEffects.insert(synapsePositionIndex);
        }
    }
    updatedSynapses.insert(branchSynapseID);
    //Here for every synapse inside the synapse's kernel that has an active counter (count!=countMax) we get the time kernel and then apply the space kernel
    //Take unto account the synaptic GAP and DT!!! This should be done elsewhere
    return;
}
double BranchedResourceSTDPAsymmetric::CallKernelHashTable(int distanceToCenterInGaps, int timeDifference)
{
    //Matrix is symmetric!! Only have to do half!
    //Here we tabulate for every distance from center a equivalency between counts and effects (reverse as counts go up to maxCount)
    //The pair consists of ns and ks. The function has to obtain the count from synapse and distance?
    //OR the hashtable contains the ns and ks multipliers (for now they are the same)
    return kernelHashTable.at(timeDifference).at(distanceToCenterInGaps);
}

void BranchedResourceSTDPAsymmetric::TickCounts()
{
    //Iterate over synapses ticking counts in the branch and in the morpho and in the spines
}

void BranchedResourceSTDPAsymmetric::ApplyEffects()
{
//     std::sort(removelist.begin(), removelist.end());  // Make sure the container is sorted
// for (auto &i = removelist.rbegin(); i != removelist.rend(); ++ i)
// {
//     a.erase(a.begin() + *i);
// }
}
void BranchedResourceSTDPAsymmetric::DeleteEffects()
{
    //Iterate over synapses, calling the culling function
//     std::sort(removelist.begin(), removelist.end());  // Make sure the container is sorted
// for (auto &i = removelist.rbegin(); i != removelist.rend(); ++ i)
// {
//     a.erase(a.begin() + *i);
// }
}

void BranchedResourceSTDPAsymmetric::Reset()
{
    //Wrapper plus clearing some of the vectors. Last Reset method to run in chronological order, where we call the ticks and the general upkeep
    //Check increase beta resources here?
    //Clear both sets in every branch
}

void BranchedResourceSTDPAsymmetric::RecalcAlphas(std::shared_ptr<ResourceBranch> branch)
{
    //Here we just need to apply all delta alphas, decay them (before makes more sense, the bump has delta t zero.), sum the result to stationary alpha, then update alpha sums? Yes
    for (std::shared_ptr<ResourceSynapseSpine> synapse : branch->branchSynapseData){
        synapse->RecalculateAlphaResources();
    }
}

void BranchedResourceSTDPAsymmetric::RecalcWeights(std::shared_ptr<ResourceBranch> branch) //This one is the one we call for every branch
{
    RecalcAlphaSums(branch);
    branch->weightResourceFactor=betaResourcePool/(omegaPassiveResourceOffset+branch->alphaTotalSum);//IMPORTANT, if we make beta non-constant, beta must be referenced from the branch
    for (std::shared_ptr<ResourceSynapseSpine>& synapse : branch->branchSynapseData){
        synapse->RecalcWeight(branch->weightResourceFactor);
    }
}

void BranchedResourceSTDPAsymmetric::RecalcAlphaSums(std::shared_ptr<ResourceBranch> branch)
{
    RecalcAlphas(branch);
    branch->alphaTotalSum=std::accumulate(branch->branchSynapseData.begin(), branch->branchSynapseData.end(), 0.0,//UNRESOLVED, does this give intended output?
                                       [] (double acc, const std::shared_ptr<ResourceSynapseSpine>& synapse) {return acc + synapse->GetAlphaResources();});
}

void BranchedResourceSTDPAsymmetric::RecordPostSpike()
{
    BranchedMorphology::RecordPostSpike();
    STDPDepressionCount=0;
}

void BranchedResourceSTDPAsymmetric::RecordExcitatoryPreSpike(unsigned long spikedSynapseId)
{
    //Here only record, afterwards we do the checks
    BranchedMorphology::RecordExcitatoryPreSpike(spikedSynapseId);
    this->resourceBranches.at(synapseDataResources.at(spikedSynapseId)->GetBranchId())->triggerCount.at(synapseDataResources.at(spikedSynapseId)->GetBranchPositionId())=0;
}

std::valarray<double> BranchedResourceSTDPAsymmetric::GetIndividualSynapticProfile(unsigned long synapseId) const
{
    return std::valarray<double>();
}

std::shared_ptr<BaseSynapseSpine> BranchedResourceSTDPAsymmetric::AllocateNewSynapse(HeteroCurrentSynapse &synapse)
{
    //here I have to set the maxcount of the spine to maxCount too 
    //Here sum over the branches.
    //And cast the proper pointers to the proper synapseData vectors.
    return std::shared_ptr<BaseSynapseSpine>();
}
