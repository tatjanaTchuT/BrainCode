#ifndef MONGILLOSYNAPSECONTINUOUS_HPP
#define MONGILLOSYNAPSECONTINUOUS_HPP

#include <iostream>
#include <vector>
#include <random>
#include <typeinfo>
#include <valarray>
#include "Synapse.hpp"
#include "../NeuronPop/NeuronPop.hpp"
#include "../GlobalFunctions.hpp"


class MongilloSynapseContinuous : public Synapse
{
protected:

    double u;
    double tau_f;  // in units of time steps
    double tau_d;  // in units of time steps

    std::valarray<std::valarray<double>> x, y, spike_submitted;

    int seed;
    std::default_random_engine generator;
    std::uniform_real_distribution<double> uni_distribution;

    virtual void advect_spikers(std::vector<double> * currents, long spiker);
    //void advect_finalize (std::vector<double> * synaptic_dV, std::vector<std::vector<std::vector<double>>> * waiting_matrix){}
    void advect_finalize(std::vector<std::vector<double>> * waiting_matrix){}

    virtual void   TransmitSpike(std::vector<double> * currents, long targetId,long spikerId);

    void SetSeed(int s);

public:
    MongilloSynapseContinuous(NeuronPop * postNeurons,NeuronPop * preNeurons,GlobalSimInfo * info);
    ~MongilloSynapseContinuous();

    void ConnectNeurons();

    void SetSeed(std::default_random_engine *generator);
    //*****************************
    //******* Get Functions *******
    //*****************************
    int                     GetNumberOfDataColumns(){return 4;} // J, <y>, <x>, <submitted_spikes>
    std::string             GetDataHeader(int data_column);
	std::string           GetUnhashedDataHeader();
    std::valarray<double>   GetSynapticState(int pre_neuron);
    std::string             GetTypeStr(){return str_mongilloSynapseContinuous;}

    void SaveParameters(std::ofstream * stream,std::string id_str);
    void LoadParameters(std::vector<std::string> *input);

};

#endif // MONGILLOSYNAPSECONTINUOUS_HPP
