#ifndef HeteroNeuronPop_HPP
#define HeteroNeuronPop_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include "../FundamentalNeuronPop/LIFNeuronPop.hpp"
#include "./HeteroNeuronPop.hpp"
#include "../../GlobalFunctions.hpp"


class HeteroLIFNeuronPop : public HeteroNeuronPop {

protected:
    int reset_type{}; //0: hard reset, 1: transfer overshoot

public:
    HeteroLIFNeuronPop(GlobalSimInfo * info, int id);
    ~HeteroLIFNeuronPop() override = default;

    void advect(std::vector<double>* synaptic_dV) override;
    std::string GetType() override;
    void SaveParameters(std::ofstream * stream) override;
    void LoadParameters(std::vector<std::string> *input) override;

};

#endif
