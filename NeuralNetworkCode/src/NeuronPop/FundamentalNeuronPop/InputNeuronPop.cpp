#include "./InputNeuronPop.hpp"
#include "InputNeuronPop.hpp"

InputNeuronPop::InputNeuronPop(GlobalSimInfo *info, int id) : NeuronPop (info, id)
{
    //connectionProbFile = info->pathTo_inputFile + "Connectivity_Matrix_"+idStr+".txt";
    //Then just open and read, with bools for raw spikers or instruction-based
}

void InputNeuronPop::LoadParameters(std::vector<std::string> *input)
{
}

void InputNeuronPop::SaveParameters(std::ofstream *stream)
{
}

void InputNeuronPop::advect(std::vector<double> *synaptic_dV)
{
    ClearSpiker();
    if (spikerListFiringBasedBool){
        ReadSpikersFromFile();
    } else if(instructionBasedBool){
        GenerateSpikersFromInstructions();
    } else {
        throw;
    }
}

void InputNeuronPop::GenerateSpikersFromInstructions()
{
}

void InputNeuronPop::ReadSpikersFromFile()
{
}
