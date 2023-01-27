#include "SimplePOnlyB.hpp"

SimplePlasticityOnlyBranch::SimplePlasticityOnlyBranch(GlobalSimInfo *info):BranchedMorphology(info)
{
}

void SimplePlasticityOnlyBranch::LoadParameters(std::vector<std::string> *input)
{
    std::string name;
    std::vector<std::string> values;
    
    BranchedMorphology::LoadParameters(input);

    for (auto & it : *input) {
        SplitString(&it,&name,&values);
        if(name.find("available_weight") != std::string::npos){
            this->availableBranchResources = std::stod(values.at(0));
        }
    }

}
void SimplePlasticityOnlyBranch::SaveParameters(std::ofstream *stream, std::string neuronPreId)
{
    BranchedMorphology::SaveParameters(stream, neuronPreId);
    *stream << neuronPreId<<"_morphology_available_weight\t\t"<<std::to_string(this->availableBranchResources);
    *stream << "\t"<<"#Total weight available to be distributed among synapses per time-step.\n";
}

void SimplePlasticityOnlyBranch::advect()
{
    Morphology::timeDecay();
    for (auto branch : branches){
        double deltaW{this->availableBranchResources/std::accumulate(branch->spikedSyn.begin(), branch->spikedSyn.end(),0)};

        for (int synapseSlotId : branch->synapseSlotClosedIndex){
            if (branch->spikedSyn.at(synapseSlotId)){
                synapseData.at(branch->morphoSynapseIDs.at(synapseSlotId))->addToWeight(deltaW);
            }
        }
    }
}
