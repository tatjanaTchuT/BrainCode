#include "../../Connectivity/HeteroDerivedConnectivity/HeteroConnectivity.hpp"
#include <vector>

HeteroConnectivity::HeteroConnectivity(unsigned long noNeuronsPre):synapticTargets(noNeuronsPre){
}

std::vector<std::pair<unsigned long, unsigned long>>& HeteroConnectivity::getSynapticTargets(const unsigned long& preId) {
    return (this->synapticTargets.at(preId));
}