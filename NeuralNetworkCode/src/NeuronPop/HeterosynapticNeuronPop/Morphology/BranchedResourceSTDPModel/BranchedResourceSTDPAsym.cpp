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
        this->resourceBranches.emplace_back(std::make_shared<ResourceBranch>(ResourceBranch(this->synapticGap, this->branchLength, anteriorBranches, branchId, this->timestepWindowSize, maxCount)));//This vector should be sorted by ID by default (tested).
        this->branches.push_back(static_cast<std::shared_ptr<Branch>>(this->resourceBranches.back()));
        //Constructor here
        if(remainingBranchingEvents>0){
            std::vector<int> anteriorBranchesCopy(anteriorBranches);
            anteriorBranchesCopy.push_back(branchId);
            this->SetUpBranchings(remainingBranchingEvents, anteriorBranchesCopy);
        }
    }
}

void BranchedResourceSTDPAsymmetric::SetUpKernelHashTable()
{
}

void BranchedResourceSTDPAsymmetric::advect()
{
    //Here I have to take into account the circular behaviour: from t=10 s=1 to s=10 t=1, sort of symmetric. (Each syn gap you move, you lose 10 dt of time)
        //If STDPDEpression count is not 10, immediately trigger STDP on spine trigger (while also flagging)
        //If first precount is bigger than twice the STDP depression count, depression. Otherwise potentiation.
    //When iterating over the synapse vector (whichever it might be inside a branch), always compare to .begin() or .end(). 
    //If begin(), call kernel and then break the for loop. If end() break the loop
}

void BranchedResourceSTDPAsymmetric::STDPPotentiation()
{
    //If post spike, apply all stimms on positive mode (remember the coded function in spines)
}

void BranchedResourceSTDPAsymmetric::STDPDepression()
{
     //If count <10, apply stimms in negative mode. Basically input -1/STDPratio to the spine function
}

void BranchedResourceSTDPAsymmetric::DetectPairing(int synapseSpineIDinMorpho)
{
}

void BranchedResourceSTDPAsymmetric::SetEffect()
{
}

double BranchedResourceSTDPAsymmetric::SpaceTimeKernel(int synapseID)
{
    //The iteration
    //Here for every synapse inside the synapse's kernel that has an active counter (count!=countMax) we get the time kernel and then apply the space kernel
    //Take unto account the synaptic GAP and DT!!! 
    return double();
}
double BranchedResourceSTDPAsymmetric::CallKernelHashTable(int distanceToCenter, int timeDifference)
{
    //Matrix is symmetric!! Only have to do half!
    //Here we tabulate for every distance from center a equivalency between counts and effects (reverse as counts go up to maxCount)
    //The pair consists of ns and ks. The function has to obtain the count from synapse and distance?
    //OR the hashtable contains the ns and ks multipliers (for now they are the same)
    return double();
}

void BranchedResourceSTDPAsymmetric::TickCounts()
{
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
//     std::sort(removelist.begin(), removelist.end());  // Make sure the container is sorted
// for (auto &i = removelist.rbegin(); i != removelist.rend(); ++ i)
// {
//     a.erase(a.begin() + *i);
// }
}

void BranchedResourceSTDPAsymmetric::Reset()
{
}

void BranchedResourceSTDPAsymmetric::RecalcAlphas(std::shared_ptr<ResourceBranch> branch)
{
    //Here we just need to apply all delta alphas, decay them (before makes more sense, the bump has delta t zero.), sum the result to stationary alpha, then update alpha sums? Yes
}

void BranchedResourceSTDPAsymmetric::RecalcWeights(std::shared_ptr<ResourceBranch> branch)
{
}

void BranchedResourceSTDPAsymmetric::RecalcAlphaSums(std::shared_ptr<ResourceBranch> branch)
{
}

void BranchedResourceSTDPAsymmetric::RecordPostSpike()
{
}

void BranchedResourceSTDPAsymmetric::RecordExcitatoryPreSpike(unsigned long synSpikerId)
{
}

std::valarray<double> BranchedResourceSTDPAsymmetric::GetIndividualSynapticProfile(unsigned long synapseId) const
{
    return std::valarray<double>();
}

std::shared_ptr<BaseSynapseSpine> BranchedResourceSTDPAsymmetric::AllocateNewSynapse(HeteroCurrentSynapse &synapse)
{
    //here I have to set the maxcount of the spine to maxCount too 
    //Here sum over the branches.
    return std::shared_ptr<BaseSynapseSpine>();
}
