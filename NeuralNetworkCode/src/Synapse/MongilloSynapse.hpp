
#ifndef MONGILLOSYNAPSE_CURRENTBASED
#define MONGILLOSYNAPSE_CURRENTBASED

#include <iostream>
#include <vector>
#include <random>
#include <typeinfo>
#include <valarray>
#include "Synapse.hpp"
#include "../NeuronPop/NeuronPop.hpp"
#include "../GlobalFunctions.hpp"

/*struct SynapseData_STP {
    bool x = 0;
    bool y = 0;
    bool spike_submitted = 0;
    bool gamma_s = false; //this is only used for PRG Synapses (can be removed for Mongillo only)
};*/

class MongilloSynapse : public Synapse
{
protected:

    double u;
    double tau_f;  // in units of time steps
    double tau_d;  // in units of time steps

    //std::valarray<std::valarray<SynapseData_STP>> synapseData;
    std::valarray<std::valarray<bool>> x,y,spike_submitted;

    int seed;
    std::default_random_engine generator;
    std::uniform_real_distribution<double> uni_distribution;


    virtual void advect_spikers(std::vector<double> * currents, long spiker);
    void advect_finalize(std::vector<std::vector<double>> * waiting_matrix){}
    virtual void   TransmitSpike(std::vector<double> * currents, long targetId,long spikerId);

    void SetSeed(int s);
public:
    MongilloSynapse(NeuronPop * postNeurons,NeuronPop * preNeurons,GlobalSimInfo * info);
    ~MongilloSynapse();

    void ConnectNeurons();

    void SetSeed(std::default_random_engine *generator);
    //*****************************
    //******* Get Functions *******
    //*****************************
    int                     GetNumberOfDataColumns()       {return 4;} // J, <y>, <x>, <submitted_spikes>
    std::string             GetDataHeader(int data_column);
	std::string				GetUnhashedDataHeader();
    std::valarray<double>   GetSynapticState(int pre_neuron);
    std::string             GetTypeStr(){return str_mongilloSynapse;};

    void SaveParameters(std::ofstream * stream,std::string id_str);
    void LoadParameters(std::vector<std::string> *input);
};


#endif // MONGILLOSYNAPSE_CURRENTBASED
