#include "./BranchedResourceHeteroSTDP.hpp"
#include "BranchedResourceHeteroSTDP.hpp"

BranchedResourceHeteroSTDP::BranchedResourceHeteroSTDP(GlobalSimInfo *info):BranchedMorphology(info)
{
}

void BranchedResourceHeteroSTDP::LoadParameters(std::vector<std::string> *input)
{
    std::string name;
    std::vector<std::string> values;
    
    BranchedMorphology::LoadParameters(input);

    for (auto & it : *input) {
        SplitString(&it,&name,&values);
        if (name.find("basal_alpha") != std::string::npos){
            this->alphaBasal = std::stod(values.at(0));
        } else if (name.find("alpha_decay_constant") != std::string::npos){
            this->alphaStimmulusExpTau = std::stod(values.at(0));
            this->alphaStimmulusExpDecay = std::exp(-this->info->dt/this->alphaStimmulusExpTau);
        } else if (name.find("base_alpha_increase") != std::string::npos){
            this->baseAlphaStimmulusBump = std::stod(values.at(0));
        } else if (name.find("omega_offset") != std::string::npos){
            this->omegaPassiveResourceOffset = std::stod(values.at(0));
        } else if(name.find("beta_resource_pool") != std::string::npos){
            this->betaResourcePool = std::stod(values.at(0));
        } else if (name.find("kernel_spatial_length") != std::string::npos){
            this->kernelGapNumber = static_cast<int>(std::stod(values.at(0))/this->synapticGap);
        } else if (name.find("kernel_temporal_length") != std::string::npos){
            this->timeKernelLength = static_cast<int>(std::stod(values.at(0))/this->info->dt);
        } else if (name.find("kernel_tau") != std::string::npos){
            this->timeKernelExpDecayConstant = std::stod(values.at(0));
        } else if (name.find("kernel_lambda") != std::string::npos){
            this->spaceKernelExpDecayConstant = std::stod(values.at(0));
        } else if (name.find("STDP_tau") != std::string::npos){
            this->ExpDecayConstantSTDP = std::stod(values.at(0));
        } else if (name.find("potentiation_depression_ratio") != std::string::npos){
            this->PotentiationDepressionRatio = std::stod(values.at(0));
        } else if (name.find("pairing_time_window") != std::string::npos){
            this->branchMaxCountTrigger = static_cast<int>(std::stod(values.at(0))/this->info->dt);
        } else if (name.find("STDP_time_window") != std::string::npos){
            this->MaxCountSTDP = static_cast<int>(std::stod(values.at(0))/this->info->dt);
        }
    }
    SetUpHashTables();
}

void BranchedResourceHeteroSTDP::SaveParameters(std::ofstream *stream, std::string neuronPreId)
{
    BranchedMorphology::SaveParameters(stream, neuronPreId);
    *stream << neuronPreId<<"_morphology_basal_alpha\t\t"<<std::to_string(this->alphaBasal);
    *stream << "\t"<<"#Alpha at rest, where alpha decays towards\n";

    *stream << neuronPreId<<"_morphology_alpha_decay_constant\t\t"<<std::to_string(this->alphaStimmulusExpTau);
    *stream << "\t"<<"#Decay constant of Alpha_stimmulus\n";

    *stream << neuronPreId<<"_morphology_base_alpha_increase\t\t"<<std::to_string(this->baseAlphaStimmulusBump);
    *stream << "\t"<<"#Default alphaStimmulus increase before applying spatial and temporal decays\n";

    *stream << neuronPreId<<"_morphology_omega_offset\t\t"<<std::to_string(this->omegaPassiveResourceOffset);
    *stream << "\t"<<"#Offset factor in the weight definition.\n";

    *stream << neuronPreId<<"_morphology_beta_resource_pool\t\t"<<std::to_string(this->betaResourcePool);
    *stream << "\t"<<"#Multiplication factor of the definition of weight, representing the available 'total resources'.\n";

    *stream << neuronPreId<<"_morphology_kernel_spatial_length\t\t"<<std::to_string(this->kernelGapNumber*this->synapticGap);
    *stream << "\t"<<"#Limit distance between two spines to be considered for synaptic spine pairing.\n";

    *stream << neuronPreId<<"_morphology_kernel_temporal_length\t\t"<<std::to_string(this->timeKernelLength*this->info->dt);//CHANGE
    *stream << "\t"<<"#Maximum time length between two spikes in contiguous spines to be considered for synaptic spine pairing\n";

    *stream << neuronPreId<<"_morphology_kernel_tau\t\t"<<std::to_string(this->timeKernelExpDecayConstant);//CHANGE
    *stream << "\t"<<"#Time decay constant for the alpha stimmulus increase in the pairing kernel\n";

    *stream << neuronPreId<<"_morphology_kernel_lambda\t\t"<<std::to_string(this->spaceKernelExpDecayConstant);//CHANGE
    *stream << "\t"<<"#Space decay constant for the alpha stimmulus increase in the pairing kernel (analogous to tau)\n";

    *stream << neuronPreId<<"_morphology_STDP_tau\t\t"<<std::to_string(this->ExpDecayConstantSTDP);//CHANGE
    *stream << "\t"<<"#Exponential decay constant for the STDP kernel. The starting point is the decayed alpha stimmulus\n";

    *stream << neuronPreId<<"_morphology_potentiation_depression_ratio\t"<<std::to_string(this->PotentiationDepressionRatio);//CHANGE
    *stream << "\t"<<"#Factor that multiplies potentiation\n";

    *stream << neuronPreId<<"_morphology_pairing_time_window\t\t"<<std::to_string(this->branchMaxCountTrigger*this->info->dt);//CHANGE
    *stream << "\t"<<"#Max time where synaptic spine pairing can happen\n";

    *stream << neuronPreId<<"_morphology_STDP_time_window\t\t"<<std::to_string(this->MaxCountSTDP*this->info->dt);//CHANGE
    *stream << "\t"<<"#Max time where STDP potentiation/depression can happen\n";

    *stream <<"##### The weight of this model is defined as wI=beta*alphaI/(omega+branch-sum(alpha)), where alphaI= alphaBasal + alphaStimmulus*exp(-dt/alphaStimTau) \n";
}

void BranchedResourceHeteroSTDP::SetUpBranchings(int remainingBranchingEvents, std::vector<int> anteriorBranches)
{
    //This is a recursive function that sets up the branched dendritic tree and is generalized for 0 branchings (1 branch). This function has been unit tested by Antoni.
    remainingBranchingEvents-=1;
    //First call is done with an empty int vector
    for (int i = 0; i < 2;i++) {
        int branchId{this->GenerateBranchId()};
        this->resourceBranches.emplace_back(std::make_shared<ResourceBranch>(ResourceBranch(this->synapticGap, this->branchLength, anteriorBranches, branchId, this->resourceSynapseData)));//This vector should be sorted by ID by default (tested).
        this->branches.push_back(static_cast<std::shared_ptr<Branch>>(this->resourceBranches.back()));
        //Constructor here
        if(remainingBranchingEvents>0){
            std::vector<int> anteriorBranchesCopy(anteriorBranches);
            anteriorBranchesCopy.push_back(branchId);
            this->SetUpBranchings(remainingBranchingEvents, anteriorBranchesCopy);
        }
    }
}

void BranchedResourceHeteroSTDP::SetUpHashTables()
{
//Statement 2 of second for loop is done thinking about time steps being in the order of magnitude of 0.1 ms, while the gap is 1 um. 
//If we make the kernel triangular, every "step" is one gap in space and in time it can be any amount of timesteps. To calculate the step relationship, we do:
    if ((static_cast<double>(timeKernelLength)/kernelGapNumber)<1.0){
        throw; //The timeKernel lenght is always thought to be longer than the spatial one. 1 ms timing for 10 um is very one-sided, and that is equilibrium
    }
    spaceTimeStepRelation = timeKernelLength/kernelGapNumber;
    kernelRadius=timeKernelLength+spaceTimeStepRelation;
    for (int spaceIndex=0; spaceIndex<=kernelGapNumber; spaceIndex++){//at least there is one gap
        for (int timeIndex=0; timeIndex<=timeKernelLength-(spaceTimeStepRelation*(spaceIndex-1)); timeIndex++){ 
            kernelHashTable.at(timeIndex).at(spaceIndex)=std::exp(-(synapticGap*spaceIndex)/spaceKernelExpDecayConstant)*std::exp(-(info->dt*timeIndex)/timeKernelExpDecayConstant);
            //Always access time, space later (always will be more times than gaps)
        }
    }

    for (int STDPindex=1; STDPindex<=MaxCountSTDP; STDPindex++){
        DecayHashTableSTDP.at(STDPindex)=std::exp(-(STDPindex*info->dt)/ExpDecayConstantSTDP);
    }
}

void BranchedResourceHeteroSTDP::advect()
{
        //If STDPDEpression count is not 10, immediately trigger STDP on spine trigger (while also flagging)
    if (!this->postSpiked){
        for (std::shared_ptr<ResourceBranch>& branch : resourceBranches){
        DetectPossiblePairing(branch);
        }
    }
    ApplyEffects();
    Reset();
    return;
}


void BranchedResourceHeteroSTDP::ApplyEffects() //Called after pairings
{
    //If first precount is bigger than twice the STDP depression count, depression. Otherwise potentiation.
    if (this->postSpiked){
         //If post spike, apply all stimms on positive mode (remember the coded function in spines) with the decay from STDP pot count. 
        //Use the count in the effects of synapses for the actual decay for STDP, but the branch vector for detecting the updatable ones
        //WITH DECAY (of alpha, STDP-like)
        for (std::shared_ptr<ResourceSynapseSpine>& synapse : resourceSynapseData){
        synapse->ApplyAllTempEffectsOnPostspike(PotentiationDepressionRatio, DecayHashTableSTDP);
        branches.at(synapse->GetBranchId())->IncreasePotentiationCount();
        }
    } else if (this->STDPDepressionCount<this->MaxCountSTDP){ //Count is supposed to stop
        for (std::shared_ptr<ResourceBranch>& branch: resourceBranches){
            for (int synapseID : branch->updatedAlphaEffects){
                std::shared_ptr<ResourceSynapseSpine>& synapse = resourceSynapseData.at(branch->synapseSlotToMorphoIDMap.at(synapseID));
                //if (synapse->GetDepressionFlagSTDP()){
                    //If count < countmax, apply stimms in negative mode. Basically input -1/STDPratio?? to the spine function. Has to be limited to zero alpha stimm or zero alpha, never negative
                    //And also pass the STDP map by reference to the function call
                    //Use STDPcount in the morpho for STDP decay
                    //Iterate over set of updatedAlphas
                    //Decay STDP but expressed as a negative number
                synapse->ApplyAllTempEffectsOnDepression(DecayHashTableSTDP, STDPDepressionCount);
                //} else if (synapse->GetPotentiationFlagSTDP()){
                //    synapse->ApplyAllTempEffectsOnConflictPotentiation(PotentiationDepressionRatio);
                //}
                branch->IncreaseDepressionCount();
            }
        }
    } else {
        return;
        }
    //Remember to call the branch plasticity counter for every event!!!
    //if this postSpike bool
}

void BranchedResourceHeteroSTDP::DetectPossiblePairing(std::shared_ptr<ResourceBranch> branch)//These are the spiked neurons on synapseDataIndexes
{
    //Here we call SetTempEffects if __it__ happens
    for (int synapseIDinBranch : branch->spikedSynapsesInTheBranch){
        if (CheckIfThereIsPairing(branch, synapseIDinBranch)){
            SpaceTimeKernel(synapseIDinBranch, branch->branchId, branch->synapseSlotToMorphoIDMap.at(synapseIDinBranch));
            branch->updatedAlphaEffects.insert(synapseIDinBranch);
        }
    }
}

bool BranchedResourceHeteroSTDP::CheckIfThereIsPairing(std::shared_ptr<ResourceBranch> branch, int synapseIDinBranch)
{
    return std::count_if(std::max(branch->triggerCount.begin(), std::next(branch->triggerCount.begin(),synapseIDinBranch-kernelGapNumber)),std::min(branch->triggerCount.end(), std::next(branch->triggerCount.begin(),synapseIDinBranch+kernelGapNumber+1)), [this](int pairingCounter){return pairingCounter<this->branchMaxCountTrigger;})>2;
}

void BranchedResourceHeteroSTDP::SpaceTimeKernel(int branchSynapseID, int branchID, int synapseSpineIDinMorpho)
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
        alphaStimmulusEffect = baseAlphaStimmulusBump;
        if (synapsePositionIndexInBranch<0){//Condition to aboid illegal indexing (correct me if you dare)
            continue;
        } else if (synapsePositionIndexInBranch>branchSlots){//Condition to aboid illegal indexing (correct me if you dare)
            break;
        } else {
            std::shared_ptr<ResourceSynapseSpine>& lateralSynapseSpine = resourceSynapseData.at(branches.at(branchID)->synapseSlotToMorphoIDMap.at(synapsePositionIndexInBranch));
            absDistance = std::abs(gapIndex);
            timeStepDifference=resourceBranches.at(branchID)->triggerCount.at(synapsePositionIndexInBranch);
            //timeStepDifference == triggerCount of first spine
            // if (timeStepDifference<=STDPDepressionCount){//Indicates depression region with no conflict. If the spike is far away, but too far for depression, nothing will happen. If a postspike happens, these flags are ignored.
            //     //The boolean is <= to classify the pairing where the postspike and first prespike are on the same timestep as depression.
            //     // centralSynapseSpine->SetDepressionFlag(true);
            //     // lateralSynapseSpine->SetDepressionFlag(true);
            //     //alphaStimmulusEffect *= -DecayHashTableSTDP.at(STDPDepressionCount-timeStepDifference);//This is to apply a decay equivalent to the STDP kernel to the first pre-spike with depression (*-1) (calcium accumulation oriented)
            // } else { //Knowing depression region, indicates conflict that is skewed towards potentiation according to STDP kernel
            //     if (timeStepDifference<=2*STDPDepressionCount){//We set the boolean to equal too to solve the conflict in favor of potentiation, as depression has the other fringe case
            //         centralSynapseSpine->SetPotentiationFlag(true);
            //         lateralSynapseSpine->SetPotentiationFlag(true);
            //         alphaStimmulusEffect *= DecayHashTableSTDP.at(timeStepDifference-STDPDepressionCount);//This is to apply a decay equivalent to the STDP kernel to the first pre-spike  (calcium accumulation oriented)
            //     } else {
            //         centralSynapseSpine->SetDepressionFlag(true);
            //         lateralSynapseSpine->SetDepressionFlag(true);
            //     }//These flags are only here to solve 
            //}
            // if (STDPDepressionCount<MaxCountSTDP){
            //     alphaStimmulusEffect=(-alphaStimmulusEffect);
            // }
            if(timeStepDifference+synapticGap*absDistance<=kernelRadius && (kernelizedSynapses.find(synapsePositionIndexInBranch) != kernelizedSynapses.end())){        //triggercount+distance gap*spaceTimeStepRelation<maxCountTime + 1*spaceTimeStepRelation (what should constrain the triangular matrix)
                //The postspiked condition is because the STDP kernel is discretized, in the case the postspike happens at the same time as the pairing of a synapse, the pairing does not happen (as the potentiation and depression would counteract themselves)
                alphaStimmulusEffect *= CallKernelHashTable(absDistance, timeStepDifference);
                centralSynapseSpine->AddTempResourcesToSpine(alphaStimmulusEffect);
                lateralSynapseSpine->AddTempResourcesToSpine(alphaStimmulusEffect);
                updatedAlphaEffects.insert(synapsePositionIndexInBranch);
            }
        }
        
    }
    kernelizedSynapses.insert(branchSynapseID);//Used to avoid double potentiation in same timestep
    //Here for every synapse inside the synapse's kernel that has an active counter (count!=countMax) we get the time kernel and then apply the space kernel
    //Take unto account the synaptic GAP and DT!!! This should be done elsewhere
}
double BranchedResourceHeteroSTDP::CallKernelHashTable(int distanceToCenterInGaps, int timeDifference)
{
    //Matrix is symmetric!! Only have to do half!
    //Here we tabulate for every distance from center a equivalency between counts and effects (reverse as counts go up to maxCount)
    //The pair consists of ns and ks. The function has to obtain the count from synapse and distance?
    //OR the hashtable contains the ns and ks multipliers (for now they are the same)
    return kernelHashTable.at(timeDifference).at(distanceToCenterInGaps);
}

void BranchedResourceHeteroSTDP::Reset()
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
}

void BranchedResourceHeteroSTDP::RecalcAlphas(std::shared_ptr<ResourceBranch> branch)
{
    //Here we just need to apply all delta alphas, decay them (before makes more sense, the bump has delta t zero.), sum the result to stationary alpha, then update alpha sums? Yes
    for (std::shared_ptr<ResourceSynapseSpine> synapse : branch->branchSynapseData){
        synapse->RecalculateAlphaResources();
    }
    return;
}

void BranchedResourceHeteroSTDP::RecalcWeights(std::shared_ptr<ResourceBranch> branch) //This one is the one we call for every branch
{
    RecalcAlphaSums(branch);
    branch->weightResourceFactor=betaResourcePool/(omegaPassiveResourceOffset+branch->alphaTotalSum);//IMPORTANT, if we make beta non-constant, beta must be referenced from the branch
    for (std::shared_ptr<ResourceSynapseSpine>& synapse : branch->branchSynapseData){
        synapse->RecalcWeight(branch->weightResourceFactor);
    }
    return;
}

void BranchedResourceHeteroSTDP::RecalcAlphaSums(std::shared_ptr<ResourceBranch> branch)
{
    RecalcAlphas(branch);
    branch->alphaTotalSum=std::accumulate(branch->branchSynapseData.begin(), branch->branchSynapseData.end(), 0.0,//UNRESOLVED, does this give intended output?
                                       [] (double acc, const std::shared_ptr<ResourceSynapseSpine>& synapse) {return acc + synapse->GetAlphaResources();});
    return;
}
void BranchedResourceHeteroSTDP::DeleteEffects()
{
    for (std::shared_ptr<ResourceSynapseSpine>& synapse : resourceSynapseData){
        synapse->CullStimmulusVectors();
        // synapse->SetDepressionFlag(false);
        // synapse->SetPotentiationFlag(false);
    }
    return;
}
void BranchedResourceHeteroSTDP::TickAllCounts()
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

void BranchedResourceHeteroSTDP::ClearSynapseSets()
{
    for (std::shared_ptr<ResourceBranch>& branch: resourceBranches){
        branch->updatedAlphaEffects.clear();
        branch->updatedSynapseSpines.clear();
    }
    return;
}

void BranchedResourceHeteroSTDP::RecordPostSpike()
{
    BranchedMorphology::RecordPostSpike();
    STDPDepressionCount=0;
    return;
}

void BranchedResourceHeteroSTDP::RecordExcitatoryPreSpike(int spikedSynapseId)
{
    //This function is NOT DELAY COMPATIBLE (careful with the delays in synapse objects)
    //Here only record, afterwards we do the checks
    BranchedMorphology::RecordExcitatoryPreSpike(spikedSynapseId);
    this->resourceBranches.at(resourceSynapseData.at(spikedSynapseId)->GetBranchId())->triggerCount.at(resourceSynapseData.at(spikedSynapseId)->GetBranchPositionId())=0;
    return;
}

std::shared_ptr<BaseSynapseSpine> BranchedResourceHeteroSTDP::AllocateNewSynapse(HeteroCurrentSynapse &synapse)
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
    newSynapse->SetDistanceFromNode(position*branches.at(branch)->synapticGap);//This has to be updated if we switch to double 
    newSynapse->SetMaxCount(MaxCountSTDP);
    newSynapse->SetAlphaBasal(alphaBasal);
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

std::valarray<double> BranchedResourceHeteroSTDP::GetOverallSynapticProfile()
{
    /*
     * returned array organised as follows:
     * item 1: average synaptic weight
     * item 2: totalLTD Events
     * item 3: totalLTP Events
     * item 4: average plasticity events
     * */
    std::valarray<double> dataArray(4);
    size_t sizeOfSynapseData {this->baseSynapseData.size()};
    double weightSum = std::accumulate(this->baseSynapseData.begin(), this->baseSynapseData.end(), 0.0,
                                       [] (double acc, const std::shared_ptr<BaseSynapseSpine>& syn) { return acc + syn->GetWeight(); });
    CalcMorphoPlasticityEvents();

   dataArray[0] = weightSum / sizeOfSynapseData;
   dataArray[1] = this->totalLTDEvents;
   dataArray[2] = this->totalLTPEvents;
   dataArray[3] = static_cast<double>(totalPlasticityEvents) / sizeOfSynapseData;
   return dataArray;
}

std::string BranchedResourceHeteroSTDP::GetOverallSynapticProfileHeaderInfo() const
{
    return std::string("{<average weight>, <total post spikes>, <total pre spikes>, <average number of plasticity events>}");
}

void BranchedResourceHeteroSTDP::CalcMorphoPlasticityEvents()
{
    totalLTDEvents = std::accumulate(this->branches.begin(), this->branches.end(), 0,
                                       [] (double acc, const std::shared_ptr<Branch>& branch) { return acc + branch->LTDevents; });
    totalLTPEvents = std::accumulate(this->branches.begin(), this->branches.end(), 0,
                                       [] (double acc, const std::shared_ptr<Branch>& branch) { return acc + branch->LTPevents; });
    totalPlasticityEvents = totalLTDEvents + totalLTPEvents;
}
