
#ifndef LIFNeuronPop_HPP
#define LIFNeuronPop_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "NeuronPop.hpp"
#include "../GlobalFunctions.hpp"


class LIFNeuronPop : public NeuronPop
{
protected:
    int reset_type; //0: hard reset, 1: transfer overshoot
public:
    LIFNeuronPop(GlobalSimInfo * info,int id):NeuronPop(info,id){reset_type = 0;}
    ~LIFNeuronPop(){};

    void advect(std::vector<double> * synaptic_dV);

    std::string GetType(){return str_LIFNeuron;}
    void SaveParameters(std::ofstream * stream);
    void LoadParameters(std::vector<std::string> *input);
};


#endif // LIFNeuronPop_HPP
