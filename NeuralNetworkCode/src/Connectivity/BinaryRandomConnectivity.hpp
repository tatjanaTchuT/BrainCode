#include <iostream>
#include <vector>
#include <random>
#include <typeinfo>
#include <cstring>
#include <vector>
#include <fstream>
#include "../GlobalFunctions.hpp"
#include "Connectivity.hpp"

#ifndef BinaryRandomConnectivity_hpp
#define BinaryRandomConnectivity_hpp

class Synapse;

class BinaryRandomConnectivity : public Connectivity
{
protected:

    double connectionProbability;

public:

    BinaryRandomConnectivity(Synapse *syn,GlobalSimInfo  * info);
    ~BinaryRandomConnectivity() override = default;

    void ConnectNeurons() override;
    const std::string GetTypeStr() override { return str_binaryrandomConnectivity; }

    unsigned long GetNumberAverageSourceNeurons() override;

    void SaveParameters(std::ofstream* stream, std::string id_str) override;
    void LoadParameters(std::vector<std::string> *input) override;
};

#endif /* BinaryRandomConnectivity_hpp */
