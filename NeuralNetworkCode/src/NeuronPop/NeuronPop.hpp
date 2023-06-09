
#ifndef NeuronPop_HPP
#define NeuronPop_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <valarray>
#include <fstream>
#include <string>
#include <memory>
#include "../GlobalFunctions.hpp"
#include "./HeterosynapticNeuronPop/Morphology/SynapseSpine.hpp"

class HeteroCurrentSynapse;
class SynapseSpine;

class NeuronPop
{
protected:

    GlobalSimInfo * info;

    int    identifier;
    unsigned long   noNeurons;
    double refractorySteps;
    double tau_m;
    double v_reset;
    double v_thresh;
    //double r_target;

    int    seed_InitialPotentials,seed_InitialPrevSpike;

    //bool streamingNOutputBool{false};
    bool taskOutputBool{false};

    std::valarray<double>   potential;         // membrane potential
    std::vector<long>       spiker;            // indices of all neurons that have emitted a spike in the previous time step
    std::valarray<long>     previous_spike_step;   // last spike time for every neuron
	std::valarray<double>   x_pos;   // position on the x axis
	std::valarray<double>   y_pos;   // position on the x axis
    //std::valarray<long>     global_id;  // global neuron id (unique across populations)
    void ClearSpiker();

public:
    // constructor
    NeuronPop(GlobalSimInfo * info,int id);
    virtual ~NeuronPop();

    //*******************
    //Get-Functions
    //*******************
    unsigned long GetNoNeurons(){return noNeurons;}
    //long GetGlobalNeuronId(long i){return global_id[i];}
	double get_Xpos(long i) { return x_pos[i]; }
	double get_Ypos(long i) { return y_pos[i]; }

    // those functions return values and addresses ( pay attention here!!!)
    // TODO/Suggestion: Replace pointer return with a return by constant reference.
    // https://github.com/saiftyfirst/BP_Demos/blob/master/C%2B%2B/constRef_vs_pointer.cpp
    std::vector<long>   * GetSpikers(){return &spiker;}

    //long                get_previous_spike_step(long i) {return previous_spike_step[i];}
    double              GetTimeSinceLastSpike(long i) { return (static_cast<double>(info->time_step) - static_cast<double>(previous_spike_step[i])) * info->dt; }

    double              GetPotential(long i) {return potential[i]; }

    virtual std::string GetType() = 0;
    int  const  GetId(){return this->identifier;}  


	//*******************
    //Set-Functions
    //*******************
    void SetNeurons(unsigned long noNeur);
	void SetPosition(long noNeur);
    void SetSeeds(int seed1,int seed2);
    //void SetGlobalNeuronId(long global_neuron_id);

    virtual void advect(std::vector<double> * synaptic_dV) = 0;
    void    LoadParameters(std::vector<std::string> *input,unsigned long noNeurons);
    virtual void LoadParameters(std::vector<std::string> *input);
    virtual void SaveParameters(std::ofstream * stream);

    //To optimize the dynamic_casting in if statements
    virtual bool HasHeterosynapticPlasticity(){return false;}
    virtual bool isBranchedBool() {return false;}
    //All of the following functions throw to ease the virtualization, but it is a bad coding practice
    virtual std::valarray<double> getIndividualSynapticProfile(unsigned long neuronId, unsigned long synapseId);
    virtual std::valarray<double> getOverallSynapticProfile(unsigned long neuronId);
    virtual void recordExcitatorySynapticSpike(unsigned long neuronId, unsigned long synapseId);
    virtual std::shared_ptr<SynapseSpine> allocateNewSynapse(unsigned long neuronId, HeteroCurrentSynapse& syn);

    //Task functions
    //bool const streamingOutput(){return streamingNOutputBool;}//functions
    bool const taskOutput(){return taskOutputBool;}
    std::vector<int> taskOutputVector(); //This function is not complete, will be implemented together with the task framework (supposed to return a vector of 0s and 1s equivalent to spikers)
};


#endif // NeuronPop_HPP
