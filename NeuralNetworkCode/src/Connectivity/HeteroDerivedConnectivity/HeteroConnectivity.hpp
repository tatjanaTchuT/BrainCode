#ifndef _HeterosynapticConnectivity_hpp_derived_
#define _HeterosynapticConnectivity_hpp_derived_

#include <vector>
 
class HeteroConnectivity {
//This is an abstract class
protected:

    explicit HeteroConnectivity(unsigned long noNeuronsPre); //the constructor is protected so that this is functionally an abstract class without a pure virtual function

    std::vector<std::vector<std::pair<unsigned long, unsigned long>>> synapticTargets; //the list with postsynaptic (or target) neurons and syanpseId (Pair<>) for each neuron of the presynaptic population

public:
    ~HeteroConnectivity() = default;

    virtual std::vector<std::pair<unsigned long, unsigned long>>& getSynapticTargets(const unsigned long&);
};

#endif