#ifndef HETEROCURRENTSYNAPSE
#define HETEROCURRENTSYNAPSE

#include <cassert>

#include "Synapse.hpp"
#include "../NeuronPop/HeterosynapricNeuronPop/HeteroLIFNeuronPop.hpp"
#include "../Connectivity/HeteroRandomConnectivity.hpp"

class HeteroCurrentSynapse : public Synapse {
protected:

    std::vector<std::shared_ptr<SynapseExt>> synapseData{};

    void advect_finalize(std::vector<std::vector<double>> * waiting_matrix) override;
    void advect_spikers (std::vector<double>& currents, long spiker) override;

public:

    HeteroCurrentSynapse(NeuronPop* postNeurons, NeuronPop* preNeurons, GlobalSimInfo * info);

    std::string GetTypeStr() override { return str_heteroSynapse; };
    unsigned long allocateSynapse(unsigned long preId, unsigned long postId);

    std::string GetDataHeader(int data_column) override;
	std::string GetUnhashedDataHeader() override;
    std::valarray<double> GetSynapticState(int pre_neuron) override;
    int GetNumberOfDataColumns() override {return 1;};

    void SaveParameters(std::ofstream * stream,std::string id_str) override;
    void LoadParameters(std::vector<std::string> *input) override;

    void advect(std::vector<double> *synaptic_dV) override;

    // Testing
    friend std::vector<std::pair<unsigned long, unsigned long>> getSynapticTargets(HeteroCurrentSynapse&, unsigned long);
    friend std::vector<SynapseExt> getSynapseData(HeteroCurrentSynapse&);
};

std::vector<std::pair<unsigned long, unsigned long>> getSynapticTargets(HeteroCurrentSynapse&, unsigned long);
std::vector<SynapseExt> getSynapseData(HeteroCurrentSynapse&);

#endif
