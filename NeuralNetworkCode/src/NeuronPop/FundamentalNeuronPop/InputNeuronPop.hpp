#ifndef _INPUT_NEURON_POP_HEADER_
#define _INPUT_NEURON_POP_HEADER_

#include "../NeuronPop.hpp"

struct Instruction{
    int neuronId;//The neuron specific to the instruction (-1 is equivalent to all?)
    long startTimeStep;//Will have to convert times to timesteps if necessary? Or make a short python programme to do it in a file by itself
    long endTimeStep;
    int fireEveryNSteps;//This variable describes the timesteps between every AP. If 3, it will fire every 3 timesteps, so 2 no and 1 yes.
    bool completed{false};
};

class InputNeuronPop: public NeuronPop {
    protected:
    std::vector<std::vector<Instruction>> inputInstructions;//A vector of instructions for each neuron of the population
    bool instructionBasedBool{false};
    bool spikerListFiringBasedBool{false};

    public:
    InputNeuronPop(GlobalSimInfo* info, int id);
    ~InputNeuronPop()=default;

    void LoadParameters(std::vector<std::string> *input) override;
    void SaveParameters(std::ofstream * stream) override;
    
    void advect(std::vector<double> * synaptic_dV) override; //Here I will have to set the spikers depending on the instructions (using modular with fireEveryNsteps)
    //Possibility of instead of instructions, giving the spikers per timestep (probably calculate the raw firing first, then the spikers per timestep) and set them in the advect function
}; 

#endif