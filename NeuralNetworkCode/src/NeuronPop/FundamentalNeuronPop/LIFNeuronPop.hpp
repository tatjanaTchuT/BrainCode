
#ifndef LIFNeuronPop_HPP
#define LIFNeuronPop_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "../NeuronPop.hpp"
#include "../../GlobalFunctions.hpp"


class LIFNeuronPop : public NeuronPop
{
protected:
    int reset_type; //0: hard reset, 1: transfer overshoot
    double membraneExpDecay{1.0};
public:
    LIFNeuronPop(GlobalSimInfo * info,int id): NeuronPop(info,id) {
        reset_type = 0;
    }
    ~LIFNeuronPop() override = default;

    void advect(std::vector<double> * synaptic_dV) override;
    std::string GetType() override {return str_LIFNeuron;}
    void SaveParameters(std::ofstream * stream) override;
    void LoadParameters(std::vector<std::string> *input) override;
};


#endif // LIFNeuronPop_HPP
