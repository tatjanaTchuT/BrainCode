#ifndef Connectivity_hpp
#define Connectivity_hpp


#include <iostream>
#include <vector>
#include <random>
#include <typeinfo>
#include <cstring>
#include <valarray>
#include <fstream>
#include "../GlobalFunctions.hpp"

class Synapse;
struct SynapseData_STP;

class Connectivity{
protected:

    GlobalSimInfo               * info;
    bool                        fixSeed;
    int                         seed;
    std::default_random_engine  generator;

    Synapse                     * synapse;
    std::vector<unsigned long>  * target_id; //the list with postsynaptic (or target) neurons for each neuron of the presynaptic population
    std::vector<int>            * D_distribution; // the list with delays D that are associated with the synapses betweeen the source and its target neurons
    std::vector<double>         * J_distribution; // list of synaptic weights J
//	bool						HasPot;
	bool						HasDdistribution;
    bool                        HasJDistribution;
public:

    Connectivity(Synapse *syn,GlobalSimInfo  * info);
    virtual ~Connectivity();

    std::vector<unsigned long>       * GetTargetList(long preNeuronId){return &target_id[preNeuronId];}
    virtual unsigned long   GetNumberAverageSourceNeurons() =0;
    virtual std::string     GetTypeStr() = 0;

    virtual void ConnectNeurons() = 0;
    void    WriteConnectivity(std::string filename,unsigned long noNeuronsConnectivity);
    void    WriteDistributionD(std::string filename, unsigned long noNeuronsDelay);
    void    WriteDistributionJ(std::string filename, unsigned long noNeuronsJPot);

    virtual void SaveParameters(std::ofstream * stream,std::string id_str);
    virtual void LoadParameters(std::vector<std::string> *input);

    int     GetSeed()       {return seed;}
    void    SetSeed(std::default_random_engine *seedGenerator);
    void    SetSeed(int s);


	int				   * GetDistributionD(long preNeuronId, long postNeuronId);
    void                 SetDistributionD();

	double             * GetDistributionJ(long preNeuronId, long postNeuronId);
    void                 SetDistributionJ();

    void Test();


};

#endif /* Connectivity_hpp */
