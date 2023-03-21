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

void BranchedResourceSTDPAsymmetric::advect()
{
    
}

void BranchedResourceSTDPAsymmetric::DeleteEffects()
{
//     std::sort(removelist.begin(), removelist.end());  // Make sure the container is sorted
// for (auto &i = removelist.rbegin(); i != removelist.rend(); ++ i)
// {
//     a.erase(a.begin() + *i);
// }
}

std::shared_ptr<BaseSynapseSpine> BranchedResourceSTDPAsymmetric::AllocateNewSynapse(HeteroCurrentSynapse &synapse)
{
    //here I have to set the maxcount of the spine to maxCount too 
    //Here sum over the branches.
    return std::shared_ptr<BaseSynapseSpine>();
}
