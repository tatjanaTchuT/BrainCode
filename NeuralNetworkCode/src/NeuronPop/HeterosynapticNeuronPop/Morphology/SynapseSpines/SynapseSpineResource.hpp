#ifndef _SYNAPSE_SPINE_RESOURCE_CLASS_HEADER_
#define _SYNAPSE_SPINE_RESOURCE_CLASS_HEADER_

#include "SynapseSpineBranched.hpp"

class SynapseSpineResource : public SynapseSpineBranched {

    protected:



    public:


    //Profile function
    std::valarray<double> GetIndividualSynapticProfile() const override;
    std::string GetIndividualSynapticProfileHeaderInfo() const override;
};

#endif