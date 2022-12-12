#include <iostream>
#include <vector>
#include <random>
#include <typeinfo>
#include <cstring>
#include <valarray>
#include <fstream>
#include "../GlobalFunctions.hpp"
#include "Connectivity.hpp"

#ifndef RandomConnectivity_hpp
#define RandomConnectivity_hpp

class Synapse;

class RandomConnectivity : public Connectivity
{
protected:

    unsigned long noSourceNeurons; //no of inputs per neuron (synapses for which each neuron is postsynaptic)
    void SetNoSourceNeurons(unsigned long noSN);

public:

    RandomConnectivity(Synapse *syn,GlobalSimInfo  * info);
    ~RandomConnectivity(){}

    void                ConnectNeurons();
    unsigned long       GetNumberAverageSourceNeurons(){return noSourceNeurons;}
    std::string         GetTypeStr(){return str_randomConnectivity;}
    double              GetConnectionProbability();

    void SaveParameters(std::ofstream * stream, std::string id_str) override;
    void LoadParameters(std::vector<std::string> *input);
};

#endif /* RandomConnectivity_hpp */
