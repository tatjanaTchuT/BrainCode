#ifndef _SYNAPSE_SPINE_RESOURCE_CLASS_HEADER_
#define _SYNAPSE_SPINE_RESOURCE_CLASS_HEADER_

#include "SynapseSpineBranched.hpp"

class SynapseSpineResource : public SynapseSpineBranched {


public:


    //Profile function
    std::valarray<double> getIndividualSynapticProfile() const override;
    std::string getIndividualSynapticProfileHeaderInfo() const override;
};

#endif