#ifndef HeterosynapticConnectivity_hpp
#define HeterosynapticConnectivity_hpp

#include "../Synapse/Synapse.hpp"
#include "../Synapse/HeteroCurrentSynapse.hpp"

class HeteroRandomConnectivity: public RandomConnectivity {

protected:
    std::vector<std::vector<std::pair<unsigned long, unsigned long>>> synapticTargets; //the list with postsynaptic (or target) neurons and syanpseId (Pair<>) for each neuron of the presynaptic population

public:
    HeteroRandomConnectivity(Synapse* syn, GlobalSimInfo* info);
    ~HeteroRandomConnectivity() override = default;

    void ConnectNeurons() override;

    std::vector<std::pair<unsigned long, unsigned long>> getSynapticTargets(unsigned long);
    void SaveParameters(std::ofstream * stream, const std::string& id_str) override;
    void LoadParameters(std::vector<std::string> *input) override;

    std::string GetTypeStr() override;

    //    unsigned long GetNumberAverageSourceNeurons() override;

    //    int* GetDistributionD(long preNeuronId, long postNeuronId) override;
    //    void SetDistributionD() override;

    //    double* GetDistributionJ(long preNeuronId, long postNeuronId) override;
    //    void SetDistributionJ() override;

};

#endif // HeterosynapticConnectivity_hpp
