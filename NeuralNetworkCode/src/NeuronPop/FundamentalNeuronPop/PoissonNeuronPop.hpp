
#ifndef PoissonNeuronPop_HPP
#define PoissonNeuronPop_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <random>
#include "../NeuronPop.hpp"
#include "../../GlobalFunctions.hpp"


class PoissonNeuronPop : public NeuronPop
{
protected:
    double r_target{}; // target firing rate
    double lambda{};
    bool inputDependant{true};
    int seed{};
    std::default_random_engine generator;
    std::uniform_real_distribution<double> uni_distribution;

public:
    PoissonNeuronPop(GlobalSimInfo * info,int id);
    virtual ~PoissonNeuronPop()=default;

    void advect(std::vector<double> * synaptic_dV);

    std::string GetType(){return str_PoissonNeuron;}
    void SaveParameters(std::ofstream * stream);
    void LoadParameters(std::vector<std::string> *input);
};


#endif // PoissonNeuronPop_HPP
