//
// Created by Saif Ahmed on 20.09.21.
//

#ifndef NEURALNETWORKCODE_HETEROPOISSONNEURONPOP_H
#define NEURALNETWORKCODE_HETEROPOISSONNEURONPOP_H

#include "HeteroNeuronPop.hpp"

class HeteroPoissonNeuronPop: public HeteroNeuronPop {

protected:
    double r_target; // target firing rate
    int seed;
    std::default_random_engine generator;
    std::uniform_real_distribution<double> uni_distribution;

public:
    HeteroPoissonNeuronPop(GlobalSimInfo *info, int id);
    ~HeteroPoissonNeuronPop() = default;

    // overrides from HeteroNeuronPop
    void SaveParameters(std::ofstream * stream) override;
    void LoadParameters(std::vector<std::string> *input) override;

    // overrides from NeuronPop
    void advect(std::vector<double> * synaptic_dV) override;
    std::string GetType() override;
};


#endif //NEURALNETWORKCODE_HETEROPOISSONNEURONPOP_H
