#ifndef POWERLAWSYNAPSE
#define POWERLAWSYNAPSE

#include <iostream>
#include <vector>
#include <random>
#include <typeinfo>
#include <valarray>
#include "Synapse.hpp"
#include "../NeuronPop/NeuronPop.hpp"
#include "../GlobalFunctions.hpp"


class PowerLawSynapse : public Synapse
{
protected:

	double n;
	int	Naveraging;
	std::valarray<int> spike_count;
	std::valarray<std::valarray<double>> ISI_table;

	virtual void advect_spikers(std::vector<double> * currents, long spiker);
	void advect_finalize(std::vector<std::vector<double>> * waiting_matrix) {};

public:
	PowerLawSynapse(NeuronPop * postNeurons, NeuronPop * preNeurons, GlobalSimInfo * info);
	~PowerLawSynapse();


	//*****************************
	//******* Get Functions *******
	//*****************************
	int                     GetNumberOfDataColumns() { return 1; } //
	std::string             GetDataHeader(int data_column);
	std::string           GetUnhashedDataHeader();
	std::valarray<double>   GetSynapticState(int pre_neuron);
	std::string             GetTypeStr() { return str_powerlawsynapse; };

	void SaveParameters(std::ofstream * stream, std::string id_str);
	void LoadParameters(std::vector<std::string> *input);
};


#endif // POWERLAWSYNAPSE
