#include <iostream>
#include <vector>
#include <random>
#include <typeinfo>
#include <cstring>
#include <valarray>
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
    ~BinaryRandomConnectivity(){}

    void                ConnectNeurons();
    std::string         GetTypeStr(){return str_binaryrandomConnectivity;}

    unsigned long       GetNumberAverageSourceNeurons();

    void SaveParameters(std::ofstream * stream,std::string id_str);
    void LoadParameters(std::vector<std::string> *input);
};

#endif /* BinaryRandomConnectivity_hpp */
