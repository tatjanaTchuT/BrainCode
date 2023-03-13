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
    //This class does not inherit LIFNeuronPop too to avoid diamond structure in inheritance, but this class is basically a copy of that class with a couple of extra calls in advect()

protected:
    int reset_type{}; //0: hard reset, 1: transfer overshoot
    double membraneExpDecay{1.0};
public:
    HeteroLIFNeuronPop(GlobalSimInfo * info, int id);
    ~HeteroLIFNeuronPop() override = default;

    void advect(std::vector<double>* synaptic_dV) override;
    std::string GetType() override;
    void SaveParameters(std::ofstream * stream) override;
    void LoadParameters(std::vector<std::string> *input) override;

};

#endif
