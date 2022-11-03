//
// Created by Saif Ahmed on 07.09.21.
//

#ifndef NEURONAL_NETWORK_SIMULATIONS_UNIFORMHETEROSYNAPTICCONNECTIVITY_H
#define NEURONAL_NETWORK_SIMULATIONS_UNIFORMHETEROSYNAPTICCONNECTIVITY_H

#include "HeteroConnectivity.hpp"

class UniformHeteroConnectivity: public HeteroConnectivity {

protected:
    double synapticGap{};

public:
    UniformHeteroConnectivity(Synapse *syn,GlobalSimInfo *info);
    ~UniformHeteroConnectivity() override = default;

    // overrides of HeterosynapticConnectivity methods
    std::shared_ptr<SynapseExtension> allocationScheme(unsigned long, unsigned long, std::shared_ptr<SynapseAllocationState>) override;
    void SaveParameters(std::ofstream * stream,std::string id_str) override;
    void LoadParameters(std::vector<std::string> *input) override;

    // overrides of Connectivity methods
    std::string GetTypeStr() override;
    unsigned long GetNumberAverageSourceNeurons() override;

};


#endif //NEURONAL_NETWORK_SIMULATIONS_UNIFORMHETEROSYNAPTICCONNECTIVITY_H
