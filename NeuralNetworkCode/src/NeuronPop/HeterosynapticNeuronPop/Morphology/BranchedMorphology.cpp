#include "./BranchedMorphology.hpp"



BranchedMorphology::BranchedMorphology(GlobalSimInfo * info): Morphology(info){

}

void BranchedMorphology::recordPostSpike() {
    Morphology::recordPostSpike();
    this->postSpiked = true;
}

void BranchedMorphology::recordExcitatoryPreSpike(unsigned long synSpikerId) {
    Morphology::recordExcitatoryPreSpike(synSpikerId);
    this->branches.at(this->synapseData.at(synSpikerId)->getBranchId())->spikedSyn.at(this->synapseData.at(synSpikerId)->getBranchPositionId())=true;
}

void BranchedMorphology::reset()
{
    Morphology::reset();
    for (std::shared_ptr<Branch> branch : branches){
        std::fill(branch->spikedSyn.begin(), branch->spikedSyn.end(), false);
    }
    this->postSpiked=false;
}

void BranchedMorphology::LoadParameters(std::vector<std::string> *input) {
    Morphology::LoadParameters(input);

    std::string name;
    std::vector<std::string> values;

    bool dendriteInitialized = false,
        synapticGapInitialized = false,
        branchingsInitialized = false;

    for (auto & it : *input) {
        SplitString(&it,&name,&values);

        if(name.find("branch_length") != std::string::npos){
            this->branchLength = std::stoi(values.at(0));
            dendriteInitialized = true;
        } else if (name.find("synaptic_gap") != std::string::npos) {
            this->synapticGap = std::stoi(values.at(0));
            synapticGapInitialized = true;
        } else if (name.find("distribute_weights") != std::string::npos) {
            //This whole part is experimental, it seems it was not completely tested
            //As such, this is deprecated from publication
            if (values.at(0) == "true") {
                this->distributeWeights = true;
            } else {
                    this->initialWeights = std::stod(values.at(1));
            }
        } else if (name.find("dendrite_branchings") != std::string::npos) {
            this->branchings = std::stoi(values.at(0));
            if (this->branchings>28){
                //EXCEPTION, integer overflow.
                throw;
            }
            branchingsInitialized=true;
        } else if (name.find("synapse_allocation") != std::string::npos){
            if (values.at(0).find("ordered") != std::string::npos){
                this->orderedSynAllocationB=true;
            }else if (values.at(0).find("random") != std::string::npos){
                this->randomSynAllocationB=true;
            }
        } else if (name.find("seed") != std::string::npos) {
            this->seed = std::stoi(values.at(0));
            this->generator=std::default_random_engine(this->seed);
        }/*else if (name.find("branch_allocation") != std::string::npos){
            if (values.at(0).find("ordered") != std::string::npos){
                orderedBranchAllocationB=true;
            } else if (values.at(0).find("set") != std::string::npos){
                setBranchAllocationB=true;
            } else if (values.at(0).find("random") != std::string::npos){
                randomBranchAllocationB=true;
            }
        }*/
    }
    //Improve exception handling
    assertm(dendriteInitialized, "Using heterosynaptic synapses without specifying dendritic_length is not allowed.");
    assertm(synapticGapInitialized, "Using heterosynaptic synapses without specifying synaptic_gap is not allowed.");
    assertm(branchingsInitialized, "Using branched morphology with no branchings specified.");
    if(info->globalSeed != -1){
        std::uniform_int_distribution<int> distribution(0,INT32_MAX);
        this->seed = distribution(info->globalGenerator);
        this->generator = std::default_random_engine(seed);
    }
    if (!orderedSynAllocationB && !randomSynAllocationB){throw;}
    //if (!orderedBranchAllocationB && !randomBranchAllocationB && !setBranchAllocationB){throw;}
    setUpMorphology();
}

void BranchedMorphology::setUpMorphology()
{
    setUpBranchings(this->branchings);
    for (auto& branch : this->branches){
        setUpSynapseSlots(branch);
    }
}

void BranchedMorphology::setUpSynapseSlots(std::shared_ptr<Branch> branch)
{
    if (this->orderedSynAllocationB){
        this->orderedSynapseAllocation(branch);
    } else if (this->randomSynAllocationB){
        this->randomSynapseAllocation(branch);
    }
}

void BranchedMorphology::SaveParameters(std::ofstream *stream, std::string neuronPreId) {
    Morphology::SaveParameters(stream, neuronPreId);

    *stream << neuronPreId<<"_morphology_branch_length\t\t"<<std::to_string(this->branchLength)<<" #μm";
    *stream << "\t"<<"#Length of each branch.\n";

    *stream << neuronPreId<<"_morphology_synaptic_gap\t\t\t"<<std::to_string(this->synapticGap)<<" #μm";
    *stream << "\t"<<"#Unitary distance between synapse slots.\n";

    *stream << neuronPreId<<"_morphology_distribute_weights\t\t";
    if (this->distributeWeights){
        *stream << "true\t";
    }else{
        *stream<<"false\t"<<std::to_string(this->initialWeights);
    }
    *stream << "\t"<<"#The bool corresponds to distributing weight between min and max uniformally. The number will be the weight assigned to all synapses if bool is false (do not confuse with implementation in MonoDendriteSTDP).\n";
    
    *stream << neuronPreId<<"_morphology_dendrite_branchings\t\t"<<std::to_string(this->branchings);
    *stream << "\t"<<"#This specifies the number of branchings in the dendritic tree FOR EVERY EXISTING BRANCH. Total isolated branches are 2^n. More than 28 will cause integer overflow\n";
    
    *stream << neuronPreId<<"_morphology_synapse_allocation\t\t";
    if (this->orderedSynAllocationB){
        *stream << "ordered\t";
    }else if (this->randomSynAllocationB){
        *stream<<"random\t";
    }
    *stream << "\t"<<"#'ordered' synapse allocation will allocate synapses from the branch node to the end of the branch. 'random' will allocate random positions in each branch\n";

    *stream << neuronPreId<<"_morphology_seed\t\t\t"<<std::to_string(this->seed);//Missing comments
    /*
    *stream << neuronPreId<<"_morphology_branch_allocation\t\t";
    if (this->randomBranchAllocationB){
        *stream << "random\t";
    }else if (this->orderedBranchAllocationB){
        *stream<<"ordered\t";
    }else if (this->setBranchAllocationB){
        *stream<<"set\t";
    }
    *stream << "\t"<<"#Missing comments\n";*/
}


double BranchedMorphology::generateSynapticWeight(){
    double weight{};
    std::uniform_real_distribution<double> distribution(this->minWeight,this->maxWeight);
            if (this->distributeWeights) {
                std::default_random_engine& generatorRef = this->generator;
                weight = distribution(generatorRef);
            } else {
                weight = this->initialWeights; // assuming a range of weight between 0 and 2, weight is initialized to midpoint: 1
            }
        this->weightsSum += weight;
        return weight;
}

std::shared_ptr<SynapseSpine> BranchedMorphology::allocateNewSynapse(HeteroCurrentSynapse &synapse)
{
    std::shared_ptr<SynapseSpine> newSynapse;
    newSynapse = std::make_shared<SynapseSpine>();

        
    newSynapse->setLastSpike(-200.0); // large negative value indicates no spikes of synapse during simulation
    newSynapse->setTheta(0);
    //Step weights has been removed fron here
    newSynapse->setWeight(this->generateSynapticWeight());

    this->weightsSum += newSynapse->getWeight();
    newSynapse->setIdInMorpho(this->synapseIdGenerator++);
    //Branch
    int branch {allocateBranch(synapse)};
    if (branches.at(branch)->openSynapsesSlots.size()==0){
        throw noAllocatableSynapseException();
    }
    //Position
    int position{branches.at(branch)->openSynapsesSlots.front()};
    branches.at(branch)->openSynapsesSlots.pop_front();
    newSynapse->setBranchPositionId(position);
    newSynapse->setDistanceFromNode(position*branches.at(branch)->synapticGap);
    branches.at(branch)->synapseSlotClosedIndex.push_back(position);
    branches.at(branch)->morphoSynapseIDs.push_back(newSynapse->getIdInMorpho());

    newSynapse->setBranchedTrue();
    //Storage (other)
    this->synapseData.push_back(newSynapse);

    this->spikedSynapses.push_back(false);
    //this->integratePostSpike.push_back(false);
    //this->integratePreSpike.push_back(false);

    return newSynapse;
    }

int BranchedMorphology::allocateBranch(const HeteroCurrentSynapse &synapse)
{
        const BranchTargeting& target= synapse.getBranchTarget();
        if (target.setTargetBranch && !(target.targetBranch>this->branches.size())){
            return target.targetBranch;
        } else if (target.randomTargetBranch){
            return randomBranchAllocation();
        } else if (target.orderedTargetBranch){
            return orderedBranchAllocation();
        } else {
            throw noAllocatableSynapseException();
        }
}

int BranchedMorphology::randomBranchAllocation()
{
        std::default_random_engine& generator = this->generator;
        //For now, the distribution will be uniform
        std::uniform_int_distribution<int> branchdsitribution(0,static_cast<int>(branches.size())+1);
        int branchID{branchdsitribution(generator)};
        return branchID;
}

int BranchedMorphology::orderedBranchAllocation()
{
    int branchId{};
    for (auto& branch:branches){
        if (branch->openSynapsesSlots.size()==0){
            continue;
        }
        branchId= branch->branchId ;
        break;
    }
    return branchId;
}

void BranchedMorphology::randomSynapseAllocation(std::shared_ptr<Branch> branch)
{
    //std::default_random_engine& generator = this->generator;
    std::vector<int> possibleSlots(branch->spikedSyn.size());
    std::iota(possibleSlots.begin(), possibleSlots.end(), 0);
   //Now we have our vector from 0 to maxSlots to pull random numbers from
    std::sample(possibleSlots.begin(), possibleSlots.end(),std::back_inserter(branch->openSynapsesSlots),branch->spikedSyn.size(),this->info->globalGenerator);
    //Then I will have to pop_front() in allocateNewSynapse
}

void BranchedMorphology::orderedSynapseAllocation(std::shared_ptr<Branch> branch)
{
    std::deque<int> possibleSlots(branch->spikedSyn.size());
    std::iota(possibleSlots.begin(), possibleSlots.end(), 0);
    //Now we have our vector from 0 to maxSlots to pull random numbers from
    copy(possibleSlots.begin(), possibleSlots.end(), back_inserter(branch->openSynapsesSlots));
    //Then I will have to pop_front() in allocateNewSynapse
}

/*void BranchedMorphology::AlternatedSynapseAllocation(std::shared_ptr<Branch> branch)
{
        for (auto& branch:branches){
        std::deque<int> possibleSlots(branch->spikedSyn.size());
        std::iota(possibleSlots.begin(), possibleSlots.end(), 0);
        copy(possibleSlots.begin(), possibleSlots.end(), back_inserter(branch->openSynapsesSlots));
        }
        //For now it is the same as ordered, the possibility of alternating synapses will probably be more useful with multiple synapses pre-post neuron.
}*/

std::valarray<double> BranchedMorphology::getIndividualSynapticProfile(unsigned long synapseId) const {
    /*
     * returned array organised as follows:
     * item 1: distance of synapse from branch root
     * item 2: value of the synaptic weight
     * item 3: last spike time of the synapse
     * */
    return synapseData.at(synapseId)->getIndividualSynapticProfile();
}

std::valarray<double> BranchedMorphology::getOverallSynapticProfile() const {
    /*
     * returned array organised as follows:
     * item 1: average synaptic weight
     * item 2: total post spikes
     * item 3: total pre spikes
     * */
    std::valarray<double> ret(3);

    double weightSum = std::accumulate(this->synapseData.begin(), this->synapseData.end(), 0.0,
                                       [] (const double acc, const std::shared_ptr<SynapseSpine>& syn) { return acc + syn->getWeight(); });

    ret[0] = weightSum / this->synapseData.size();
    ret[1] = this->totalPostSpikes;
    ret[2] = this->totalPreSpikes;
    return ret;
}

void BranchedMorphology::setUpBranchings(int remainingBranchingEvents, std::vector<int> anteriorBranches)
{
    //This is a recursive function that sets up the branched dendritic tree and is generalized for 0 branchings (1 branch). This function has been unit tested by Antoni.
    remainingBranchingEvents-=1;
    //First call is done with an empty int vector
    for (int i = 0; i < 2;i++) {
        int branchId{this->generateBranchId()};
        this->branches.emplace_back(std::make_shared<Branch>(Branch(this->synapticGap, this->branchLength, anteriorBranches, branchId)));//This vector should be sorted by ID by default (tested).
        //Constructor here
        if(remainingBranchingEvents>0){
            std::vector<int> anteriorBranchesCopy(anteriorBranches);
            anteriorBranchesCopy.push_back(branchId);
            this->setUpBranchings(remainingBranchingEvents, anteriorBranchesCopy);
        }
    }
}