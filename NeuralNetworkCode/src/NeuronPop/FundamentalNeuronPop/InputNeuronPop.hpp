#ifndef _INPUT_NEURON_POP_HEADER_
#define _INPUT_NEURON_POP_HEADER_

#include "../NeuronPop.hpp"

struct Instruction{
    long startTimeStep;//Will have to convert times to timesteps if necessary? Or make a short python programme to do it in a file by itself
    long endTimeStep;
    int fireEveryNSteps;//This variable describes the timesteps between every AP. If 3, it will fire every 3 timesteps, so 2 no and 1 yes.
};

class InputNeuronPop: public NeuronPop {
    protected:
    std::vector<Instruction> inputInstructions;

    public:
    InputNeuronPop(GlobalSimInfo* info, int id);
    ~InputNeuronPop()=default;
}; 

#endif