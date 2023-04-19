#ifndef MONGILLOSYNAPSECONTINUOUS_HPP
#define MONGILLOSYNAPSECONTINUOUS_HPP

#include <iostream>
#include <vector>
#include <random>
#include <typeinfo>
#include <vector>
#include "Synapse.hpp"
#include "../NeuronPop/NeuronPop.hpp"
#include "../GlobalFunctions.hpp"


class MongilloSynapseContinuous : public Synapse
{
protected:

    double u;
    double tau_f;  // in units of time steps
    double tau_d;  // in units of time steps

    std::vector<std::vector<double>> x, y, spike_submitted;

    int seed;
    std::default_random_engine generator;
    std::uniform_real_distribution<double> uni_distribution;

    void advectSpikers(std::vector<double>& currents, long spiker) override;
    //void advect_finalize (std::vector<double> * synaptic_dV, std::vector<std::vector<std::vector<double>>> * waiting_matrix){}
    //void advect_finalize(std::vector<std::vector<double>> * waiting_matrix) override {}

    virtual void TransmitSpike(std::vector<double>& currents, long targetId,long spikerId);

    void SetSeed(int s);

public:
    MongilloSynapseContinuous(NeuronPop * postNeurons,NeuronPop * preNeurons,GlobalSimInfo * info);
    ~MongilloSynapseContinuous() override = default;

    void ConnectNeurons() override;

    void SetSeed(std::default_random_engine *generator) override;
    //*****************************
    //******* Get Functions *******
    //*****************************
    int GetNumberOfDataColumns() override { return 4; } // J, <y>, <x>, <submitted_spikes>
    std::string GetDataHeader(int data_column) override;
	std::string GetUnhashedDataHeader() override;
    std::vector<double> GetSynapticState(int pre_neuron) override;
    const std::string GetTypeStr() override { return str_mongilloSynapseContinuous; }

    void SaveParameters(std::ofstream * stream,std::string id_str) override;
    void LoadParameters(std::vector<std::string> *input) override;

};

#endif // MONGILLOSYNAPSECONTINUOUS_HPP
