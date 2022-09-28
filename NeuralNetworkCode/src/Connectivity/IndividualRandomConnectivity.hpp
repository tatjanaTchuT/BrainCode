#include <iostream>
#include <vector>
#include <random>
#include <typeinfo>
#include <cstring>
#include <valarray>
#include <fstream>
#include "../GlobalFunctions.hpp"
#include "Connectivity.hpp"

#ifndef IndividualRandomConnectivity_hpp
#define IndividualRandomConnectivity_hpp

class Synapse;

class IndividualRandomConnectivity : public Connectivity
{
protected:

    unsigned long noSourceNeurons; //no of inputs per neuron (synapses for which each neuron is postsynaptic)
    std::vector<std::vector<int>> connectivity_matrix;
    std::string connectionProbFile;
    void SetNoSourceNeurons(unsigned long noSN);

public:

    IndividualRandomConnectivity(Synapse* syn, GlobalSimInfo* info);
    ~IndividualRandomConnectivity() {}

    void                ConnectNeurons();
    unsigned long       GetNumberAverageSourceNeurons() { return noSourceNeurons; }
    std::string         GetTypeStr() { return str_individualRandomConnectivity; }
    double              GetConnectionProbability();
    double              GetConnectionProbabilityFromWeight(double weight);
    void                GetConnectionWeightsFromFile(std::string filepath);

    void SaveParameters(std::ofstream* stream, std::string id_str);
    void LoadParameters(std::vector<std::string>* input);

    void                SetDistributionJ();
};

#endif /* RandomConnectivity_hpp */
