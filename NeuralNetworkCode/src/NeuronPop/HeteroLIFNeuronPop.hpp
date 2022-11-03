//
// Created by Saif Ahmed on 07.09.21.
//

#ifndef NEURONAL_NETWORK_SIMULATIONS_HETEROLIFNEURONPOP_H
#define NEURONAL_NETWORK_SIMULATIONS_HETEROLIFNEURONPOP_H

#include "HeteroNeuronPop.hpp"

class HeteroLIFNeuronPop: public HeteroNeuronPop {

protected:
    int reset_type; //0: hard reset, 1: transfer overshoot

public:
    HeteroLIFNeuronPop(GlobalSimInfo *info, int id);
    ~HeteroLIFNeuronPop() override = default;

    // overrides from HeteroNeuronPop
    void SaveParameters(std::ofstream * stream) override;
    void LoadParameters(std::vector<std::string> *input) override;

    // overrides from NeuronPop
    void advect(std::vector<double> * synaptic_dV) override;
    std::string GetType() override;
};


#endif //NEURONAL_NETWORK_SIMULATIONS_HETEROLIFNEURONPOP_H
