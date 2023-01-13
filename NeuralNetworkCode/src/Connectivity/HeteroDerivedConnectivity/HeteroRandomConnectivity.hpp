#ifndef _Heterosynaptic_RANDOM_Connectivity_hpp_derived_
#define _Heterosynaptic_RANDOM_Connectivity_hpp_derived_

#include <vector>
#include <string>
#include <utility>
//Do not include synapse related hpps, it creates an include loop
#include "../../Connectivity/RandomConnectivity.hpp"
#include "../../Connectivity/HeteroDerivedConnectivity/HeteroConnectivity.hpp"

class HeteroRandomConnectivity: public RandomConnectivity, public HeteroConnectivity {


public:
    HeteroRandomConnectivity(Synapse* syn, GlobalSimInfo* info);
    ~HeteroRandomConnectivity() override = default;

    void ConnectNeurons() override;

    const std::string GetTypeStr() override;

    //To solve multiple inheritance problem

    virtual std::vector<std::pair<unsigned long, unsigned long>>& getSynapticTargets(const unsigned long&) override;

    //    unsigned long GetNumberAverageSourceNeurons() override;

    //    int* GetDistributionD(long preNeuronId, long postNeuronId) override;
    //    void SetDistributionD() override;

    //    double* GetDistributionJ(long preNeuronId, long postNeuronId) override;
    //    void SetDistributionJ() override;

};

#endif
