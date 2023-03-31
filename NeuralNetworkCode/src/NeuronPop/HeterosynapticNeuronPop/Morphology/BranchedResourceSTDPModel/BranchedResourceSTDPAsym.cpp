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
    return;
}

void BranchedResourceSTDPAsymmetric::SaveParameters(std::ofstream *stream, std::string neuronPreId)
{
    BranchedMorphology::SaveParameters(stream, neuronPreId);
    *stream << neuronPreId<<"_morphology_available_weight\t\t"<<std::to_string(this->branchings);//CHANGE
    *stream << "\t"<<"#Total weight available to be distributed among synapses per time-step.\n";
    return;
}

void BranchedResourceSTDPAsymmetric::SetUpBranchings(int remainingBranchingEvents, std::vector<int> anteriorBranches)
{
    //This is a recursive function that sets up the branched dendritic tree and is generalized for 0 branchings (1 branch). This function has been unit tested by Antoni.
    remainingBranchingEvents-=1;
    //First call is done with an empty int vector
    for (int i = 0; i < 2;i++) {
        int branchId{this->GenerateBranchId()};
        this->resourceBranches.emplace_back(std::make_shared<ResourceBranch>(ResourceBranch(this->synapticGap, this->branchLength, anteriorBranches, branchId, MaxCountSTDP, branchMaxCountTrigger, this->resourceSynapseData)));//This vector should be sorted by ID by default (tested).
        this->branches.push_back(static_cast<std::shared_ptr<Branch>>(this->resourceBranches.back()));
        //Constructor here
        if(remainingBranchingEvents>0){
            std::vector<int> anteriorBranchesCopy(anteriorBranches);
            anteriorBranchesCopy.push_back(branchId);
            this->SetUpBranchings(remainingBranchingEvents, anteriorBranchesCopy);
        }
    }
    return;
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
        DecayHashTableSTDP[STDPindex]=std::exp(-(STDPindex*info->dt)/DecayConstantSTDP);
    }
    return;
}

void BranchedResourceSTDPAsymmetric::advect()
{
        //If STDPDEpression count is not 10, immediately trigger STDP on spine trigger (while also flagging)
    for (std::shared_ptr<ResourceBranch>& branch : resourceBranches){
        DetectPossiblePairing(branch);
    }
    ApplyEffects();
    Reset();
    return;
}


void BranchedResourceSTDPAsymmetric::ApplyEffects() //Called after pairings
{
    //If first precount is bigger than twice the STDP depression count, depression. Otherwise potentiation.
    if (this->postSpiked){
        for (std::shared_ptr<ResourceSynapseSpine>& synapse : resourceSynapseData){
        STDPPotentiation(synapse);
        }
    } else if (this->STDPDepressionCount<this->MaxCountSTDP){ //Count is supposed to stop
        for (std::shared_ptr<ResourceBranch>& branch: resourceBranches){
            for (int synapseID : branch->updatedAlphaEffects){
                std::shared_ptr<ResourceSynapseSpine>& synapse = resourceSynapseData.at(branch->synapseSlotToMorphoIDMap.at(synapseID));
                if (synapse->GetDepressionFlagSTDP()){
                    STDPDepression(synapse);
                } else if (synapse->GetPotentiationFlagSTDP()){
                    STDPPotentiation(synapse);
                }
            }
        }
    } else {
        return;
        }
    //Remember to call the branch plasticity counter for every event!!!
    //if this postSpike bool
}

void BranchedResourceSTDPAsymmetric::STDPPotentiation(std::shared_ptr<ResourceSynapseSpine>& synapse)
{
    //If post spike, apply all stimms on positive mode (remember the coded function in spines)
    //Use the count in the effects of synapses for the actual decay for STDP, but the branch vector for detecting the updatable ones
    //WITH DECAY
    return;
}

void BranchedResourceSTDPAsymmetric::STDPDepression(std::shared_ptr<ResourceSynapseSpine>& synapse)
{
     //If count <10, apply stimms in negative mode. Basically input -1/STDPratio to the spine function. Has to be limited to zero alpha stimm or zero alpha, never negative
     //And also pass the STDP map by reference to the function call
     //Use STDPcount in the morpho for STDP decay
     //Iterate over set of updatedAlphas
     return;
}

void BranchedResourceSTDPAsymmetric::DetectPossiblePairing(std::shared_ptr<ResourceBranch> branch)//These are the spiked neurons on synapseDataIndexes
{
    //Here we call SetTempEffects if __it__ happens
    for (int synapseIDinBranch : branch->spikedSynapsesInTheBranch){
        if (CheckIfThereIsPairing(branch, synapseIDinBranch)){
            SpaceTimeKernel(synapseIDinBranch, branch->branchId, branch->synapseSlotToMorphoIDMap.at(synapseIDinBranch));
            branch->updatedAlphaEffects.insert(synapseIDinBranch);
        }
    }
    return;
}

bool BranchedResourceSTDPAsymmetric::CheckIfThereIsPairing(std::shared_ptr<ResourceBranch> branch, int synapseIDinBranch)
{
    return std::count(std::max(branch->triggerCount.begin(), std::next(branch->triggerCount.begin(),synapseIDinBranch-kernelGapNumber)),std::min(branch->triggerCount.end(), std::next(branch->triggerCount.begin(),synapseIDinBranch+kernelGapNumber+1)), [this](int pairingCounter){return pairingCounter<this->branchMaxCountTrigger;})>2;
}

void BranchedResourceSTDPAsymmetric::SpaceTimeKernel(int branchSynapseID, int branchID, int synapseSpineIDinMorpho)
{
    int synapsePositionIndexInBranch,absDistance,timeStepDifference;
    double alphaStimmulusEffect;
    std::set<int>& kernelizedSynapses = resourceBranches.at(branchID)->updatedSynapseSpines;
    std::set<int>& updatedAlphaEffects = resourceBranches.at(branchID)->updatedAlphaEffects;
    size_t& branchSlots{branches.at(branchID)->branchSlots};
    std::shared_ptr<ResourceSynapseSpine>& centralSynapseSpine = resourceSynapseData.at(synapseSpineIDinMorpho);
    //Only call this function if synapse itself has spiked this timestep
    for (int gapIndex = -kernelGapNumber; gapIndex<=kernelGapNumber;gapIndex++){ //And then use branchSynapseID+gapindex
        synapsePositionIndexInBranch = branchSynapseID+gapIndex;
        if (synapsePositionIndexInBranch<0){
            continue;
        } else if (synapsePositionIndexInBranch>branchSlots){
            break;
        } else {
            std::shared_ptr<ResourceSynapseSpine>& lateralSynapseSpine = resourceSynapseData.at(branches.at(branchID)->synapseSlotToMorphoIDMap.at(synapsePositionIndexInBranch));
            absDistance = std::abs(gapIndex);
            timeStepDifference=resourceBranches.at(branchID)->triggerCount.at(synapsePositionIndexInBranch);
            if (timeStepDifference<STDPDepressionCount){//Indicates depression region with no conflict
                centralSynapseSpine->SetDepressionFlag(true);
                lateralSynapseSpine->SetDepressionFlag(true);
            } else { //Knowing depression region, indicates conflict that is skewed towards potentiation according to STDP kernel
                if (timeStepDifference<2*STDPDepressionCount){
                    centralSynapseSpine->SetPotentiationFlag(true);
                    lateralSynapseSpine->SetPotentiationFlag(true);
                } else {
                    centralSynapseSpine->SetDepressionFlag(true);
                    lateralSynapseSpine->SetDepressionFlag(true);
                }
            }
            if(timeStepDifference+synapticGap*absDistance<=kernelRadius && (kernelizedSynapses.find(synapsePositionIndexInBranch) != kernelizedSynapses.end())){        //triggercount+distance gap*spaceTimeStepRelation<maxCountTime + 1*spaceTimeStepRelation (what should constrain the triangular matrix)
                alphaStimmulusEffect = baseAlphaStimmulusBump*CallKernelHashTable(absDistance, timeStepDifference);
                centralSynapseSpine->AddTempResourcesToSpine(alphaStimmulusEffect);
                lateralSynapseSpine->AddTempResourcesToSpine(alphaStimmulusEffect);
                updatedAlphaEffects.insert(synapsePositionIndexInBranch);
            }
        }
        
    }
    kernelizedSynapses.insert(branchSynapseID);//Used to avoid double potentiation in same timestep
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

void BranchedResourceSTDPAsymmetric::Reset()
{
    BranchedMorphology::Reset();
    //Wrapper plus clearing some of the vectors. Last method to run in chronological order, where we call the ticks and the general upkeep
    //Check increase beta resources here? Not necessary anymore
    //Clear both sets in every branch DONE
    //REVIEW if there are any remaining things to put in this function
    ClearSynapseSets();
    for (std::shared_ptr<ResourceBranch>& branch : resourceBranches){
        RecalcWeights(branch);
    }
    DeleteEffects();
    TickAllCounts();
    return;
}

void BranchedResourceSTDPAsymmetric::RecalcAlphas(std::shared_ptr<ResourceBranch> branch)
{
    //Here we just need to apply all delta alphas, decay them (before makes more sense, the bump has delta t zero.), sum the result to stationary alpha, then update alpha sums? Yes
    for (std::shared_ptr<ResourceSynapseSpine> synapse : branch->branchSynapseData){
        synapse->RecalculateAlphaResources();
    }
    return;
}

void BranchedResourceSTDPAsymmetric::RecalcWeights(std::shared_ptr<ResourceBranch> branch) //This one is the one we call for every branch
{
    RecalcAlphaSums(branch);
    branch->weightResourceFactor=betaResourcePool/(omegaPassiveResourceOffset+branch->alphaTotalSum);//IMPORTANT, if we make beta non-constant, beta must be referenced from the branch
    for (std::shared_ptr<ResourceSynapseSpine>& synapse : branch->branchSynapseData){
        synapse->RecalcWeight(branch->weightResourceFactor);
    }
    return;
}

void BranchedResourceSTDPAsymmetric::RecalcAlphaSums(std::shared_ptr<ResourceBranch> branch)
{
    RecalcAlphas(branch);
    branch->alphaTotalSum=std::accumulate(branch->branchSynapseData.begin(), branch->branchSynapseData.end(), 0.0,//UNRESOLVED, does this give intended output?
                                       [] (double acc, const std::shared_ptr<ResourceSynapseSpine>& synapse) {return acc + synapse->GetAlphaResources();});
    return;
}
void BranchedResourceSTDPAsymmetric::DeleteEffects()
{
    for (std::shared_ptr<ResourceSynapseSpine>& synapse : resourceSynapseData){
        synapse->CullStimmulusVectors();
        synapse->SetDepressionFlag(false);
        synapse->SetPotentiationFlag(false);
    }
    return;
}
void BranchedResourceSTDPAsymmetric::TickAllCounts()
{
    for (std::shared_ptr<ResourceBranch>& branch : resourceBranches){
        branch->TickAllCounts();
    }
    for (std::shared_ptr<ResourceSynapseSpine>& synapse: resourceSynapseData){
     synapse->TickStimmulusCounts();   
    }
    STDPDepressionCount++;//If it is unbound, no bool checks 
    // if (STDPDepressionCount<MaxCountSTDP){
    //     STDPDepressionCount++;
    // }
    return;
}

void BranchedResourceSTDPAsymmetric::ClearSynapseSets()
{
    for (std::shared_ptr<ResourceBranch>& branch: resourceBranches){
        branch->updatedAlphaEffects.clear();
        branch->updatedSynapseSpines.clear();
    }
    return;
}

void BranchedResourceSTDPAsymmetric::RecordPostSpike()
{
    BranchedMorphology::RecordPostSpike();
    STDPDepressionCount=0;
    return;
}

void BranchedResourceSTDPAsymmetric::RecordExcitatoryPreSpike(int spikedSynapseId)
{
    //Here only record, afterwards we do the checks
    BranchedMorphology::RecordExcitatoryPreSpike(spikedSynapseId);
    this->resourceBranches.at(resourceSynapseData.at(spikedSynapseId)->GetBranchId())->triggerCount.at(resourceSynapseData.at(spikedSynapseId)->GetBranchPositionId())=0;
    return;
}

std::shared_ptr<BaseSynapseSpine> BranchedResourceSTDPAsymmetric::AllocateNewSynapse(HeteroCurrentSynapse &synapse)
{
    //here I have to set the maxcount of the spine to maxCount too 
    //Here sum over the branches.????
    //And cast the proper pointers to the proper baseSynapseData vectors.
    std::shared_ptr<ResourceSynapseSpine> newSynapse;
    newSynapse = std::make_shared<ResourceSynapseSpine>();
    //Step weights has been removed fron here
    newSynapse->SetWeight(this->GenerateSynapticWeight());

    //this->weightsSum += newSynapse->GetWeight();
    newSynapse->SetIdInMorpho(this->synapseIdGenerator++);
    //Branch
    int branch {AllocateBranch(synapse)};
    if (branches.at(branch)->openSynapsesSlots.size()==0){
        throw noAllocatableSynapseException();
    }
    //Position
    int position{branches.at(branch)->openSynapsesSlots.front()};
    branches.at(branch)->openSynapsesSlots.pop_front();
    newSynapse->SetBranchPositionId(position);
    //newSynapse->SetDistanceFromNode(position*branches.at(branch)->synapticGap);//This has to be updated if we switch to double 
    newSynapse->SetMaxCount(MaxCountSTDP);
    newSynapse->SetAlphaBasal(alphaBasal);
    newSynapse->SetAlphaStimmulusRest(alphaStimmulusRest);
    newSynapse->SetAlphaExpDecay(alphaStimmulusExpDecay);
    branches.at(branch)->synapseSlotClosedIndex.push_back(position);
    //branches.at(branch)->morphoSynapseIDs.push_back(newSynapse->GetIdInMorpho());
    branches.at(branch)->synapseSlotToMorphoIDMap.at(position)=newSynapse->GetIdInMorpho();
    //Storage (other)
    this->baseSynapseData.push_back(static_cast<std::shared_ptr<BaseSynapseSpine>>(newSynapse));
    this->branchedSynapseData.push_back(static_cast<std::shared_ptr<BranchedSynapseSpine>>(newSynapse));
    this->resourceSynapseData.push_back(newSynapse);

    return static_cast<std::shared_ptr<BaseSynapseSpine>>(newSynapse);
}
