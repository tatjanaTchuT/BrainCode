#ifndef EXPONENTIALCURRENTSYNAPSE
#define EXPONENTIALCURRENTSYNAPSE

#include <iostream>
#include <vector>
#include <random>
#include <typeinfo>
#include <valarray>
#include "Synapse.hpp"
#include "../NeuronPop/NeuronPop.hpp"
#include "../GlobalFunctions.hpp"

//*****************************************************************************
// Incoming synaptic current to neuron i:
//    ds_i/dt      = - s_i/tau + sum[delta(t-t_incomingSpikes_i - t_synDelay)]
//    dV_{syn,i}/dt  = J/tau*s_i(t)
// here, we save outgoing current from each neuron, then sum up respectively
// this can be done due to the linearity of s_i - evolution - equation
//*****************************************************************************

class ExponentialCurrentSynapse : public Synapse
{
protected:
	double	Tau;
	void advect_spikers(std::vector<double> * currents, long spiker); // here, only expAddon is advected
	void advect_finalize(std::vector<std::vector<double>> * waiting_matrix) {}
    void ResetWaitingMatrixEntry();  // synaptic_dV is updated here!
	void resetcumulatedDV();

public:
    ExponentialCurrentSynapse(NeuronPop * postNeurons,NeuronPop * preNeurons,GlobalSimInfo * info);

    //*****************************
    //******* Get Functions *******
    //*****************************
    int                     GetNumberOfDataColumns(){ return 1;}
    std::string             GetDataHeader(int data_column);
	std::string				GetUnhashedDataHeader();
    std::valarray<double>   GetSynapticState(int pre_neuron);
    std::string             GetTypeStr(){return str_exponentialCurrentSynapse;};

    void SaveParameters(std::ofstream * stream,std::string id_str);
    void LoadParameters(std::vector<std::string> *input);

    //~ExponentialCurrentSynapse(){ delete expAddon;}
};


#endif // EXPONENTIALCURRENTSYNAPSE
