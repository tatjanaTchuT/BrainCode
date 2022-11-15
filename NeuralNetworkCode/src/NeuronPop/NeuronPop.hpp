#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <valarray>
#include <fstream>
#include <string>
#include "../GlobalFunctions.hpp"

#ifndef NeuronPop_HPP
#define NeuronPop_HPP


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
    double              GetTimeSinceLastSpike(long i){ return double(info->time_step - previous_spike_step[i])*info->dt;}

    double              GetPotential(long i) {return potential[i]; }

    virtual std::string GetType() = 0;
    int     GetId(){return this->identifier;}


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

};


#endif // NeuronPop_HPP
