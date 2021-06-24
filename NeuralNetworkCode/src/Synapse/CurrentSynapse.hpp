#ifndef CURRENTSYNAPSE
#define CURRENTSYNAPSE

#include <iostream>
#include <vector>
#include <random>
#include <typeinfo>
#include <valarray>
#include "Synapse.hpp"
#include "../NeuronPop/NeuronPop.hpp"
#include "../GlobalFunctions.hpp"

class CurrentSynapse : public Synapse
{
protected:
    void advect_spikers (std::vector<double> * currents, long spiker);
    //void advect_finalize(std::vector<double> * synaptic_dV, std::vector<std::vector<std::vector<double>>> * waiting_matrix){}
    void advect_finalize(std::vector<std::vector<double>> * waiting_matrix){}

public:
    CurrentSynapse(NeuronPop * postNeurons,NeuronPop * preNeurons,GlobalSimInfo * info);

    //*****************************
    //******* Get Functions *******
    //*****************************
    int                   GetNumberOfDataColumns(){ return 1;}
    std::string           GetDataHeader(int data_column);
	std::string           GetUnhashedDataHeader();
    std::valarray<double> GetSynapticState(int pre_neuron);
    std::string           GetTypeStr(){return str_currentSynapse;};

    void SaveParameters(std::ofstream * stream,std::string id_str);
    void LoadParameters(std::vector<std::string> *input);
};


#endif // CURRENTSYNAPSE
