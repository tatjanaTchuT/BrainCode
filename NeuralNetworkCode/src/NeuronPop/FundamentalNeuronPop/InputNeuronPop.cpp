#include "./InputNeuronPop.hpp"
#include "InputNeuronPop.hpp"

InputNeuronPop::InputNeuronPop(GlobalSimInfo *info, int id) : NeuronPop (info, id)
{
}

void InputNeuronPop::LoadParameters(std::vector<std::string> *input)
{
}

void InputNeuronPop::SaveParameters(std::ofstream *stream)
{
}

void InputNeuronPop::advect(std::vector<double> *synaptic_dV)
{
}
