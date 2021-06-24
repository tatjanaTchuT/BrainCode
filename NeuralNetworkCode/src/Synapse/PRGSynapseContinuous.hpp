#ifndef PRGSYNAPSECONTINUOUS_HPP
#define PRGSYNAPSECONTINUOUS_HPP

#include <iostream>
#include <vector>
#include <random>
#include <typeinfo>
#include <valarray>
#include "Synapse.hpp"
#include "MongilloSynapseContinuous.hpp"
#include "../GlobalFunctions.hpp"
#include "../NeuronPop/NeuronPop.hpp"


class PRGSynapseContinuous : public MongilloSynapseContinuous
{
protected:

    std::valarray<std::valarray<double>> l;

    double tau_l;       // Decay time constant
    double M;           // LPA2 filling probability per transmitted spike
    double Delta_U;     // Effect of LPA on U
	double Delta_tau_f; // Effect of LPA on tau_f

    void   TransmitSpike(std::vector<double> * currents,long targetId,long spikerId);
	void   advect_spikers(std::vector<double> * currents, long spiker);
	void   ConnectNeurons();

public:
    PRGSynapseContinuous(NeuronPop * postNeurons,NeuronPop * preNeurons,GlobalSimInfo * info);
    ~PRGSynapseContinuous();

    int                     GetNumberOfDataColumns() {return 5;} // J, <y>, <x>, <l>, <submitted_spikes>
	std::string             GetDataHeader(int data_column);
	std::string				GetUnhashedDataHeader();
	std::valarray<double>   GetSynapticState(int pre_neuron);
    std::string             GetTypeStr(){return str_prgSynapseContinuous;}

    void                    SaveParameters(std::ofstream * stream,std::string id_str);
    void                    LoadParameters(std::vector<std::string> *input);

};

#endif // PRGSYNAPSECONTINUOUS_HPP
