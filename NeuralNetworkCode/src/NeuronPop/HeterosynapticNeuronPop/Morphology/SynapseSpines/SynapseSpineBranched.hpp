#ifndef _SYNAPSE_SPINE_BRANCHED_CLASS_HEADER_
#define _SYNAPSE_SPINE_BRANCHED_CLASS_HEADER_

#include "SynapseSpineBase.hpp"

class SynapseSpineBranched : public SynapseSpineBase {

    int branchId{}; //This has to be discrete
    int branchPositionId{}; //This has to be discrete
    int distanceFromNode{}; //This will probably be discrete too, as the distance is id*gap, and gap will be 1 um.
    //int uniqueTreeId{}; Useful only in the indexing of the triangular matrix if implemented
    bool isBranchedBool{true};

    public:
    SynapseSpineBranched()=default;
    SynapseSpineBranched(int distanceFromNode, double lastSpike, double weight, int branchId, int branchPositionId);
    //getters
    int getBranchId() const {return branchId;};
    int getBranchPositionId() const {return branchPositionId;};
    int getDistanceFromNode() const {return distanceFromNode;};
    bool getBranchedBool() const { return isBranchedBool;}
    //setters
    void setBranchId(int idIn){branchId=idIn;};
    void setBranchPositionId(int positionId){branchPositionId=positionId;};
    void setDistanceFromNode(int distance){distanceFromNode=distance;};

    //Profile function
    std::valarray<double> getIndividualSynapticProfile() const override;
    std::string getIndividualSynapticProfileHeaderInfo() const override;
};

#endif